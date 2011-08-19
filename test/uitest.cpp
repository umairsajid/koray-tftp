#include <iostream>

using namespace std;

static string cursorText = "tftp> ";

class fsm{
    public:
        fsm(){
            curState = 1;
            }
        short int state(){
            return curState;
            }
        friend istream& operator>>(istream& input, fsm &f){
            input >> f.lastCommand;
            f.checkState();
            return input;
            }
    protected:
        short int curState;
        string lastCommand;
        void checkState(){
            cout << lastCommand << endl;
            }
    };

int main(){
    
    fsm f;
    
    cout << cursorText;
    while (!(cin >> f) || !(f.state() == 5)) {
        cin.clear();
        cin.ignore(1024, '\n');
        cout << cursorText;
        }
    return 0;
    }
