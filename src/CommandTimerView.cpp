#include "CommandTimerView.h"

CommandTimerView::CommandTimerView(BRect cTViewRect)
	: BView(cTViewRect, "CommandTimerView", B_FOLLOW_ALL, B_WILL_DRAW)
{
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

void CommandTimerView::AttachedToWindow()
{
	BRect tempRect(25, 15, 265, 25);
	commandTextControl = new BTextControl(tempRect, "commandTextControl", "Command:", NULL,
		new BMessage('CMND'), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE); 
	commandTextControl->SetDivider(60);
	commandTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	AddChild(commandTextControl);

	tempRect.Set(25, 45, 265, 55);
	pathTextControl = new BTextControl(tempRect, "pathTextControl", "Use Path:", NULL,
		new BMessage('CMND'), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE); 
	pathTextControl->SetDivider(60);
	pathTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	pathTextControl->SetText("/boot/home");
	pathTextControl->SetEnabled(false);
	AddChild(pathTextControl);

	tempRect.Set(60, 78, 82, 90);
	hoursTextControl = new BTextControl(tempRect, "hoursTextControl", NULL, "00",
		NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(hoursTextControl);
	
	tempRect.Set(85, 78, 107, 90);
	minsTextControl = new BTextControl(tempRect, "minsTextControl", NULL, "00",
		NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(minsTextControl);
	
	tempRect.Set(110, 78, 132, 90);
	secsTextControl = new BTextControl(tempRect, "minsTextControl", NULL, "00", 
		NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(secsTextControl);
 
	tempRect.Set(175, 75, 225, 85);
	startStopButton = new BButton(tempRect, "startStopButton", "Start",
		new BMessage('CLOK'));
	AddChild(startStopButton);
	
	tempRect.Set(30, 105, 100, 115);
	repeatCheckBox = new BCheckBox(tempRect, "repeatCheckBox", "Repeat", 
		new BMessage('REPT'), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(repeatCheckBox);

	tempRect.Set(120, 105, 190, 115);
	alarmCheckBox = new BCheckBox(tempRect, "alarmCheckBox", "Alarm", 
		new BMessage('BEEP'), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(alarmCheckBox);

	tempRect.Set(200, 105, 290, 115);
	trackerCheckBox = new BCheckBox(tempRect, "trackerCheckBox", "Use Tracker", 
		new BMessage('TRAK'), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(trackerCheckBox);
	
	tempRect.Set(270, 45, 300, 55);
	pathCheckBox = new BCheckBox(tempRect, "pathCheckBox", NULL, 
		new BMessage('PATH'), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(pathCheckBox);

	isRunning = false;
	alarm = false;
	repeat = false;
	tracker = false;
	path = false;
	
	runner = new BMessageRunner(BMessenger(this), new BMessage('PULS'), 1000000);
}	

void CommandTimerView::doPulse()
{
	if(isRunning)
	{
		if(seconds)
			seconds--;
		else
		{
			if(isRunning)
			{
				seconds = secondsBackup;
				executeCommand();
				if(alarm)
					beep();
				if(!repeat) 
				{
					setPermissions(true);
					isRunning = false;
					startStopButton->SetLabel("Start");
				}
			}
		}
		updateTime();
	}
}

void CommandTimerView::updateTime()
{
	char digits[3];
	sprintf(digits, "%02ld", seconds / 3600);
	hoursTextControl->SetText(digits);
	sprintf(digits, "%02ld", (seconds / 60) % 60);
	minsTextControl->SetText(digits);
	sprintf(digits, "%02ld", seconds % 60);
	secsTextControl->SetText(digits);
}

void CommandTimerView::startStopClock()
{
	if(isRunning)
	{
		setPermissions(true);
		isRunning = false;
		startStopButton->SetLabel("Start");
	}
	else
	{
		setPermissions(false);
		isRunning = true;
		getTime();
		startStopButton->SetLabel("Stop");
	}
}

void CommandTimerView::toggleRepeat()
{
	repeat = !repeat;
}

void CommandTimerView::toggleAlarm()
{
	alarm = !alarm;
}

void CommandTimerView::toggleTracker()
{
	tracker = !tracker;
}

void CommandTimerView::togglePath()
{
	path = !path;
	pathTextControl->SetEnabled(path);
}

void CommandTimerView::setPermissions(bool permission)
{
	commandTextControl->SetEnabled(permission);
	pathCheckBox->SetEnabled(permission);
	pathTextControl->SetEnabled(permission ? path : false);
	hoursTextControl->SetEnabled(permission);
	minsTextControl->SetEnabled(permission);
	secsTextControl->SetEnabled(permission);
}

void CommandTimerView::getTime()
{
	seconds = atoi(hoursTextControl->Text()) * 3600
		+ atoi(minsTextControl->Text()) * 60
		+ atoi(secsTextControl->Text());
	secondsBackup = seconds;
}

void CommandTimerView::executeCommand()
{
	char *buf;
   int length = 3;
   
   if(path)
      length += strlen(pathTextControl->Text());
   if(tracker)
      length += 24;
   length += strlen(commandTextControl->Text());
	buf = new char[length];

   buf[0] = '\0';

   if(tracker)
		strcat(buf, "/boot/beos/system/Tracker ");
   if(path)
   {
      strcat(buf, pathTextControl->Text());
      strcat(buf, "/");
   }
   
   strcat(buf, commandTextControl->Text());
   strcat(buf, " &");
	system(buf);

	delete[] buf;
}

