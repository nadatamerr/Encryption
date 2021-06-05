#include <ThreadManager.h>


ThreadManager::ThreadManager(){

}
void ThreadManager::operator += (Thread * t)
{
    threads.push_back(t);
}
void ThreadManager::start ()
{
    
    for(auto t=threads.begin(); t != threads.end(); t++)
    {
        t[0]->start();//calling start function from class thread for each thread in the vector Threads
    }
    //This function needed to be implemented by you
}
void ThreadManager::barrier ()
{
      for(auto t=threads.begin(); t != threads.end(); t++)
    {
        t[0]->wait();//calling wait function from class thread for each thread in the vector Threads to wait and join the threads
    }
    //This function needed to be implemented by you
}
ThreadManager::~ThreadManager(){
}
