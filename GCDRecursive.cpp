#include <iostream>
using namespace std;

int gcd (int a, int b){
    if (b == 0)
        return a;
    return gcd(b, a%b);
}

int main(){
    // variable
    int a,b ;

    cout << "enter the number for see the gcdc of a and b"<< endl;

    cout << "enter a = ";
    cin >> a;
    cout << endl;
    
    cout << "enter b = ";
    cin >> b;
    cout << endl;
    
    cout << "GCD of " << a << " and " << b << " = " << gcd(a , b) << endl;

    return 0;
}