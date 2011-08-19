# include <fstream>
# include <iostream>

using namespace std;

void yaz(char b[130], int nsize){
    for (int i = 0; i<nsize; i++){
        if (b[i]=='\n'){
            cout << ' ';
        } else {
            cout << b[i];
        }
    }
    cout << '|' << endl;
    };

void read(){
    char buffer[130];
    int lastbytes;
    ifstream is;
    is.open ("rfc783.txt", ios::binary );
    for (int i = 0; i<10; i++){
        is.seekg( 0, ios::beg );
        
        is.seekg( i * 130, ios::beg );
        is.read(buffer, 130);
        
        lastbytes = is.gcount();
        
        if (is.eof()){
            cout << "\n\n----------EOF!----------\n\n";
            std::fill(buffer+ lastbytes, buffer + 130, 0);
        }
        
        
        cout << i * 130 << ": " << is.gcount() << " ::: \t|";
        yaz(buffer, 130);
        }
    is.close();
    }

void write(){
    char buffer[20];
    for (int i = 0; i < 19; i++){
        buffer[i] = i + 65;
    }
    buffer[19] = '\n';
    ofstream os;
    os.open("deney.txt", ios::binary);
    os.seekp (0, ios::end);
    for (int i = 0; i < 30; i++) {
        os.write(buffer, 20);
    }
    for (int i = 1; i < 20; i++)
    {
        buffer[i] = 0;
    }
    buffer[0] = EOF;
    
    os.write(buffer, 20);
    
    os.close();
}

int main(){
    read();
//~     write();
    }
