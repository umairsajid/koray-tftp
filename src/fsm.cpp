/*
    TFTP client UI
    
    Y Koray Kalmaz 2011
    
*/
/*  Client commands:
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
/*  Server commands:
    mode        transfer-mode [ascii|bin]
    ascii       Shorthand for "mode ascii"
    binary      Shorthand for "mode binary"
    stop        stop transfer for thread x
    quit
    rexmt       retransmission-timeout
    status      Show current status.
    timeout     total-transmission-timeout
    verbose     Toggle verbose mode.
    ?           help
*/

# include "fsm.h"

#define THREADS 50

/* FSM Default Constructor                                          */
fsm::fsm(){
    curState = 1;
    sessionTimeout = 1000;
    rexmt = 1000;
    // Fill default commands:
    task t;
    t.cmdName = "mode";
    t.helpText = "Set transfer mode";
    commands.push_back(t);
    t.cmdName = "ascii";
    t.helpText = "Set mode ascii";
    commands.push_back(t);
    t.cmdName = "binary";
    t.helpText = "Set mode binary";
    commands.push_back(t);
    t.cmdName = "quit";
    t.helpText = "Exit";
    commands.push_back(t);
    t.cmdName = "rexmt";
    t.helpText = "Set per-packet retransmission timeout";
    commands.push_back(t);
    t.cmdName = "status";
    t.helpText = "Print status";
    commands.push_back(t);
    t.cmdName = "timeout";
    t.helpText = "Set general retransmission timeout";
    commands.push_back(t);
    t.cmdName = "verbose";
    t.helpText = "Print everything";
    commands.push_back(t);
    t.cmdName = "?, help";
    t.helpText = "Show help";
    commands.push_back(t);
    }

/* Debug writer */
void fsm::dw(char * hede){
    cout << hede << endl;
    }

/* Return current state                                             
    client FSM States:
        1.  Idle
        20  Download
        29  Download failed
        30  Upload
        39  Upload failed
        90  Quiting
                                                                    */
short int fsm::getState(){
    return curState;
    }

int tftpClient::beginTransfer(vector <string> tkn){
    if (tkn.size() == 1) {
        cout << "MISSING: What should I " << tkn.at(0) << "?" << endl;
        return 0;
        }
    for (int i = 1; i < (int) tkn.size(); i++) {
        // initialize transfer for tkn.at(i):
        cout << tkn.at(0) << "ting " << tkn.at(i) << endl;
        // start client for single transfer
        // TODO: Fix this conversation later..
        char * www = new char [ tkn.at(i).size() + 1 ];
        copy(tkn.at(i).begin(), tkn.at(i).end(), www);
        myNet.startClient(remoteAddress, www, (tkn.at(0) == "put"));
    }
    return 0;
    }

void fsm::printStatus(){
    cout << "General timeout:\t" << sessionTimeout << " ms." << endl;
    cout << "Packet timeout: \t" << rexmt << " ms." << endl;
    cout << "Connections:\n";
    myNet.printList();
    }

void fsm::abort(vector <string> arg){
    if (arg.size() < 2) {
        cout << "Missing argument" << endl;
    } else {
        int ao [THREADS];
        int x;
        myNet.getOps(ao, x);
        for (unsigned int i = 1; i < arg.size(); i++){
            // check if op exists:
            for (unsigned int j = 0; j < (unsigned int) x; j++) {
                cout << atoi(arg.at(i).c_str()) << " = " << ao[j];
                cout << " : " << (atoi(arg.at(i).c_str()) == ao[j]);
                cout << endl;
                if (atoi(arg.at(i).c_str()) == ao[j]) {
                    // abort:
                    myNet.abort(atoi(arg.at(i).c_str()));
                    cout << "Aborted: " << arg.at(i) << endl;
                    }
                }
            }
        }
    }

/* TFTP Client default                                              */
tftpClient::tftpClient() : fsm(){
    curState = 1;
    tMode = 1;
    remoteAddress[0] = '\0';
    fillCommands();
    startCLI();
    }

/* TFTP Client with hostname                                        */
tftpClient::tftpClient(char * rHost) : fsm(){
    curState = 1;
    cout << "Host address given" << endl;
    strncpy(remoteAddress, rHost, 256);
    fillCommands();
    // TODO: check host address
    startCLI();
    }

int tftpClient::startCLI(){
    string cursorText = "tftp> ";
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

/* change mode, print string, return short int */
short int tftpClient::transferMode(string mode){
    // not implemented for assignment CSC 8415
    // binary mode is default
    cout << "Mode: binary" << endl;
    return 1;
    }

void tftpClient::fillCommands(){
    task t;
    t.cmdName = "connect";
    t.helpText = "Connect to remote TFTP";
    commands.push_back(t);
    t.cmdName = "get";
    t.helpText = "Receive file";
    commands.push_back(t);
    t.cmdName = "put";
    t.helpText = "Send file";
    commands.push_back(t);
    }

void tftpClient::runCommand(vector<string> tokens){
    if (tokens.at(0) == "mode"){
        cout << "mode: " << endl;
        transferMode(tokens.at(1));
    } 
    else if (tokens.at(0) == "ascii"){
        transferMode("ascii");
    } 
    else if (tokens.at(0) == "binary"){
        transferMode("binary");
    }
    else if (tokens.at(0) == "connect"){
        char * nl = new char [ tokens.at(1).size() + 1 ];
        copy(tokens.at(1).begin(), tokens.at(1).end(), nl);
        char * nlk = myNet.nameLookup(nl);
        if (0 != (int) nlk) {
            cout << nl << ": " << nlk << endl;
        } else {
            cout << "WARNING: Address not found.\n";
            }
        strncpy(remoteAddress, nl, 256);
    } 
    else if (tokens.at(0) == "get"){
        beginTransfer(tokens);
    } 
    else if (tokens.at(0) == "put"){
        beginTransfer(tokens);
    } else if (tokens.at(0) == "quit"){
        cout << "User quit." << endl;
        curState = 90;
    } 
    else if (tokens.at(0) == "abort"){
        abort(tokens);
    }
    else if (tokens.at(0) == "rexmt"){
        if (200 > (int) tokens.at(1).c_str() ){
            cout << "Value can not be smaller than 200.";
        } else {
            rexmt = atoi(tokens.at(1).c_str());
            }
    }
    else if (tokens.at(0) == "status"){
        cout << "Remote Address: \t" << remoteAddress << endl;
        printStatus();
    } 
    else if (tokens.at(0) == "timeout"){
        if (200 > (int) tokens.at(1).c_str() ){
            cout << "Value can not be smaller than 200.";
        } else {
            sessionTimeout = atoi(tokens.at(1).c_str());
            }
    } 
    else if (tokens.at(0) == "verbose"){
        verboseMode = !verboseMode;
        if (verboseMode) {
            cout << "Verbose mode ON" << endl;
        } else {
            cout << "Verbose mode OFF" << endl;
        }
    } 
    else if ((tokens.at(0) == "?") || (tokens.at(0) == "help")){
        for (unsigned int i = 0; i < commands.size(); i++) {
            task t = commands.at(i);
            cout << "\t" << t.cmdName << "\t\t" << t.helpText << endl;
        }
    } 
    else {
        cout << "Unknown command: " << tokens.at(0) << endl;
        }
    }

/* Checks command                                                   */
void tftpClient::checkCommand(char cmdLine [256]){
    if (cmdLine[0] == 0x00) {
        // cout << "EMPTY" << endl;
        return;
    }
    istringstream iss(cmdLine, istringstream::in);
    vector<string> tokens;
    copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter<vector<string> >(tokens));
    runCommand(tokens);
    }
