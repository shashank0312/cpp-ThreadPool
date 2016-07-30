#include "Client.h"
#include "signal.h"

DerivedCommand gObj[10];


void DerivedCommandOM::execute()
{


}

void DerivedCommandCRM::execute()
{


}
int main()
{
    signal(SIGINT, signalhandler);
    for (int i = 0; i < 10; i++)
        Queue::getInstance()->insertItem(&gObj[i]);

    /*for (int i = 0; i < 10; i++ ){
        Command *pInst = Queue::getInstance()->dequeueItem();
        pInst->execute();
    }*/
    Manager::getInstance()->createWorkerThread();
    Manager::getInstance()->checkQueue();



    return 0;
}
