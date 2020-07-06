/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef COMMANDTIMERWINDOW_H
#define COMMANDTIMERWINDOW_H

#include "CommandTimerView.h"

#include <Application.h>
#include <CheckBox.h>
#include <InterfaceKit.h>
#include <MessageRunner.h>
#include <SupportKit.h>
#include <TextControl.h>
#include <Window.h>

class CommandTimerWindow : public BWindow
{
public:
	CommandTimerWindow(BRect cTWindowRect);

	virtual void MessageReceived(BMessage* cTMessage);
	virtual bool QuitRequested();

	void doPulse();
	void updateTime();
	void startStopClock();
	void toggleRepeat();
	void toggleAlarm();
	void toggleTracker();
	void togglePath();
	void setPermissions(bool permission);
	void getTime();
	void executeCommand();

private:
	bool isRunning;
	long seconds;
	long secondsBackup;
	bool alarm;
	bool repeat;
	bool tracker;
	bool path;

	BButton* startStopButton;
	BCheckBox* repeatCheckBox;
	BCheckBox* alarmCheckBox;
	BCheckBox* trackerCheckBox;
	BCheckBox* pathCheckBox;
	BTextControl* commandTextControl;
	BTextControl* pathTextControl;
	BTextControl* hoursTextControl;
	BTextControl* minsTextControl;
	BTextControl* secsTextControl;
	BMessageRunner* runner;
};

#endif
