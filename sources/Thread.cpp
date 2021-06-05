#include <Thread.h>

void Thread::fire (Thread * me)
{
    me->mainThreadBody(); //assigning the current thread to execute mainthreadbody function
    me->run_once.unlock();//end of critical section
}
Thread::Thread ()
{
    t = NULL;
}

thread * Thread::start ()
{
    run_once.lock();//start of critical section
    t = new thread(Thread::fire,this);//calling the private method fire from the class 
    return t; 
}
void Thread::wait()
{
    if ( t != NULL) t->join();//join each thread
}

Thread::~Thread()
{
    if ( t != NULL) delete(t);
}
