#include <iostream>
#include <fstream>
#include <sstream>
#include "fileops.h"
using namespace std;

void writeTest(){
    // write test:
    cout << "Creating Object" << endl;
    
    fileOps fo("testingFO.txt", true, 4);
    
    cout << "Creating chunk" << endl;
    
    char c [512];
    for (int i = 0; i < 512; i++){
        c[i] = 'A';
    }
    c[0] = '-';
    c[511] = '|';
    
    cout << "Creating test stream" << endl;
    
    istringstream us (stringstream::in | stringstream::out);
    
    
    for (int i = 0; i < 9; i++){
        cout << "Write:" << i << endl;
        us.clear();
        us.str(c);
        cout << "Writing: " << c[1] << endl;
        us >> fo;
        cout << "Write ok." << endl;
        c[1] = 49 + i;
    }
    cout << "Final write:" << endl;
    us.clear();
    us.str("Koray Kalmaz");
    us >> fo;
    
    cout << "All done!" << endl;
    
    }

void readTest(){
    // read test:
    cout << "Creating Object" << endl;
    fileOps fi("testingFO.txt", false, 4);
    cout << "Error status: " << fi.fserror << endl;
    
//~     ostringstream us (stringstream::in | stringstream::out);
    
    ofstream us ("rtest.txt");
    us.clear();
    
    while (fi.fserror != 100){
        cout << "ECHO: " << us.tellp() << endl;
        us.flush();
//~         us.str("");
//~         us << fi.nextSegment() << endl;
        us << fi;
//~         cout << us.str();
//~         cout << fi.nextSegment() << endl;
        }
    us.close();
    }

int main(){
//~     writeTest();
    readTest();
}
