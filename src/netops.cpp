/*  TFTP Network operations handler
    
    Y Koray Kalmaz 2011
    
    compile with -lpthread
                                                                    */
/*  Protocol:
1.  Request (WRQ or RRQ)
2.  acknowledgment packet for WRQ, or the first data packet for RRQ.
3.  Data block number is 0 for WRQ acknowledgement.
4.  Reject all other TIDs' data packets.
5.  Finish transfer with data packet under 512 byte.
                                                                    */
/*  Packet types:
    
        RRQ/WRQ:    Max size:   524
        2 bytes  | string   |1 byte|string 8 byte |1 byte
        ------------------------------------
        | Opcode | Filename | 0 | Mode | 0 |
        ------------------------------------
        
        Opcode:     1: RRQ, 2: WRQ
        Filename:   Chars + '\0'
        Mode:       Case insensitive string (netascii, binary, mail)
        
        
        DATA:       Max size:   516
        2 bytes 2 bytes n bytes
        ---------------------------
        | Opcode | Block # | Data |
        ---------------------------
        
        Opcode:     3
        Block #:    Block number
        Data:       Up to 512 byte
        
        
        ACK:        Max size:   4
        2 bytes 2 bytes
        --------------------
        | Opcode | Block # |
        --------------------
        
        Opcode:     4
        
        
        ERROR:      Max size:   86
        2 bytes 2 bytes string 1 byte
        -----------------------------------
        | Opcode | ErrorCode | ErrMsg | 0 |
        -----------------------------------
        
        Opcode:     5
    
    
    Error Codes:
    
    0   Not defined, see error message (if any).
    1   File not found.
    2   Access violation.
    3   Disk full or allocation exceeded.
    4   Illegal TFTP operation.
    5   Unknown transfer ID.
    6   File already exists.
    7   No such user.
*/

# include "netops.h"

using namespace std;

operation ops [THREADS];            // data for threads
int lst[THREADS];                   // for getOps()
serverSettings srvset;              // server settings
short int counter = 0;              // thread ID index
pthread_mutex_t mutexStats;         // mutex to protect thread data
pthread_mutex_t mutexSrvset;        // mutex to protect srvset data
pthread_attr_t attr;                // thread attribute

static void * tx(void * o){
    int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s!=0) {
        cout << "FATAL; pthread_setcancelstate" << endl;
        exit(-1);
        }
    operation * op = (operation *) o;
    cout << "TX loading for: ";
    cout << inet_ntoa(op->rsock.sin_addr) << ':';
    cout << ntohs(op->rsock.sin_port) << endl;
    // Create socket here
    // set op->state to initialized
    bool server = ( (op->state ^ STATE_SERVER) < op->state );       // server flag
    if (server == false) {
        // until general timeout:
        // Send WRQ first when client
        // Wait for ACK with #0 or resend WRQ after rexmt timeout
        }
    // set op->state to connection
    // initialize read-ahead object
    // Prepare Opcode & Packet No
    // read-ahead:
    // Send data & keep last 512k chunk
    // if after 1 packet: set op->state to Transfer
    // Listen for ACK until rexmt timeout
    // if EOF: listen for last ACK & set op->state to Complete & exit
    // read-ahead next data chunk to buffer
    pthread_exit((void*) o);
    }

void killSocket( short int * st ) {
    short int s = *st;
    s = s | STATE_ERROR;
    }

// returns true if receive timeouts (BUGGY!!!)
bool recvTimeout(int sock, sockaddr_in * sockfrom, void *data, 
                                        size_t *length, int timeout) {
    fd_set socks;                   // Socket file descriptor
    timeval t;                      // for timeout
    
    FD_ZERO(&socks);                // reset fd_set socks
    FD_SET(sock, &socks);           // add sock to socks
    
    socklen_t slen=sizeof(*sockfrom);   // socket memory size
    if (timeout >= 1000) {
        t.tv_sec = timeout / 1000;                  // whole seconds
        t.tv_usec = timeout - (t.tv_sec * 1000);    // miliseconds
    } else {
        t.tv_sec = 0;                   // whole seconds
        t.tv_usec = timeout;            // miliseconds
        }

    cout << "   calling select()\n";

    int sn = select(sock + 1, &socks, NULL, NULL, &t);
    if (sn < 0) {
        cout << "ERROR; select() failed: " << sn << endl;
        exit(-1);
    } else if ( sn == 0 ){
        cout << "   timeout in select() : " << FD_ISSET(sock, &socks) << endl;
        return true;
    } else if ( FD_ISSET(sock, &socks) ) {
        cout << "   No timeout in select() \n";
        int recver = recvfrom(sock, data, *length, 0, (sockaddr*) sockfrom, &slen);
        
        cout << "   Packet received: " << recver << endl;
        return false;
    } else {
        cout << "   Sommethings wrong!\n";
        }
    return true;
    }

