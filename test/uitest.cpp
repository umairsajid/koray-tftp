#include <iostream>
#include <cstring>
#include <arpa/inet.h>

using namespace std;


int main(){
    
    char *kk = new char [4];
    
    unsigned short int * bir = new unsigned short int;
    unsigned short int * cok = new unsigned short int;
    
    uint16_t * ters = new uint16_t;
    
    *bir = 1;
    *cok = 128;
    
    *ters = htons((uint16_t) * cok) ;
    
    cout << "bir: " << * bir << ' ' << * (bir + 1) << endl;
    cout << "cok: " << * cok << ' ' << * (cok + 1) << endl;
    cout << "ters: " << * ters << ' ' << * (ters + 1) << endl;
    
    cout << "before: ";
    for (int i = 0; i < 4; i++) {
        cout << (unsigned int) * (kk + i) << ' ';
        }
    cout << endl;
    
    
    memcpy(kk+2, ters, 2);
    memcpy(kk, cok, 2);
    
    
    cout << "after: ";
    for (int i = 0; i < 4; i++) {
        cout << (unsigned int) * (kk + i) << ' ';
        }
    cout << endl;
    
    return 0;
    }
