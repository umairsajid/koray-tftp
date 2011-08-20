/*
    TFTP client UI
    
    Y Koray Kalmaz 2011
    
*/

# include "fsm.h"

/*  FSM default commands:
    mode        transfer-mode [ascii|bin]
    ascii       Shorthand for "mode ascii"
    binary      Shorthand for "mode binary"
    quit
    rexmt       retransmission-timeout
    status      Show current status.
    timeout     total-transmission-timeout
    verbose     Toggle verbose mode.
    ?           help
*/
/*  Client commands:
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
*/
/*  Server commands:
    stop        stop transfer for thread x
*/

static string cursorText = "tftp> ";

/* FSM Default Constructor                                          */
fsm::fsm(){
    curState = 1;
    tMode = 1;
    fillCommands()
    }

/* Return current state                                             */
short int fsm::getState(){
    return curState;
    }

/* Checks command                                                   */
void fsm::checkCommand(char cmdLine [256]){
    
    if (cmdLine[0] == 0x00) {
        // cout << "EMPTY" << endl;
        return;
    }
    
    istringstream iss(cmdLine, istringstream::in);
    vector<string> tokens;
    copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter<vector<string> >(tokens));

    if (tokens.at(0) == "mode"){
        transferMode(tokens.at(1));
    } 
    else if (tokens.at(0) == "ascii"){
        transferMode("ascii");
    } 
    else if (tokens.at(0) == "binary"){
        transferMode("binary");
    } 
    else if (tokens.at(0) == "connect"){
        remoteAddress = tokens.at(1);
    } 
    else if (tokens.at(0) == "get"){
        beginTransfer(tokens);
    } 
    else if (tokens.at(0) == "put"){
        beginTransfer(tokens);
    } 
    else if (tokens.at(0) == "quit"){
        curState = 90;
    } 
    else if (tokens.at(0) == "rexmt"){
        rexmt = atoi(tokens.at(1).c_str());
    } 
    else if (tokens.at(0) == "status"){
        printStatus();
    } 
    else if (tokens.at(0) == "timeout"){
        sessionTimeout = atoi(tokens.at(1).c_str());
    } 
    else if (tokens.at(0) == "verbose"){
        verboseMode = !verboseMode;
        if (verboseMode) {
            cout << "Verbose mode ON" << endl;
        } else {
            cout << "Verbose mode OFF" << endl;
        }
    } 
    else if (tokens.at(0) == "?"){
        for (unsigned int i = 0; i < commandsCli.size(); i++) {
            task t = commandsCli.at(i);
            cout << "\t" << t.cmdName << "\t\t" << t.helpText << endl;
        }
    } 
    else {
        cout << "Unknown command: " << tokens.at(0) << endl;
        }
    }
/* change mode, print string, return short int */
short int fsm::transferMode(string mode){
    // not implemented for assignment CSC 8415
    // binary mode is default
    cout << "Mode: binary" << endl;
    return 1;
    }

int fsm::beginTransfer(vector <string> tkn){
    return 0;
    }

int fsm::startCLI(){
    // given command:
    char cmd [256];
    // Go to command line
    while (getState() != 90) {
        cout << cursorText;
        cin.getline(cmd, 256);
        if (cin.eof()) {
            cout << "\nCtrl+D Pressed" << endl;
            break;
            }
        checkCommand(cmd);
        }
    return 0;
    }

void fsm::printStatus(){}

void fsm::fillCommands(){
    task t;
    
    t.cmdName = "mode";
    t.helpText = "Set transfer mode";
    commandsCli.push_back(t);
    t.cmdName = "ascii";
    t.helpText = "Set mode ascii";
    commandsCli.push_back(t);
    t.cmdName = "binary";
    t.helpText = "Set mode binary";
    commandsCli.push_back(t);
    t.cmdName = "quit";
    t.helpText = "Exit";
    commandsCli.push_back(t);
    t.cmdName = "rexmt";
    t.helpText = "Set per-packet retransmission timeout";
    commandsCli.push_back(t);
    t.cmdName = "status";
    t.helpText = "Print status";
    commandsCli.push_back(t);
    t.cmdName = "timeout";
    t.helpText = "Set general retransmission timeout";
    commandsCli.push_back(t);
    t.cmdName = "verbose";
    t.helpText = "Print everything";
    commandsCli.push_back(t);
    t.cmdName = "?";
    t.helpText = "Show help";
    commandsCli.push_back(t);
    }

/* TFTP Client default                                              */
tftpClient::tftpClient() : fsm(){
    }

/* TFTP Client with hostname                                        */
tftpClient::tftpClient(char * rHost) : fsm(){
    }

void tftpClient::fillCommands(){
    t.cmdName = "connect";
    t.helpText = "Connect to remote TFTP";
    commandsCli.push_back(t);
    t.cmdName = "get";
    t.helpText = "Receive file";
    commandsCli.push_back(t);
    t.cmdName = "put";
    t.helpText = "Send file";
    commandsCli.push_back(t);
    }