// reads from buf, writes to packetNo and data
// DOESN'T WORK FOR 128
void readDataPacket(char * buf, int * packetNo, char * data, 
                                                short int * recvSize){
    // this only gets one byte:
    *packetNo = (unsigned short int) *(buf + 3);
    
    // reverse byte order to new char array:
    char * ts = new char [4];
    for (int i = 0; i < 4; i++){
        * (ts + i) = * (buf + ( 3 - i));
        }
    
    // static cast to void, then ushort (cannot cast from char: weird?)
    //unsigned short * a = static_cast<unsigned short*>(static_cast<void*>(ts + 2));
    unsigned short * b = static_cast<unsigned short*>(static_cast<void*>(ts));
    
    // this doesn't work either (single byte problem)
    struct p {
        u_int16_t oc;
        u_int16_t pn;
        char dt [512];
        };
    
    p * np = new p;
    np = (p *) buf;
    
    np->pn = ((np->pn >> 8) | (np->pn << 8));
    // print everything for test:
    //cout << *packetNo << " oc: " << np->oc << " pn: " << np->pn <<  " kk:" << *a << " : " << *b << endl;
    // nope.. it doesn't work
    *packetNo = *b;
    // this is rubbish too:
//~     uint16_t ttmp = *(buf + 3);
//~     *packetNo = (short int) ttmp;
    
    // finally, copy all data:
    memcpy(data, buf + 4, *recvSize);
    }

// TODO: Use memcpy instead
void makeACK(char * buf, int * pNo){
    *(buf) = 0x00;
    *(buf + 1) = 0x04;
    uint16_t tmpno = htons(* pNo);
    memcpy(buf + 2, &tmpno, 2);
    }

static void * rx(void * o){
    int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    operation * op = (operation *) o;
    if (s!=0) {
        cout << op->id << ": FATAL; pthread_setcancelstate" << endl;
        exit(-1);
        }
    cout << "RX loading for: ";
    cout << inet_ntoa(op->rsock.sin_addr) << ':';
    cout << ntohs(op->rsock.sin_port) << endl;

    // Create socket here
    socklen_t slen=sizeof(op->rsock);    // other socket memory size
    /* kernel will figure out what protocol to use if 0 is given 
    instead of IPPROTO_UDP */
    op->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (op->s == -1){
        cout << op->id << ": FATAL; Can not create socket!\n";
        exit(-1);
        }
    // bind:
    op->lsock.sin_family = AF_INET;                 // internet
    op->lsock.sin_addr.s_addr = htonl(INADDR_ANY);  // listen from any IP
    if (bind(op->s, (struct sockaddr *) &op->lsock, 
                                            sizeof(op->lsock))==-1) {
        cout << "FATAL; bind() failed!\n";
        exit(-1);
        }
    getsockname(op->s,(struct sockaddr *)&op->lsock,&slen);
    // set op->state to initialized
    op->state = op->state | STATE_INITIALIZED;
    char * sndbuf;
    int sndlen = 0;
    if ( (op->state ^ STATE_SERVER) >= op->state) {
        // if client: packet is RRQ (00 01 Filename 00 Mode 00)
        sndlen = 8 + strlen(op->rfile);
        sndbuf = new char [sndlen];
        sndbuf[1] = 0x01;
        strncpy(sndbuf + 2, op->rfile, strlen(op->rfile));
    } else {
        // if server: packet is ACK with #0 (00 04 00 00)
        sndbuf = new char [4];
        sndbuf[1] = 0x04;
        sndlen = 4;
        }
    // set op->state to Connection
    op->state = op->state | STATE_CONNECTED;
    // initialize write-behind object
    op->packetNo = 0;
    int currentPacketno = 0;
    while (op->abort == false) {
        if (op->progress < 512) {
            op->state = op->state | STATE_TRANSFER;
            }
        short int recvBytes = 0;
        // send ACK or RRQ
        if (sendto(op->s, sndbuf, sndlen, 0, 
                        (struct sockaddr *) &op->rsock, slen)==-1){
            cout << "ERROR; sendto() failed!\n";
            exit(-1);
            }
        // listen
        memset(&op->buf, 0, DATA_SIZE_MAX);     // flush buffer
        if ( (op->state ^ STATE_FINISHED) >= op->state) {
            recvBytes = recvfrom(op->s, op->buf, DATA_SIZE_MAX, 0, 
                            (struct sockaddr *) &op->rsock, &slen);
            }
        // Check rhost, rport
        char * kxtmp = inet_ntoa(srvset.rsock.sin_addr);
        if (strcmp((char *) op->rhost, kxtmp) != 0) {
            cout << "WARNING; Got data from wrong host. Ignoring...\n";
            cout << "...Expected to get from: " << op->rhost << endl;
            cout << "...Instead i got from  : ";
            cout << inet_ntoa(srvset.rsock.sin_addr) << endl;
            continue; // try listening again
            }
        // This was last ACK?
        if ( (op->state ^ STATE_FINISHED) < op->state) {
            break;
            }
        // check #packet
        memset(&op->lastSent, 0, 512);     // flush buffer
        readDataPacket((char *) op->buf, &currentPacketno, 
                                (char *) op->lastSent, &recvBytes - 4);
        if (currentPacketno != op->packetNo + 1) {
            continue;
            }
        // if after 1 packet: set op->state to Transfer
        if (op->packetNo == 0) {
            op->state = op->state | STATE_TRANSFER;
            }
        if (op->packetNo == 65534) {
            cout << "RFC1350 size limit\n";
            op->state = op->state | STATE_ERROR;
            break;
            }
        op->packetNo++;
        op->progress = op->progress + (recvBytes - 4);
        // ACK
        sndbuf = new char [4];
        makeACK(sndbuf, &op->packetNo);
        // write-behind HERE:
        
        // check EOF & listen or set op->state to Complete & exit
        if (recvBytes < 516) {
            op->state = op->state | STATE_FINISHED;
            }
        }
    cout << "Exiting: " << op->id << endl;
    pthread_exit((void*) o);
    }

