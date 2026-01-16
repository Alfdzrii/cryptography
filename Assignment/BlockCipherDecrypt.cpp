#include <iostream>
#include <string>
#include <bitset>
#include <iomanip>

using namespace std;

// CIRCULAR SHIFT
unsigned short rotateRight(unsigned short value, int shift) {
    return (value >> shift) | (value << (16 - shift));
}

// Decrypt
void decryptRecursive(unsigned short &L, unsigned short &R, unsigned char K, int round) {
    if (round == 0) return;
    unsigned short step1 = L ^ K;
    unsigned short noise = rotateRight(step1, 4);
    R = R ^ noise;
    decryptRecursive(L, R, K, round - 1);
}

int main() {
    int keyInput;
    unsigned short L, R;
    int inputFormat;

    cout << "===========================================" << endl;
    cout << "      CYBER 1: DECRYPTION TOOL ONLY        " << endl;
    cout << "===========================================" << endl;


    cout << "input Key (0-255): ";
    cin >> keyInput;
    unsigned char Key = (unsigned char)keyInput;


    cout << "\nSelect Input Data Format:" << endl;
    cout << "1. Decimal (Example: 27044)" << endl;
    cout << "2. Binary  (Example: 01101001...)" << endl;
    cout << "Choice: ";
    cin >> inputFormat;

    if (inputFormat == 1) {

        cout << "Input Left Number  : "; cin >> L;
        cout << "Input Right Number : "; cin >> R;
    } else {

        string binL, binR;
        cout << "input Binary Left  : "; cin >> binL;
        cout << "input Binary Right : "; cin >> binR;

        // text Binary -> Angka
        try {
            L = (unsigned short)bitset<16>(binL).to_ulong();
            R = (unsigned short)bitset<16>(binR).to_ulong();
        } catch (...) {
            cout << "ERROR: Invalid binary format! Ensure only 0 and 1." << endl;
            return 1;
        }
    }

    cout << "\n[PROCESS] Decryption..." << endl;
    decryptRecursive(L, R, Key, 1);

    char c1 = (char)(L >> 8);
    char c2 = (char)(L & 0xFF);
    char c3 = (char)(R >> 8);
    char c4 = (char)(R & 0xFF);

    cout << "-------------------------------------------" << endl;
    cout << "REAL TEXT: " << c1 << c2 << c3 << c4 << endl;
    cout << "-------------------------------------------" << endl;

    return 0;
}