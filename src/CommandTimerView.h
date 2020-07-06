#ifndef COMMANDTIMERVIEW_H
#define COMMANDTIMERVIEW_H

#include <CheckBox.h>
#include <InterfaceKit.h>
#include <MessageRunner.h>
#include <SupportKit.h>
#include <TextControl.h>
#include <View.h>
#include <stdio.h>
#include <stdlib.h>

class CommandTimerView : public BView
{
public:
	CommandTimerView(BRect cTViewRect);
	~CommandTimerView();

	virtual void AttachedToWindow();

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