// return 0 for WRQ or STATE_RECEIVE for RRQ
int processRequest(char * buf, char * fn, bool * ascii){
    /* RRQ/WRQ:    Max size:   524
    2 bytes  | char[255] |1 byte|string 8 byte |1 byte
    ------------------------------------
    | Opcode | Filename | 0 | Mode | 0 |
    ------------------------------------
    
    Opcode:     1: RRQ, 2: WRQ
    Filename:   Chars + '\0'
    Mode:       Case insensitive string (netascii, binary, mail)
    */
    if (buf[1] > 2) {
        return buf[1];
        }
    bool wrq = (1 == (int16_t) buf[1]);
    ascii = new bool;
    
    // get filename
    strncpy (fn, buf + 2, REQUEST_SIZE_MAX - 10);
    char * i;
    i = (char *) memchr(buf+2, 0, REQUEST_SIZE_MAX - 10);
    
    // get mode
    char * mstr = new char [9];
    strncpy ( mstr, i+1, 9);
    if (strlen(mstr) > 0) {
        *ascii = (strcmp(mstr, (char *) "netascii") == 0);
        *ascii = (*ascii | (strcmp(mstr, (char *) "NETASCII") == 0));
        *ascii = (*ascii | (strcmp(mstr, (char *) "NetASCII") == 0));
        *ascii = (*ascii | (strcmp(mstr, (char *) "Netascii") == 0));
        *ascii = (*ascii | (strcmp(mstr, (char *) "NetAscii") == 0));
        }
    // return 0 if wrq
    return (int) (wrq == false) * STATE_RECEIVE;
    }

