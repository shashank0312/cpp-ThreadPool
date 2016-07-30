#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <pthread.h>
#include <set>

#define MAX_WORKERS 10

using namespace std;

class Command;
class Worker;

struct LockStruct
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
};

void signalhandler(int signo);

class Manager
{

private:

    set<const Worker *> mFree;
    set<const Worker *> mBusy;
    int numWorkers;
    int miWaitingWorkers;
    LockStruct mLockStruct;
    pthread_mutex_t managerMutex;

    static Manager *inst;

protected:

    Manager();
    Manager(const Manager &rhs);
    Manager & operator=(const Manager &rhs);

public:
    volatile bool bContinue;
    void notifyFree(const Worker *id);
    void notifyBusy(const Worker *id);
    void incrementWaitingWorkers()
    {
        miWaitingWorkers++;
    }

    void decrementWaitingWorkers()
    {
        miWaitingWorkers--;
    }

    static Manager * getInstance();
    ~Manager();
    void addWorker(Worker *worker);
    Command *getRequest();
    void checkQueue();

    void createWorkerThread();

    //void createWorkers();

    void snoozeWorkers();
    //void registerWithWorkers();
    void procesRequest(Command *obj);
    Command *getRequestfromQueue();
};

class Worker
{
    Manager *subject;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    pthread_t  threadid;

public:
    Worker(pthread_mutex_t *mut,
           pthread_cond_t  *con):mutex(mut),cond(con){}

    void notifyManagerFree()
    {
        subject->notifyFree(this);
    }

    void notifyManagerBusy()
    {
        subject->notifyBusy(this);
    }

    void registerManager(Manager *inst)
    {
        subject =inst;
    }

    Command *pollRequest();

    void processRequest(Command *obj);

    void enableSignalling();

    pthread_t getThreadId()
    {
        return threadid;
    }

};
#endif
