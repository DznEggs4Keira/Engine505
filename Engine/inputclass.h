////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <D3DX10math.h>
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);

	//Exit
	bool IsEscapePressed();
	
	//Rotate camera via keyboard keys -- NOT USED
	bool IsLeftPressed(); // turn left
	bool IsRightPressed(); // turn right
	bool IsUpPressed(); // look up
	bool IsDownPressed(); // look down

	//rotate camera via mouse
	D3DXVECTOR3 GetMouseMovement();

	//Hitting
	bool IsEPressed();

	//Movement
	bool IsWPressed(); // move forward
	bool IsSPressed(); // move backward
	bool IsAPressed(); // move up
	bool IsDPressed(); // move down

	bool IsFPressed(); // Post Process Effect
	bool IsQPressed(); // 2nd Post Process Effect

	// randomly place and scale volumetric spheres
	bool isRPressed();

	//Procedural Terrains
	bool IsPPressed(); //Particle Deposition

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif