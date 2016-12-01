#include "XboxController.h"

XboxController::XboxController(int controllerIndex)
{
	_controllerNum = controllerIndex;
}

void XboxController::updateState()
{
	bool connectState;

	// Clear the current state memory so it can be re-allocated to new state
	ZeroMemory(&_currentState, sizeof(XINPUT_STATE));

	// Gets the new state of the controller 
	unsigned long Result = XInputGetState(_controllerNum, &_currentState);

	// If it's a success, it's connected
	if (Result == ERROR_SUCCESS)
	{
		connectState = true;
		
		if ( ( (abs(_currentState.Gamepad.sThumbLX)) > (abs(_currentState.Gamepad.sThumbLY)) ) && ( ( abs(_currentState.Gamepad.sThumbLX) ) > LTHUMB_DEADZONE) )
		{
			_currentState.Gamepad.sThumbLY = 0;
		}
		else if ( ( (abs(_currentState.Gamepad.sThumbLY) ) > ( abs(_currentState.Gamepad.sThumbLX) ) ) && ( ( abs(_currentState.Gamepad.sThumbLY) ) > LTHUMB_DEADZONE) )
		{
			_currentState.Gamepad.sThumbLX = 0;
		}
		else
		{
			_currentState.Gamepad.sThumbLX = 0;
			_currentState.Gamepad.sThumbLY = 0;
		}
	}
	else
	{
		connectState = false;
	}

	// We then update this connected status in the object
	_connected = connectState;

	// states can be retrieved via checkConnected() and returnState()
}

bool XboxController::checkConnected()
{
	return _connected;
}

XINPUT_STATE XboxController::returnState()
{
	return _currentState;
}

void XboxController::rumble()
{
	XINPUT_VIBRATION Vibration;

	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	Vibration.wLeftMotorSpeed = LEFT_RUMBLEVAL;
	Vibration.wRightMotorSpeed = RIGHT_RUMBLEVAL;

	XInputSetState(_controllerNum, &Vibration);
}


XboxController::~XboxController()
{
	ZeroMemory(&_currentState, sizeof(XINPUT_STATE));
}
