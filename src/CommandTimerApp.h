#ifndef COMMANDTIMERAPP_H
#define COMMANDTIMERAPP_H

#include "CommandTimerWindow.h"
#include <Application.h>

class CommandTimerApp : public BApplication
{
public:
	CommandTimerApp();

private:
	CommandTimerWindow* cTWindow;
};

#endif
