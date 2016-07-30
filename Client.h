#include "Queue.h"
#include "Command.h"
#include "Manager.h"

#include <iostream>

using namespace std;

class DerivedCommandOM:public Command
{
    public:
        void execute();

};

class DerivedCommandCRM:public Command
{
	public:
		void execute();
};