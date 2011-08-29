#include <iostream>
#include <cstring>
#include <arpa/inet.h>

using namespace std;


int main(){
    int i;
    char * a = new char [6];
    short int q = 5;
    short int w = 6;
    
    memcpy(a, &q, 2);
    memcpy(a + 2, &w, 2);
    
    cout << "normal: ";
    for (i = 0; i < 4; i++) {
        cout << hex << showbase << (int) * (a + i) << " ";
    }
    cout << endl;
    
    uint16_t qa = htons(q);
    uint16_t wa = htons(w);
    
    memcpy(a, &qa, 2);
    memcpy(a + 2, &wa, 2);
    
    cout << "ntohs: ";
    for (i = 0; i < 4; i++) {
        cout << hex << showbase <<(int) * (a + i) << " ";
    }
    cout << endl;
    
    cout << hex << noshowbase << 96 << endl;
    
    return 0;
    }
