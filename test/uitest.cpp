#include <iostream>
#include <cstring>
#include <arpa/inet.h>

using namespace std;


int main(){
    
    char * q = new char [2];
    
    *q = 57;
    *(q+1) = 5;
    
    // type cast:   gets first byte only
    u_int16_t z = (u_int16_t) *q;
    
    // returns 57
    cout << z << endl;
    
    // copy memory block:   gets correct value: 1337
    memcpy(&z, q, 2);
    cout << z << endl;
    
    return 0;
    }
