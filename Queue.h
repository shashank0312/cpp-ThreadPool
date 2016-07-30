#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include "Command.h"

using namespace std;

class CommandDecorator:public Command
{
    Command *inst;

public:
    CommandDecorator *next;
    CommandDecorator *previous;

    CommandDecorator(Command *obj):inst(obj),next(NULL),previous(NULL){}
    void execute()
    {
        //cout << "CommandDecorator::execute" << endl;
        inst->execute();
    }

};

class EmptyQueue:public exception
{
public:
    const char *what()
    {
        return "EmptyQueue Exception" ;
    }
};

class Queue
{
    CommandDecorator *front;

    CommandDecorator *rear;

    size_t miNumElements;

    pthread_mutex_t mQueueMutex;

    static Queue *inst;

protected:

    Queue():front(NULL),rear(NULL),miNumElements(0)
    {
        pthread_mutex_init(&mQueueMutex, NULL);
    }

public:

    static Queue* getInstance();

    void insertItem(Command *inst);

    Command *dequeueItem();

    size_t getNumElementsInQueue();
};
#endif
