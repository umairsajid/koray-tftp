/*
    TFTP client UI
    
    Y Koray Kalmaz 2011
    
*/

# include <algorithm>
# include <iostream>
# include <istream>
# include <iterator>
# include <cstdlib>
# include <sstream>
# include <string>
# include <strings.h>
# include <vector>

using namespace std;

class fsm {
    public:
        fsm();
        short int getState();
        int beginTransfer(vector <string> tkn);
        void dw(char * hede);
    protected:
        short int curState;         // 0:idle, 1:sending, 2:receiving
        struct task {
            string cmdName;
            string helpText;
            };
        vector <task> commands;     // commands list
        int rexmt;                  // general packet timeout
        int sessionTimeout;         // general timeout
        bool verboseMode;           // verbose mode
    private:
        void printStatus();                     // put, get, timeouts
    };

class tftpServer : fsm {
    public:
        tftpServer(int listenPort);
    private:
        struct transfer {
            string clientAddress;
            short int tMode;                    // 1: binary, 2: ascii
            long int fileSize;                  // total size in bytes
            long int completed;                 // bytes done
            };
        vector <transfer> clients;
        int port;                               // listening port
        void runCommand(vector<string> tokens);       // server interpreter
        void fillCommands();                    // fill server commands
    };

class tftpClient : fsm {
    public:
        tftpClient();
        tftpClient(char * rHost);
        int startCLI();                         // command cursor
        short int transferMode(string mode);    // Not implemented
        void checkCommand(char cmdLine [256]);  // written command
    private:
        string remoteAddress;                   // server address
        short int tMode;                        // 1: binary, 2: ascii
        long int fileSize;                      // total size in bytes
        long int completed;                     // bytes done
        void runCommand(vector<string> tokens); // client interpreter
        void fillCommands();                    // fill client commands
        void printStatus();                     // put, get, timeouts
    };
