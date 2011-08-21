# include "threads.h"
# include <cstdlib>
using namespace std;

int main(){
    anne t;

    long hede [50];
    int rakam = 50;
    
    for (int i = 0; i < rakam; i++) {

        hede[i] = t.startNew("sommetext", "whitebeer");
    }
    
    for (int i = 0; i < 5; i++) {
        sleep(1);
        t.printThreads();
        cout << "-----------------" << endl;
    }
    
    
    cout << "deleting in 2 seconds..!" << endl;
    sleep(2);
    
    for (int i = 0; i < rakam - 1; i++) {
        cout << "Deleting " << hede[i] << "... ";
        t.abort(hede[i]);
        cout << "Deleted: " << hede[i] << endl;
        cout << "-----------------" << endl;
        t.printThreads();
        sleep(1);
    }
    
    
    for (int i = 0; i < 6; i++) {
        sleep(1);
        t.printThreads();
        cout << 5-i << endl;
    }
    
    cout << "Deleting " << hede[rakam -1] << "... ";
    t.abort(hede[rakam -1]);
    cout << "-----\n";
    t.printThreads();
    
    cout << "test done!" << endl;
    return 0;
    }
