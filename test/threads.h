// compile with -lpthread
// for print:
# include <iostream>
// for strncpy:
# include <cstring>
// for exit() and sleep():
# include <cstdlib>
// for pthread:
# include <pthread.h>
// for tracking thhreads:
# include <vector>

using namespace std;

struct tStat {
    pthread_t trd;
    short int id;
    short int state;
    int target;
    int progress;
    char rhost [256];
    char rfile [256];
    bool abort;
    };

class anne {
    public:
        anne();                                         // multi thread
        void printThreads();
        short int startNew(char * host, char * file);   // returns new thread ID
        int abort(long tid);        // abort thread t
        int getTarget(long tid);    // get target filesize?
        int getProgress(long tid);  // get current progress
    private:
        pthread_attr_t attr;        // thread attribute
        void * status;              // for join
        tStat * gett(long tid);
    };
