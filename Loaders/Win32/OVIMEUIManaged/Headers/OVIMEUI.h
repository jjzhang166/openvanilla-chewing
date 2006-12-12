#pragma unmanaged
#include <windows.h>
#include <tchar.h>

#include "OVUtility.h"

typedef struct _tagUICHILD{
	HWND    hWnd;
	POINT   pt;
	SIZE    sz;
} UICHILD, *LPUICHILD;

extern "C" {
// Interfaces
// OVIMEUI.cpp
BOOL IMEUIRegisterClass( HINSTANCE );
BOOL IMEUIUnRegisterClass( HINSTANCE );
BOOL MyIsIMEMessage(UINT);
void UIPushInputMethod(wchar_t*);
int UICurrentInputMethod();

void UIConstruct();
void UIDispose();

// UIStatus.cpp
void UICreateStatusWindow(HWND);
void UIMoveStatusWindow(HWND, int, int);
void UIShowStatusWindow();
void UIHideStatusWindow();
void UISetCursorPos(int);
//Begin : move to UIComp.cpp//
//void UISetMarkFrom(int);
//void UISetMarkTo(int);
//End : move to UIComp.cpp//
void UIChangeModule(HWND);
void UIChangeModuleByMouse(HWND);
void UIChangeHalfFull(HWND);
void UIChangeChiEng(HWND);
void UISetStatusModStr(wchar_t*);
void UISetStatusMenuModStr(wchar_t*);
void UIClearStatusMenuModString();
// UIComp.cpp
void UICreateCompWindow(HWND);
void UIMoveCompWindow(int, int);
void UIShowCompWindow();
void UIHideCompWindow();
int UIGetHeight();
void UISetCompStr(wchar_t*);
void UISetCompCaretPosX(int);
void UISetMarkFrom(int);
void UISetMarkTo(int);
void UIClearCompStr();
// UICand.cpp
void UICreateCandWindow(HWND);
void UIMoveCandWindow(int, int);
//void UIMoveCandWindow(HWND, int, int, wchar_t*);
void UIShowCandWindow();
void UIHideCandWindow();
void UISetCandStr(wchar_t*);
// UINotify.cpp
void UICreateNotifyWindow(HWND);
void UIShowNotifyWindow(wchar_t*);
void UIHideNotifyWindow();
}