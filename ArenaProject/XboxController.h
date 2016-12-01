#pragma once
#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "XInput.lib")

/* ------------------------------------------

This code was influenced by the 
"Xbox 360 Controller Input in C++ with XInput"
tutorial at www.codeproject.com

--------------------------------------------- */

#define LTHUMB_DEADZONE 8000

class XboxController
{
public:

	XboxController(int controllerIndex);

	void updateState();
	XINPUT_STATE returnState();
	bool checkConnected();
	void rumble();
	~XboxController();

private:
	static const int LEFT_RUMBLEVAL = 32767;
	static const int RIGHT_RUMBLEVAL = 32767;

	XINPUT_STATE _currentState;
	bool _connected;
	int _controllerNum;
};

