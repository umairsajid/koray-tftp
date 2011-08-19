#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;
class Beer{
    private:
        char btype [10];
    public:
        int toplam;
        Beer() { 
            toplam = 2;
        }
        // in to out
        friend ostream& operator<<(ostream& output, const Beer &b) {
            output << b.btype;
            return output;
        }
        // out to in
        friend istream& operator>>(istream& input, Beer &b);
};

istream& operator>>(istream& input, Beer &b){
    char x [512];
    x[511] = '|';
    input.seekg( 0, ios::beg );
    input.read(x, 512);
    short int lastbytes = input.gcount();
    b.toplam = lastbytes; 
   
    memcpy (b.btype, x, lastbytes);
    return input;
    }

int main() {
    Beer a;
 
    /* This cout will use your overloaded << function */
    cout << a.toplam << endl;
    cout << "Basla!" << endl;
    /* This cin will use your overloaded >> function */
    char u [] = "Koray Kalmaz";
    istringstream us (stringstream::in | stringstream::out);
    us.clear();
    us.str(u);
    us >> a;
    
    cout << "Read: " << a << endl;
    cout << "Got: " << a.toplam << endl;
    
    int s = 54321;
    cout << s % 4096 << endl;
}
