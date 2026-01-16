#include <iostream>
#include <string>
#include <bitset>   
#include <iomanip>  

using namespace std;

// CIRCULAR SHIFT 
unsigned short rotateRight(unsigned short value, int shift) {
    return (value >> shift) | (value << (16 - shift));
}

// ENKRIPSI 
void encryptBlock(unsigned short &L, unsigned short &R, unsigned char K) {
    unsigned short step1 = L ^ K; 
    unsigned short noise = rotateRight(step1, 4);
    R = R ^ noise; 
}

// DEKRIPSI 
void decryptRecursive(unsigned short &L, unsigned short &R, unsigned char K, int round) {
    if (round == 0) return;
    unsigned short step1 = L ^ K;
    unsigned short noise = rotateRight(step1, 4);
    R = R ^ noise;
    decryptRecursive(L, R, K, round - 1);
}

int main() {
    int choice;
    int keyInput;

    cout << "===========================================" << endl;
    cout << "   CYBER 1: ENCRYPTION & DECRYPTION TOOL   " << endl;
    cout << "===========================================" << endl;
    cout << "Pilih Mode:" << endl;
    cout << "1. Encrypt (Text -> Ciphertext)" << endl;
    cout << "2. Decrypt (Ciphertext -> Text)" << endl;
    cout << "Pilihan: ";
    cin >> choice;

    cout << "input Key (0-255): ";
    cin >> keyInput;
    unsigned char Key = (unsigned char)keyInput;

    if (choice == 1) {
        // encrypt 
        string textInput;
        cout << "input Teks (Maks 4 huruf): ";
        if (cin.peek() == '\n') cin.ignore();
        getline(cin, textInput);

        while (textInput.length() < 4) textInput += " ";

        unsigned short L = (textInput[0] << 8) | textInput[1];
        unsigned short R = (textInput[2] << 8) | textInput[3];

        cout << "\n[PROCESS] Encryption..." << endl;
        encryptBlock(L, R, Key);

        // visualisasi
        cout << "-------------------------------------------" << endl;
        cout << "Result :" << endl;
        cout << "1. Decimal Format : "<< endl; 
        cout << "left block : " << L << endl;
        cout << "right block : "<< R << endl<< endl;
        cout << "2. Binary Format  : " << endl;
        cout << "left block : "<<bitset<16>(L)<< endl;
        cout << "right block : "<< bitset<16>(R) << endl;
        cout << "-------------------------------------------" << endl;
        
        cout << "Result (CIPHERTEXT):" << endl;
        cout << "Hex Format : " 
             << hex << uppercase << setfill('0') 
             << setw(4) << L << " " << setw(4) << R 
             << dec << endl; 

        char c1 = (char)(L >> 8);
        char c2 = (char)(L & 0xFF);
        char c3 = (char)(R >> 8);
        char c4 = (char)(R & 0xFF);
        
        cout << "Raw String : [" << c1 << c2 << c3 << c4 << "]" << endl;
        cout << "(Note: If empty/weird, it contains non-printable chars)" << endl;
        cout << "-------------------------------------------" << endl;

    } else if (choice == 2) {
        unsigned short L, R;
        int inputFormat;

        cout << "\nSelect Input Data Format:" << endl;
        cout << "1. Decimal (Example: 27044)" << endl;
        cout << "2. Binary  (Example: 01101001...)" << endl;
        cout << "Choice: ";
        cin >> inputFormat;

        if (inputFormat == 1) {
            // Input Angka Biasa
            cout << "Input Left Number  : "; cin >> L;
            cout << "Input Right Number : "; cin >> R;
        } else {
            // Input String Binary 
            string binL, binR;
            cout << "input Binary Left  : "; cin >> binL;
            cout << "input Binary Right : "; cin >> binR;

            // Teks Binary -> Angka
            try {
                L = (unsigned short)bitset<16>(binL).to_ulong();
                R = (unsigned short)bitset<16>(binR).to_ulong();
            } catch (...) {
                cout << "ERROR: Invalid binary format! Ensure only 0 and 1." << endl;
                return 1;
            }
        }

        cout << "\n[PROCESS] Encryption..." << endl;
        decryptRecursive(L, R, Key, 1);

        char c1 = (char)(L >> 8);
        char c2 = (char)(L & 0xFF);
        char c3 = (char)(R >> 8);
        char c4 = (char)(R & 0xFF);

        cout << "-------------------------------------------" << endl;
        cout << "REAL TEXT: " << c1 << c2 << c3 << c4 << endl;
        cout << "-------------------------------------------" << endl;
    } else {
        cout << "NOT A VALID ANSWER." << endl;
    }

    return 0;
}