/*  Socket Listener thread:
        1. get general settings:
        2. open port for listening:
        3. create transfer thread for each request
*/
void * server(void * ss){
    cout << "Loading server thread\n";
    int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s!=0) {
        cout << "FATAL; pthread_setcancelstate" << endl;
        exit(-1);
        }
    // clean ops[]:
    int  i = 0; //for loops
    pthread_mutex_lock(&mutexStats);
    for (i = 0; i < THREADS; i++){
        ops[i].id = -1;
    }
    pthread_mutex_unlock(&mutexStats);
    // create server socket:
    socklen_t slen=sizeof(srvset.rsock);    // socket memory size
    char buf[srvset.rbl];                   // initialize request buffer
    /* kernel will figure out what protocol to use if 0 is given 
    instead of IPPROTO_UDP */
    srvset.s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (srvset.s == -1){
        cout << "FATAL; Can not create socket!\n";
        exit(-1);
        }
    // Socket Interface: ME
    srvset.lsock.sin_family = AF_INET;                 // internet
    srvset.lsock.sin_port = htons(srvset.port);        // port
    srvset.lsock.sin_addr.s_addr = htonl(INADDR_ANY);  // listen from any IP
    
    // bind port:
    if (bind(srvset.s, (struct sockaddr *) &srvset.lsock, 
                                            sizeof(srvset.lsock))==-1) {
        cout << "FATAL; bind() failed for port " << srvset.port;
        cout << "!\nAre you root?\n";
        exit(-1);
        }
    int slot = -1;
    // while abort==false:
    while ( srvset.abort == false ){
        // reset buffer:
        memset (buf, '\0', srvset.rbl);
        
        // check srvset for abort flag & set running
        if (srvset.abort != true) {
            pthread_mutex_lock(&mutexSrvset);
            srvset.running = true;
            pthread_mutex_unlock(&mutexSrvset);
            }
        // start listening
        cout << "Ready for new client\n";
        if (recvfrom(srvset.s, buf, srvset.rbl, 0, 
                        (struct sockaddr *) &srvset.rsock, &slen)==-1){
            cout << "FATAL; recvfrom() failed!\n";
            exit(-1);
            }
        cout << "got something!\n";
        // understand request:
        char * rfn = new char [REQUEST_SIZE_MAX - 11];     // requested file name
        bool * ascii = new bool;
        ascii = false;
        int lastReq = processRequest(buf, rfn, ascii);
        if (lastReq < 0) {
            cout << "WARNING; unknown request from: ";
            cout << inet_ntoa(srvset.rsock.sin_addr) << ':';
            cout << ntohs(srvset.rsock.sin_port) << endl;
            continue;   // listen next
            }
        cout << "NEW; request from: ";
        cout << inet_ntoa(srvset.rsock.sin_addr) << ':';
        cout << ntohs(srvset.rsock.sin_port) << endl;
        
        // create new op for incomming connection
        counter++;      // hit threadID counter
        operation op;
        op.id = counter;
        op.rsock = srvset.rsock;
        strncpy((char *)op.rhost, inet_ntoa(srvset.rsock.sin_addr), 256);
        strncpy((char *)op.rfile, rfn, 256);
        op.state = STATE_SERVER | lastReq;
        op.netascii = ascii;
        op.abort = false;
        // find empty slot
        for (i = 0; i < THREADS; i++){
            if (ops[i].id == -1) {
                slot = i;
                break;
                }
            }
        
        // lock mutex Stats & put op into ops
        pthread_mutex_lock(&mutexStats);
        ops[slot] = op;
        pthread_mutex_unlock(&mutexStats);
        
        // create new thread for rx or tx 
        int rc;
        if (lastReq == STATE_RECEIVE){
            rc = pthread_create(&(ops[slot].trd), &attr, &rx, &ops[slot]); 
        } else {
            rc = pthread_create(&(ops[slot].trd), &attr, &tx, &ops[slot]); 
            }
        if (rc){
            cout << "ERROR; pthread_create(): " << rc << endl;
            exit(-1);
            }
        cout << "Thread created!\n";
        }
    // safe exit:
    pthread_mutex_lock(&mutexSrvset);
    srvset.running = false;
    pthread_mutex_unlock(&mutexSrvset);
    
    pthread_exit((void*) ss);
    }

/*  returns slot index for opID (first emty one when -1 given)      
    *This is not in the class as it is called by server function
                                                                    */
int getSlot(int opID){
    for (int i = 0; i < THREADS; i++){
        if (ops[i].id == opID) {
            return i;
            }
        }
        return 1;       // Not found
    }

// TODO: Create a non class member function and call from here instead
char * rxtx::nameLookup(char * rhost){
    hostent * host;     /* host information */
    in_addr h_addr;    /* internet address */
    
    host = gethostbyname(rhost);
    if (host == NULL) {
        return (char *) '\0';      // not found
        }
    h_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
    return (char *) inet_ntoa(h_addr);
    }

void * transfer(void * opID){
    //cout << "Starting transfer\n";
    // Get the communication:
    operation * a;
    a = & ops[ getSlot( (int) opID) ];
    // Raise error on array fault
    if (a->id < 0) {
        cout << "ERROR: opID cannot be found in slots!\n";
        exit(2);
        }
    // Find out operation by checking state:
    bool sending = ( (a->state ^ 2) < a->state);      // transmit bit set
    
    // load related function:
    if (sending) {
        tx(a);
    } else {
        rx(a);
        }
    pthread_exit((void*) opID);     // Exit
    }

