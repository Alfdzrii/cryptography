#include <iostream>
#include <string>
#include <bitset>   
#include <iomanip>  

using namespace std;

// CIRCULAR SHIFT 
unsigned short rotateRight(unsigned short value, int shift) {
    return (value >> shift) | (value << (16 - shift));
}

// encrypt
void encryptBlock(unsigned short &L, unsigned short &R, unsigned char K) {
    unsigned short step1 = L ^ K; 
    unsigned short noise = rotateRight(step1, 4);
    R = R ^ noise; 
}

int main() {
    int keyInput;
    string textInput;

    cout << "===========================================" << endl;
    cout << "      CYBER 1: ENCRYPTION TOOL ONLY        " << endl;
    cout << "===========================================" << endl;
    

    cout << "input Key (0-255): ";
    cin >> keyInput;
    unsigned char Key = (unsigned char)keyInput;

    // encyrpt logic
    cout << "input Teks (Maks 4 huruf): ";
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, textInput);

    // Padding jika kurang dari 4 karakter
    while (textInput.length() < 4) textInput += " ";

    unsigned short L = (textInput[0] << 8) | textInput[1];
    unsigned short R = (textInput[2] << 8) | textInput[3];

    cout << "\n[PROCESS] Encryption..." << endl;
    encryptBlock(L, R, Key);


    cout << "-------------------------------------------" << endl;
    cout << "Result :" << endl;
    cout << "1. Decimal Format : "<< endl; 
    cout << "left block : " << L << endl;
    cout << "right block : "<< R << endl<< endl;
    cout << "2. Binary Format  : " << endl;
    cout << "left block : "<< bitset<16>(L) << endl;
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

    return 0;
}