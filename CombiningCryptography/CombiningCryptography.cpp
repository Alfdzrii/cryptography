//
// Created by ACER on 1/19/2026.
//

#include <iostream>
#include <string>
#include <cmath>

using namespace std;

// Fungsi untuk menghitung (base^exp) % mod
// Kita butuh ini karena pangkat angka bisa menjadi sangat besar
long long modExp(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) // Jika eksponen ganjil
            result = (result * base) % mod;
        exp = exp >> 1; // Bagi eksponen dengan 2
        base = (base * base) % mod;
    }
    return result;
}

// Fungsi untuk mencari Faktor Persekutuan Terbesar (GCD)
long long gcd(long long a, long long b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Fungsi sederhana untuk mencari Private Key (d)
// Rumus: (d * e) % phi == 1
long long findModInverse(long long e, long long phi) {
    for (long long d = 1; d < phi; d++) {
        if ((e * d) % phi == 1)
            return d;
    }
    return -1;
}

int main() {
    // --- STEP 1: KEY GENERATION (Sesuai Slide) ---
    cout << "=== 1. GENERATE KEYS (RSA) ===" << endl;

    // 1. Generate two small primes (dari request user)
    long long p = 17;
    long long q = 13;

    // 2. Compute n dan phi
    long long n = p * q;
    long long phi = (p - 1) * (q - 1);

    // Pilih Public Key (e)
    // e harus relatif prima terhadap phi (gcd(e, phi) == 1)
    long long e = 5;
    while (gcd(e, phi) != 1) {
        e++;
    }

    // 3. Compute Private Key (d)
    long long d = findModInverse(e, phi);

    cout << "Prime p: " << p << ", Prime q: " << q << endl;
    cout << "Modulus (n): " << n << endl;
    cout << "Totient (phi): " << phi << endl;
    cout << "Public Key (e, n): (" << e << ", " << n << ")" << endl;
    cout << "Private Key (d, n): (" << d << ", " << n << ")" << endl;
    cout << "---------------------------------------" << endl << endl;


    // --- STEP 2: SMALL RSA ENCRYPTION ---
    cout << "=== 2. SMALL RSA ENCRYPTION ===" << endl;

    // Pilih angka untuk dienkripsi (pesan harus lebih kecil dari n/221)
    long long messageNum = 88;

    // Enkripsi: C = (M ^ e) % n
    long long encryptedNum = modExp(messageNum, e, n);

    // Dekripsi: M = (C ^ d) % n
    long long decryptedNum = modExp(encryptedNum, d, n);

    cout << "Pesan Asli (Angka): " << messageNum << endl;
    cout << "Enkripsi (Ciphertext): " << encryptedNum << endl;
    cout << "Dekripsi (Kembali ke asli): " << decryptedNum << endl;
    cout << "---------------------------------------" << endl << endl;


    // --- STEP 3: DIGITAL SIGNATURE ---
    cout << "=== 3. DIGITAL SIGNATURE ===" << endl;

    // 1. Choose small message
    string text = "HELLO";

    // 2. Compute Hash (Simple Sum of ASCII values sesuai slide)
    long long hashVal = 0;
    cout << "Pesan: " << text << endl;
    cout << "Nilai ASCII: ";
    for (char c : text) {
        cout << (int)c << " ";
        hashVal += (int)c;
    }
    cout << endl;
    cout << "Hash (Sum ASCII): " << hashVal << endl;

    // Hash harus lebih kecil dari n agar matematika RSA bekerja sempurna untuk demo ini
    // Jika hash > n (422 > 221), kita modulkan dengan n agar muat
    long long finalHash = hashVal % n;
    cout << "Hash dimodulo n (agar muat): " << finalHash << endl;

    // 3. Sign with PRIVATE KEY (Signature = Hash ^ d % n)
    long long signature = modExp(finalHash, d, n);
    cout << "Digital Signature (Signed with Private Key): " << signature << endl;

    // 4. Verify with PUBLIC KEY (Verified = Signature ^ e % n)
    long long verifiedHash = modExp(signature, e, n);
    cout << "Verifikasi Signature (Decrypt with Public Key): " << verifiedHash << endl;

    // Cek validitas
    if (verifiedHash == finalHash) {
        cout << "STATUS: SIGNATURE VALID! (Pesan Asli)" << endl;
    } else {
        cout << "STATUS: SIGNATURE TIDAK VALID!" << endl;
    }

    return 0;
}