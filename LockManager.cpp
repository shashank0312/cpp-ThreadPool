#include <iostream>
#include <pthread.h>
#include "LockManager.h"

using namespace std;

LockManager::LockManager(pthread_mutex_t *pMutex)
{
        mMutex = pMutex;

        pthread_mutex_init(mMutex, NULL);
        int ret = pthread_mutex_lock(mMutex);

        if (ret < 0 )
        {
            throw LockException();
        }
        if ( ret == 0 )
            cout << "Mutex Locked Succesfully" << endl;
}
LockManager::~LockManager()
{
    int ret = pthread_mutex_unlock(mMutex);
    if ( ret < 0 )
    {
        throw LockUnlockException();
    }
    else
        cout << "Mutex unLocked Succesfully" << endl;
}
