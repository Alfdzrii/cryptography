#include <iostream>
using namespace std;

int main(){
    int n = 11;
    int cnt = 0;
    if (n <= 1){
        cout << n << " is not prime"<<endl;
    }
    else {
        for(int i = 2; i * i <= n; i++){
            if (n % i == 0){
                cnt++;
            }
        }
         if (cnt > 0)
            cout << n << " is NOT prime";

        else
            cout << n << " is prime";
    }
}


// int main() {
//     int n = 12;
//     int cnt = 0;
    
//     // If number is less than/equal to 1,
//     // it is not prime
//     if (n <= 1)
//         cout << n << " is NOT prime";
//     else {

//         // Count the divisors of n
//         for (int i = 2; i * i <= n; i++) {
//             if (n % i == 0)
//                 cnt++;
//         }

//         // If n is divisible by more than 2 
//         // numbers then it is not prime
//         if (cnt > 0)
//             cout << n << " is NOT prime";

//         // else it is prime
//         else
//             cout << n << " is prime";
//     }
//     return 0;
// }