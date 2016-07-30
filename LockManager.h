#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H

#include <iostream>
#include <pthread.h>

using namespace std;

class LockException:public exception
{
    public:
        const char * what()
        {
            return "LockException" ;
        }
};

class LockUnlockException:public exception
{
    public:
        const char *what()
        {
            return "LockUnlockException";
        }
};

class LockManager
{
    pthread_mutex_t *mMutex;

public:
    LockManager(pthread_mutex_t *pMutex);

    ~LockManager();
    /*{
        int ret = pthread_mutex_unlock(&mutex);
        if ( ret < 0 )
        {
            throw LockUnlockException();
        }
    }*/
};
#endif
