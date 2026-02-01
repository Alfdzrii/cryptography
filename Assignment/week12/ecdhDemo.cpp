#include <iostream>
#include <cstring>
#include <string>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

// --- GLOBAL VARIABLES ---
const int KEY_SIZE = 32;
const int IV_SIZE  = 12;
const int TAG_SIZE = 16;
const int MAX_MSG  = 1024;

unsigned char alice_secret[64];
unsigned char bob_secret[64];
unsigned char aes_key[KEY_SIZE];
unsigned char iv[IV_SIZE];
unsigned char tag[TAG_SIZE];
unsigned char ciphertext[MAX_MSG];
unsigned char decrypted_buffer[MAX_MSG];

size_t alice_len = 0;
size_t bob_len = 0;
int cipher_len = 0;
int decrypted_len = 0;

EVP_PKEY* alice_key = NULL;
EVP_PKEY* bob_key   = NULL;

// Helper untuk error
void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

// -----------------------------------------------------------
// GENERATE KEY PAIR (Public & Private)
// Menggunakan kurva NIST P-256 (prime256v1)
// -----------------------------------------------------------
EVP_PKEY* create_ecdh_key() {
    EVP_PKEY* key = NULL;
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!ctx) handleErrors();

    if (EVP_PKEY_keygen_init(ctx) <= 0) handleErrors();
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0) handleErrors();
    if (EVP_PKEY_keygen(ctx, &key) <= 0) handleErrors();

    EVP_PKEY_CTX_free(ctx);
    return key;
}

// -----------------------------------------------------------
// COMPUTE SHARED SECRET (ECDH)
// Menggabungkan Private Key sendiri dengan Public Key lawan
// Penting: Harus cek ukuran buffer dulu agar tidak error/crash
// -----------------------------------------------------------
void compute_secret(EVP_PKEY* my_key, EVP_PKEY* peer_pub_key, unsigned char* buffer, size_t* len) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(my_key, NULL);
    if (!ctx) handleErrors();

    if (EVP_PKEY_derive_init(ctx) <= 0) handleErrors();
    if (EVP_PKEY_derive_set_peer(ctx, peer_pub_key) <= 0) handleErrors();

    // Cek ukuran secret yang dibutuhkan
    size_t required_len;
    if (EVP_PKEY_derive(ctx, NULL, &required_len) <= 0) handleErrors();
    *len = required_len;

    // Hitung secret
    if (EVP_PKEY_derive(ctx, buffer, len) <= 0) handleErrors();

    EVP_PKEY_CTX_free(ctx);
}

// -----------------------------------------------------------
// [BAGIAN KRUSIAL 3] KEY DERIVATION (SHA-256)
// Mengubah raw secret ECDH menjadi kunci AES yang aman
// -----------------------------------------------------------
void hash_secret(unsigned char* secret, size_t len, unsigned char* output_key) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    unsigned int temp_len;

    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, secret, len);
    EVP_DigestFinal_ex(ctx, output_key, &temp_len);

    EVP_MD_CTX_free(ctx);
}

// -----------------------------------------------------------
// ENKRIPSI AES-GCM
// Menghasilkan Ciphertext + Authentication Tag
// -----------------------------------------------------------
int encrypt_aes(unsigned char* plain, int len, unsigned char* key, unsigned char* iv_out, unsigned char* cipher_out, unsigned char* tag_out) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int outlen, tmplen;

    RAND_bytes(iv_out, IV_SIZE); // Generate IV Acak

    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL);
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv_out);

    EVP_EncryptUpdate(ctx, cipher_out, &outlen, plain, len);
    tmplen = outlen;

    EVP_EncryptFinal_ex(ctx, cipher_out + outlen, &outlen);
    tmplen += outlen;

    // Ambil Tag Integritas
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag_out);

    EVP_CIPHER_CTX_free(ctx);
    return tmplen;
}

// -----------------------------------------------------------
// [BAGIAN KRUSIAL 5] DEKRIPSI & VERIFIKASI TAG
// Jika Tag salah, dekripsi dianggap gagal (anti-tamper)
// -----------------------------------------------------------
int decrypt_aes(unsigned char* cipher, int len, unsigned char* key, unsigned char* iv_in, unsigned char* tag_in, unsigned char* plain_out) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int outlen, tmplen, ret;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL);
    EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv_in);

    EVP_DecryptUpdate(ctx, plain_out, &outlen, cipher, len);
    tmplen = outlen;

    // Set Tag untuk verifikasi
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag_in);

    // Final check: ret > 0 artinya sukses, ret <= 0 artinya gagal (tag mismatch)
    ret = EVP_DecryptFinal_ex(ctx, plain_out + outlen, &outlen);
    tmplen += outlen;

    EVP_CIPHER_CTX_free(ctx);

    if(ret > 0) return tmplen;
    else return -1;
}

// --- MAIN PROGRAM ---
int main() {
    cout << "=== ECDH & AES-GCM DEMO ===" << endl;

    // 1. Generate Keys
    alice_key = create_ecdh_key();
    bob_key   = create_ecdh_key();
    cout << "[INFO] Key Pair Generated." << endl;

    // 2. Compute Secret
    compute_secret(alice_key, bob_key, alice_secret, &alice_len);
    compute_secret(bob_key, alice_key, bob_secret, &bob_len);
    cout << "[INFO] ECDH Secret Computed." << endl;

    // 3. Hash to AES Key
    hash_secret(alice_secret, alice_len, aes_key);

    // 4. Encrypt
    string pesan = "Ini adalah pesan rahasia yang aman.";
    cout << "\n[Alice] Mengirim: " << pesan << endl;

    cipher_len = encrypt_aes((unsigned char*)pesan.c_str(), pesan.length(), aes_key, iv, ciphertext, tag);
    cout << "[Network] Ciphertext dikirim (" << cipher_len << " bytes)." << endl;

    // 5. Decrypt
    decrypted_len = decrypt_aes(ciphertext, cipher_len, aes_key, iv, tag, decrypted_buffer);

    if (decrypted_len > 0) {
        decrypted_buffer[decrypted_len] = '\0';
        cout << "[Bob]   Terima: " << decrypted_buffer << endl;
    } else {
        cout << "[Bob]   GAGAL! Autentikasi salah." << endl;
    }

    EVP_PKEY_free(alice_key);
    EVP_PKEY_free(bob_key);
    return 0;
}