/* Initialize and set thread detached attribute */
rxtx::rxtx(){
    pthread_mutex_init(&mutexStats, NULL);  //  init Stats mutex
    pthread_mutex_init(&mutexSrvset, NULL); //  init Srvset mutex
    pthread_attr_init(&attr);               // init & set thread attr
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    counter = 0;                            // reset counter
    for (int i = 0; i < THREADS; i++){
        ops[i].id = -1;                     // empty thread slots
        }
    }

/* create thread for sending & receiving data as client. Return opID
    @gp: 1: Get, 2:Put
    TODO:   Overload this function to make command line downloader
            (something like: startClient(host, file) )
*/
int rxtx::startClient(char * host, char * file, bool send){
    counter++;              // increase counter
    operation o;            // new slot
    o.state = 2 * send;     // 0: receive, 2: send
    strncpy(o.rhost, host, 256);    // remote host address
    strncpy(o.rfile, file, 256);    // treated as local when sending
    o.abort = false;
    o.id = counter;
    
    short int slotIndex = getSlot(-1);
    
    pthread_mutex_lock(&mutexStats);        // lock mutex
    ops[slotIndex] = o;                     // write  new one to ops
    pthread_mutex_unlock(&mutexStats);      // unlock
    
    int rc;
    rc = pthread_create(&(ops[slotIndex].trd), &attr, transfer, 
                                                    (void *)counter); 
    if (rc){
        cout << "ERROR; Client pthread_create(): " << rc << endl;
        exit(-1);
        }
    
    // wait for thread to mark initialization:
    while ((ops[slotIndex].state ^ 16) < ops[slotIndex].state){
        pthread_yield();
        }
    return counter;
    }

/* return operation #opID */
operation * rxtx::op(short int opID){
    for (int i = 0; i < THREADS; i++){
        if (ops[i].id == opID) {
            return &ops[i];
            }
        }
    cout << "Thread " << opID << " cannot be found: " << endl;
    cout << "ops: [ ";
    for (int i = 0; i < THREADS; i++){
        if (ops[i].id != -1){
            cout << ops[i].id << ',';
            }
        }
    cout << "]\n";
    exit(-1);
    }
/*  abort operation #opID   */
int rxtx::abort(short int opID){
    operation * a;
    a = op(opID);
    
    if (a->abort == true) {
        cout << "Already aborted.\n";
    } else {
        pthread_mutex_lock(&mutexStats);
        a->abort = true;                    // set abort flag
        pthread_mutex_unlock(&mutexStats);
        }

    int rc;
    rc = pthread_join(a->trd, &status);     // join, wait for finish
    if (rc) {
            cout << "ERROR; pthread_join(): " << rc << endl;
            exit(-1);
            }
    for (int i = 0; i < THREADS; i++){
        if (ops[i].id == opID) {
            pthread_mutex_lock(&mutexStats);
            ops[i].id = -1;              // remove id
            pthread_mutex_unlock(&mutexStats);
            }
        }
    return rc;
    }
// print summary:
void rxtx::printList(){
    operation a;
    int opcount;
    cout << "Slot\tID\tHost\t\tFile\tTransferred\tTotal\tState\n";
    for (int i = 0; i < THREADS; i++){
        if (ops[i].id == -1) {
            continue;
            }
        opcount++;
        a = ops[i];
        cout << i << '\t' << a.id << '\t' << a.rhost << '\t' << a.rfile;
        cout << '\t' << a.progress << "\t\t" << a.fileSize << '\t';
        cout << a.state << endl;
        }
    }

/* fire up the server thread. Return 0 if successful                */
int rxtx::startServer(int port){
    srvset.abort = false;
    srvset.rbl = REQUEST_SIZE_MAX;
    srvset.port = port;
    srvset.gTimeout = 10000; // timeout
    srvset.pTimeout = 3000; // rexmt
    int rc;
    rc = pthread_create(&(srvset.trd), &attr, server, (void *)counter);
    if (rc){
        cout << "ERROR; Server pthread_create(): " << rc << endl;
        exit(-1);
        }
    while (srvset.running != true){
        pthread_yield();
        }
    return 0;
    }


/* fill in array for active ops and return pointer                  */
void rxtx::getOps(int * rv, int & rvp){
    memset(rv, 0, sizeof (int) * THREADS);
    rvp = 0;
    for (int i = 0; i < THREADS; i++){
        if (ops[i].id == -1) {
            continue;
            }
        rv[rvp] = ops[i].id;
        rvp++;
        }
    }
