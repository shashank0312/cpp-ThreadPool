#include <iostream>
#include "Manager.h"
#include "Command.h"
#include "Queue.h"
#include "LockManager.h"

using namespace std;

volatile bool bContinue = true;

pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

Manager *Manager::inst = NULL;

void signalhandler(int signo)
{
    Manager::getInstance()->bContinue = false;
    bContinue = false;
}

void * createWorker(void *pLockStruct)
{
    LockStruct *pInst = reinterpret_cast<LockStruct *>(pLockStruct);
    Worker *worker = new Worker(&pInst->mutex, &pInst->cond);

    Manager::getInstance()->addWorker(worker);

    while ( bContinue)
    {
        Command *req = worker->pollRequest();
        worker->notifyManagerBusy();
        worker->processRequest(req);
    }
    pthread_exit(NULL);
    return reinterpret_cast<void *>(worker);
}

Manager *Manager::getInstance()
{
    if ( inst == NULL )
    {
        pthread_mutex_lock(&gMutex);
        if ( inst == NULL )
        {
            inst = new Manager();
            pthread_mutex_unlock(&gMutex);
        }
    }
    return inst;
}

void Manager::checkQueue()
{
    Queue *pQueue = Queue::getInstance();

    while (bContinue)
    {
        // If no workers are free and none is waiting
        // for the manager to signal the work.
        if (    mFree.size()     == 0
            &&  miWaitingWorkers == 0)
        {
            sleep(5);
            continue;
        }

        if ( pQueue->getNumElementsInQueue() > 0 && miWaitingWorkers > 0 )
        {
            pthread_mutex_lock(&mLockStruct.mutex);

            pthread_cond_signal(&mLockStruct.cond);

            cout << "Signalled the condition variable" << endl;

            pthread_mutex_unlock(&mLockStruct.mutex);
        }
    }

    set<const Worker *>::iterator iter = mFree.begin();
    pthread_t threadid;
    /*for (; iter != mFree.end(); ++iter ){
        threadid = iter->getThreadId();
        pthread_join(threadid, NULL );
    }*/

}

void Manager::createWorkerThread()
{
    pthread_t threadid;

    for ( int i = 0; i < 10; i++)
    {
        int ret = pthread_create(   &threadid, NULL,
                                    createWorker,
                                    reinterpret_cast<void *>(&mLockStruct));

    }
}

void Manager::addWorker(Worker *worker)
{
    LockManager Lock(&managerMutex) ;

    mFree.insert(worker);
    numWorkers++;

    worker->registerManager(this);
}

void Manager::notifyFree(const Worker *inst)
{
    set<const Worker *>::iterator iter = mBusy.find(inst);

    if ( iter != mBusy.end() )
    {
        LockManager lock(&managerMutex);
        mBusy.erase(*iter );
        mFree.insert(inst);
    }
}

void Manager::notifyBusy(const Worker *inst)
{
    set<const Worker *>::iterator iter = mFree.find(inst);

    if ( iter != mFree.end() )
    {
        LockManager lock(&managerMutex);

        mFree.erase(*iter );
        mBusy.insert(inst);

    }
}

Manager::~Manager()
{
    set<const Worker *>::iterator iter = mFree.begin();
    for (;iter != mFree.end(); ++iter )
    {
        delete const_cast<Worker *>(*iter);
    }

    iter = mBusy.begin();

    for (; iter != mBusy.end(); ++iter)
    {
        delete const_cast<Worker *>(*iter);
    }

}

Manager::Manager()
{
    bContinue = true;

    miWaitingWorkers = 0;

    pthread_mutex_init(&mLockStruct.mutex, NULL);

    pthread_cond_init(&mLockStruct.cond, NULL);

    pthread_mutex_init(&managerMutex, NULL);
}

Command *Manager::getRequest()
{
    return Queue::getInstance()->dequeueItem();
}


// The callback should be invoked. It should be exception safe.
void Worker::processRequest(Command *obj)
{
    obj->execute();
}

// Poll for request from the queue.
Command *Worker::pollRequest()
{
    Command *pInst = NULL;

    bool bIncrementWaiter = true;

    pthread_mutex_lock(mutex);

    while ( (pInst = subject->getRequest() ) == NULL )
    {
        if ( bIncrementWaiter)
            subject->incrementWaitingWorkers();
            //miWaitingWorkers++;

        pthread_cond_wait(cond, mutex);

        bIncrementWaiter = false;
    }

    if ( !bIncrementWaiter )
        //miWaitingWorkers--;
        subject->decrementWaitingWorkers();

    cout << "Thread with id: " << threadid << " received message from Queue" << endl;

    pthread_mutex_unlock(mutex);

    return pInst;
}

void Worker::enableSignalling()
{
    //pthread_mutex_t inst = subject->get
}
