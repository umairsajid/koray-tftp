# include "threads.h"

using namespace std;

#define THREADS 50

//vector <tStat> tStats;
// try with array instead:
tStat tStats [THREADS];

// gives id to new threads:
short int counter = 0;

pthread_mutex_t mutexStats; // mutex to protect stats data

/* Initialize and set thread detached attribute */
anne::anne(){
    pthread_mutex_init(&mutexStats, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    size_t stacksize;
    pthread_attr_getstacksize(&attr, &stacksize);
    cout << "Stack size: " << stacksize << endl;
    counter = 0;
    }
/* get tStat by id */
tStat * anne::gett(signed long tid){
    for (int i = 0; i < (int) tStats.size(); i++){
        if (tStats.at(i).id == tid) {
            return &tStats.at(i);
            }
        }
    cout << "Thread " << tid << " cannot be found: " << tid << endl;
    for (int i = 0; i < (int) tStats.size(); i++){
        cout << tStats.at(i).id << endl;
        }
    exit(-1);
    }

void * doJob(void * k){
    int s;
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s!=0) {
        cout << "ERROR; pthread_setcancelstate" << endl;
        exit(-1);
        }
    
    tStat * a;
    
    
    for (int i = 0; i < (int) tStats.size(); i++){
        
        
        if (tStats.at(i).id == (int) k) {
            a = &tStats.at(i);
            }
        }
    
    cout << "I am worker " << k << " with ID: " << a->id;
    cout << " and State " << a->state << " Abort: " << a->abort;
    cout << " host: " << a->rhost << " file " << a->rfile << endl;
    
    pthread_mutex_lock(&mutexStats);
    a->state = 1;
    pthread_mutex_unlock(&mutexStats);
    
    a->target = 1500000;
    
    int j = 0;
    
    for (int i = 0; i < a->target; i++) {
        
        if (tStats.size() == 0) {
            cout << "ERROR; tStats.size()" << endl;
            exit(-1);
            }
        
        j++;
        
        pthread_mutex_lock(&mutexStats);
        a->progress = a->progress + 1;
        pthread_mutex_unlock(&mutexStats);
        
        if (a->abort == true) {
            cout << "ABORTED; " << k << " at: " << a->progress << endl;
            break;
            }
        }
//~     cout << "Goodbye world! I was worker " << a->id << endl;
    pthread_exit((void*) k);
    }

short int anne::startNew(char * host, char * file){
    // increase thread counter for new ID
    counter++;
    
    tStat ns;
    
    ns.abort = false;
    strncpy(ns.rhost, host, 256);
    strncpy(ns.rfile, file, 256);
    
    
    ns.progress = 0;
    ns.id = counter;
    
    pthread_mutex_lock(&mutexStats);
    tStats.push_back(ns);
    pthread_mutex_unlock(&mutexStats);
    
    // create thread:
    int rc;
    rc = pthread_create(&(tStats.back().trd), &attr, doJob, (void *)counter); 
    if (rc){
            cout << "ERROR; pthread_create(): " << rc << endl;
            exit(-1);
            }
    
    while (tStats.back().state != 1){
        pthread_yield();
        }
    
    return counter;
    }


int anne::abort(long tid){
    tStat * a;
    a = gett(tid);
    
    if (a->abort == true) {
        // already aborted
        cout << "Already aborted.\n";
    } else {
        pthread_mutex_lock(&mutexStats);
        a->abort = true;
        pthread_mutex_unlock(&mutexStats);
        }
    
    int rc;
//~     rc = pthread_cancel(a->trd);
//~     if (rc) {
//~             cout << "***ERROR; thread " << tid << " not found: " << a->trd << endl;
//~             cout << "***ERROR; pthread_cancel(): " << rc << endl;
//~             exit(-1);
//~             }
    rc = pthread_join(a->trd, &status);  // join, wait for finish
    if (rc) {
            
            cout << "ERROR; pthread_join(): " << rc << endl;
            exit(-1);
            }
    for (int i = 0; i < (int) tStats.size(); i++){
        if (tStats.at(i).id == tid) {
            pthread_mutex_lock(&mutexStats);
            tStats.erase(tStats.begin() + i);
            pthread_mutex_unlock(&mutexStats);
            }
        }
    return 0;
    }

void anne::printThreads(){
    tStat a;
    cout << "Total: " << tStats.size() << endl;
    for (int i = 0; i < (int) tStats.size(); i++){
        a = tStats.at(i);
        cout << "Id: " << a.id;
        cout << "\t   Abort: " << a.abort;
        cout << "\t   State: " << a.state;
        cout << "\t   Host: " << a.rhost;
        cout << "\t   File: " << a.rfile;
        cout << "\t   Progress: " << a.progress;
        cout << "\t   TID: " << a.trd << endl;
        }
    }

int anne::getTarget(long tid){
    tStat * a;
    a = gett(tid);
    return a->target;
    }

int anne::getProgress(long tid){
    tStat * a;
    a = gett(tid);
    return a->progress;
    }
