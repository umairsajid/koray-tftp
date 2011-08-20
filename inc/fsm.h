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

/*  client ui commands
    mode        transfer-mode [ascii|bin]
    ascii       Shorthand for "mode ascii"
    binary      Shorthand for "mode binary"
    connect     host-name [port]
                connect command remembers what host is to be used.
    get         filename
    get         remotename localname
    get         file1 file2 ... fileN
                Source can be a filename or hosts:filename
                hostname becomes the default for future transfers.
    put         file
    put         localfile remotefile
    put         file1 file2 ... fileN remote-directory
    quit
    rexmt       retransmission-timeout
    status      Show current status.
    timeout     total-transmission-timeout
    verbose     Toggle verbose mode.
    ?           help
*/
/*  FSM States:
        1.  Idle
        20  Download
        29  Download failed
        30  Upload
        39  Upload failed
        90  Quiting
*/
using namespace std;

class fsm {
    public:
        fsm();
        short int getState();
        void checkCommand(char cmdLine [256]);
        int beginTransfer(vector <string> tkn);
        int startCLI();
    protected:
        short int curState;
        short int transferMode(string mode);
    private:
        struct task {
            string cmdName;
            string helpText;
            };
        vector <task> commands;  // commands list
        short int tMode;    // 1: binary, 2: ascii
        int rexmt;          // packet timeout
        int sessionTimeout; // general timeout
        bool verboseMode;   // verbose mode
        void printStatus(); // print put, get, timeouts
        void fillCommands();// fill commands for use
    };

class tftpServer : fsm {
    public:
        tftpServer(int listenPort);
    private:
        int port;
    };

class tftpClient : fsm {
    public:
        tftpClient();               // CLI mode (pthread)
        tftpClient(char * rHost);   // CLI mode (pthread) with hostname
    private:
        string remoteAddress;
    };
