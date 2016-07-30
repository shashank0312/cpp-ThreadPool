#include <iostream>
#include "Queue.h"
#include "LockManager.h"
#include "Command.h"

using namespace std;

pthread_mutex_t gQueueMutex = PTHREAD_MUTEX_INITIALIZER;

Queue * Queue::inst = NULL;

Queue * Queue::getInstance()
{
    if ( inst != NULL )
        return inst;
    if (inst == NULL )
    {
        pthread_mutex_lock(&gQueueMutex);
        if ( inst == NULL )
            inst = new Queue();
        pthread_mutex_unlock(&gQueueMutex);
    }

    return inst;
}

void Queue::insertItem(Command *inst)
{
    CommandDecorator *pInst;

    LockManager lock(&mQueueMutex);
    if ( front == NULL)
    {
        front = new CommandDecorator(inst);
        rear = front;
        front->next = rear;
        rear->previous = front;
        miNumElements++;
    }
    else
    {
        pInst = new CommandDecorator(inst);

        pInst->previous = front;

        pInst->next = rear;
        front->next = pInst;

        front = pInst;
        rear->previous = front;
        miNumElements++;
    }

    if ( rear == NULL )
    {
        //rear = front;
        //miNumElements++;
    }
}

Command *Queue::dequeueItem()
{
    LockManager inst(&mQueueMutex);
    Command *pInst;
    if ( rear == NULL )
    {
        return NULL;
    }
    else if (miNumElements > 1)
    {
        rear->next->previous = rear->previous;
        //delete rear;
        pInst = rear;

        rear = rear->next;
        miNumElements--;
    }
    else
    {
        //delete rear;
        pInst = rear;
        rear = front = NULL;
        miNumElements--;
    }
    return pInst;
}

size_t Queue::getNumElementsInQueue()
{
    return miNumElements;
}
