#include <iostream>
using namespace std;

string Ceasar(string p, int key){
    int len = p.length();
    string result="";
    for (int i = 0; i<len;i++){
        if (isupper(p[i])){
            result += ((int (p[i])+ key - 65)%26+65);
        }
        else{
            result += ((int (p[i])+ key - 97)%26-97);
        }
    }
    return result;
}

int main() {
    string Cipher, Data;
    Cipher = Ceasar(Data, 3);
    Data = Ceasar(Cipher, 26-3);
    
    
    return 0;
}