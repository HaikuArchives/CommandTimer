#ifndef COMMANDTIMERAPP_H
#define COMMANDTIMERAPP_H

#include <Application.h>
#include "CommandTimerWindow.h"

class CommandTimerApp : public BApplication
{
public:
	CommandTimerApp();

private:
	CommandTimerWindow *cTWindow;
};

#endif