// compile with -lpthread
// for print:
# include <iostream>
// for exit() and sleep():
# include <stdlib.h>
// for pthread:
# include <pthread.h>
// for example functionality:
# include <vector>
# include <cassert>

using namespace std;

class threaded_class
{
public:
    threaded_class()
        : m_stoprequested(false), m_running(false)
    {
        pthread_mutex_init(&m_mutex);
    }
 
    ~threaded_class()
    {
        pthread_mutex_destroy(&m_mutex);
    }

/*  Create the thread and start work
    Because we are required to pass a C style function pointer into the 
    pthread_create function we end up with at least 2 (and in our case 
    3) functions to actually kick off the thread.

        -go() (the public interface for starting the work) calls 
            pthread_create() which calls:
        -start_thread() (the C style interface for starting the 
            thread) which finally calls:
        -do_work() the object level private entry into the thread.    */
    void go() 
    {
        assert(m_running == false);
        m_running = true;
        pthread_create(&m_thread, 0, &threaded_class::start_thread, this);
    }
/*  stop() shuts down the thread and calls pthread_join() which does not
    return until the thread has been fully shutdown. What happens here 
    if we forget to call stop? We may end up with a runaway thread that 
    we have no way of shutting down in the best case. In the worst case 
    we get a crash. The crash occurs when the threaded_class gets 
    destroyed by the main thread of execution but the do_work thread is 
    still running and is now trying to work on destructed data.*/ 
    void stop() // Note 2
    {
        assert(m_running == true);
        m_running = false;
        m_stoprequested = true;
        pthread_join(&m_thread, 0);
    }
 
    int get_fibonacci_value(int which)
    {
        pthread_mutex_lock(&m_mutex); // Note 3 
        int value = m_fibonacci_values.get(which); // Note 4 
        pthread_mutex_unlock(&m_mutex);
        return value;
    }
 
private:
/*  m_stoprequested must be defined volatile for code correctness. 
    There is a chance it would work if it were not, but without volatile
    it is possible to create a situation where the thread would never 
    exit because it would not know that m_stoprequested had changed to 
    true        */
    volatile bool m_stoprequested;
    volatile bool m_running;
    pthread_mutex_t m_mutex; // Variable declarations added 4/14/2010
    pthread_t m_thread;
 
    std::vector<int> m_fibonacci_values;
 
    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<threaded_class *>(obj)->do_work();
    }
 
    int fibonacci_number(int num)
    {
        switch(num)
        {
            case 0:
            case 1:
                return 1;
            default:
                return fibonacci_number(num-2) + fibonacci_number(num-1);
        };
    }    
 
    // Compute and save fibonacci numbers as fast as possible
    void do_work()
    {
        int iteration = 0;
        while (!m_stoprequested)
        {
            int value = fibonacci_number(iteration);
            pthread_mutex_lock(&m_mutex);
            m_fibonacci_values.push_back(value);
            pthread_mutex_unlock(&m_mutex); // against deadlock
        }
    }                    
};
