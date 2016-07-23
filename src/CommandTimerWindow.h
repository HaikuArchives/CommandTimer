#ifndef COMMANDTIMERWINDOW_H
#define COMMANDTIMERWINDOW_H

#include <Window.h>
#include <InterfaceKit.h>
#include <Application.h>
#include "CommandTimerView.h"

class CommandTimerWindow : public BWindow
{
public:
	CommandTimerWindow(BRect cTWindowRect);
	
	virtual void MessageReceived(BMessage *cTMessage);
	virtual bool QuitRequested();

private:
	CommandTimerView *cTView;
};

#endif
	