/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#include "CommandTimerView.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TimeView"


CommandTimerView::CommandTimerView(BRect frame)
	:
	BView(frame, "CommandTimerView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW
		| B_SUPPORTS_LAYOUT)
{
	commandTextControl = new BTextControl("commandTextControl",
		B_TRANSLATE("Command:"), NULL, new BMessage('CMND'));
	commandTextControl->SetDivider(60);
	commandTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);

	pathTextControl = new BTextControl("pathTextControl",
		B_TRANSLATE("Use path:"), "/boot/home", new BMessage('CMND'));
	pathTextControl->SetDivider(60);
	pathTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	pathTextControl->SetEnabled(false);

	hoursTextControl = new BTextControl("hoursTextControl", NULL, "00", NULL);
	minsTextControl = new BTextControl("minsTextControl", NULL, "00", NULL);
	secsTextControl = new BTextControl("minsTextControl", NULL, "00", NULL);

	startStopButton = new BButton("startStopButton", B_TRANSLATE("Start"),
		new BMessage('CLOK'));

	repeatCheckBox = new BCheckBox("repeatCheckBox", B_TRANSLATE("Repeat"),
		new BMessage('REPT'));
	alarmCheckBox = new BCheckBox("alarmCheckBox", B_TRANSLATE("Alarm"),
		new BMessage('BEEP'));
	trackerCheckBox = new BCheckBox("trackerCheckBox",
		B_TRANSLATE("Use Tracker"), new BMessage('TRAK'));

	pathCheckBox = new BCheckBox("pathCheckBox", NULL, new BMessage('PATH'));

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(commandTextControl)
		.AddGroup(B_HORIZONTAL, 0)
			.Add(pathTextControl)
			.Add(pathCheckBox)
		.End()
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL, 0)
			.AddGroup(B_VERTICAL, 0)
				.Add(repeatCheckBox)
				.Add(alarmCheckBox)
				.Add(trackerCheckBox)
			.End()
			.AddGroup(B_VERTICAL, 0)
				.AddGroup(B_HORIZONTAL, 0)
					.Add(hoursTextControl)
					.Add(minsTextControl)
					.Add(secsTextControl)
				.End()
				.Add(startStopButton)
			.End()
		.End()
	.End();

	isRunning = false;
	alarm = false;
	repeat = false;
	tracker = false;
	path = false;
}


CommandTimerView::~CommandTimerView()
{
	delete startStopButton;
	delete repeatCheckBox;
	delete alarmCheckBox;
	delete trackerCheckBox;
	delete pathCheckBox;
	delete commandTextControl;
	delete pathTextControl;
	delete hoursTextControl;
	delete minsTextControl;
	delete secsTextControl;
	delete runner;
}


void
CommandTimerView::AttachedToWindow()
{
	runner
		= new BMessageRunner(BMessenger(this), new BMessage('PULS'), 1000000);
}


void
CommandTimerView::doPulse()
{
	if (isRunning) {
		if (seconds)
			seconds--;
		else if (isRunning)
			seconds = secondsBackup;
		executeCommand();
		if (alarm)
			beep();
		if (!repeat) {
			setPermissions(true);
			isRunning = false;
			startStopButton->SetLabel("Start");
		}
		updateTime();
	}
}


void
CommandTimerView::updateTime()
{
	char digits[3];
	sprintf(digits, "%02ld", seconds / 3600);
	hoursTextControl->SetText(digits);
	sprintf(digits, "%02ld", (seconds / 60) % 60);
	minsTextControl->SetText(digits);
	sprintf(digits, "%02ld", seconds % 60);
	secsTextControl->SetText(digits);
}


void
CommandTimerView::startStopClock()
{
	if (isRunning) {
		setPermissions(true);
		isRunning = false;
		startStopButton->SetLabel(B_TRANSLATE("Start"));
	} else {
		setPermissions(false);
		isRunning = true;
		getTime();
		startStopButton->SetLabel(B_TRANSLATE("Stop"));
	}
}


void
CommandTimerView::toggleRepeat()
{
	repeat = !repeat;
}


void
CommandTimerView::toggleAlarm()
{
	alarm = !alarm;
}


void
CommandTimerView::toggleTracker()
{
	tracker = !tracker;
}


void
CommandTimerView::togglePath()
{
	path = !path;
	pathTextControl->SetEnabled(path);
}


void
CommandTimerView::setPermissions(bool permission)
{
	commandTextControl->SetEnabled(permission);
	pathCheckBox->SetEnabled(permission);
	pathTextControl->SetEnabled(permission ? path : false);
	hoursTextControl->SetEnabled(permission);
	minsTextControl->SetEnabled(permission);
	secsTextControl->SetEnabled(permission);
}


void
CommandTimerView::getTime()
{
	seconds = atoi(hoursTextControl->Text()) * 3600
		+ atoi(minsTextControl->Text()) * 60 + atoi(secsTextControl->Text());
	secondsBackup = seconds;
}


void
CommandTimerView::executeCommand()
{
	char* buf;
	int length = 3;

	if (path)
		length += strlen(pathTextControl->Text());
	if (tracker)
		length += 24;
	length += strlen(commandTextControl->Text());
	buf = new char[length];

	buf[0] = '\0';

	if (tracker)
		strcat(buf, "/boot/system/Tracker ");
	if (path) {
		strcat(buf, pathTextControl->Text());
		strcat(buf, "/");
	}

	strcat(buf, commandTextControl->Text());
	strcat(buf, " &");
	system(buf);

	delete[] buf;
}
