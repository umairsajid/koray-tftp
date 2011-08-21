/* What to do:

commandLine:
    print cursor, wait for command
    cmd: put, get       ->  start new thread
    cmd: status         ->  read status of threads
    cmd: stop x         ->  stop thread x

*/
// compile with -lpthread
// for print:
# include <iostream>
// for exit() and sleep():
# include <stdlib.h>
// for pthread:
# include <pthread.h>

using namespace std;

#define NUM_THREADS     5

short int tStatus [NUM_THREADS];

void * PrintHello(void *threadid){
    long tid;
    tid = (long) threadid;
    
    for (int i = 50 - tid * 5; i > -1; i--) {
        sleep(1);
        tStatus[tid] = i;
    }
    cout << "Goodbye World! It's me, thread #" << tid << endl;;
    //
    //pthread_exit(NULL);
    // exit joinable:
    pthread_exit((void*) threadid);
    }

bool allFinished(){
    for (int i = 0; i < NUM_THREADS; i++){
        if (tStatus[i] > 0){
            return false;
            }
        }
    return true;
    }

int main (int argc, char *argv[]){
    // create thread array:
    pthread_t threads[NUM_THREADS]; 
    // declare attribute:
    pthread_attr_t attr;
    // error handling:
    int rc;
    // number for loop & argument to pass:
    long t;
    // for keeping record of thread status:
    void * status;
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    for(t=0; t<NUM_THREADS; t++){
        cout << "In main: creating thread " << t << endl;
        // create thread with NULL attribute:
        //rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
        // with some attribute, attr:
        rc = pthread_create(&threads[t], &attr, PrintHello, (void *)t); 
        // look for error:
        if (rc){
            cout << "ERROR; pthread_create(): " << rc << endl;
            exit(-1);
            }
        }
    while (1){
        cout << "===================" << endl;
        sleep(1);
        for(t=0; t<NUM_THREADS; t++){
            cout << "Thread " << t << " is at: " << tStatus[t] << endl;
        }
        if (allFinished() == true) {
            break;
            }
        }
    
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    
    for(t=0; t<NUM_THREADS; t++){
        rc = pthread_join(threads[t], &status);  // join, wait for finish
        if (rc) {
            cout << "ERROR; pthread_join(): " << rc << endl;
            exit(-1);
            }
        cout << "Joined: " << t << "\tStatus: " << status << endl;
        }
    
    
    cout << "Finished..." << endl;
    /* Last thing that main() should do */
    pthread_exit(NULL);
    }
