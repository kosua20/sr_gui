

#if defined(__cplusplus)

#include "sr_gui.h"
#include "sr_gui_internal.h"
#include <stdio.h>
#include <wchar.h>
#include <windows.h>

// Transfer output ownership to the caller.
wchar_t* _sr_gui_widen_string(const char* str) {
	const int sizeWide = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wchar_t* output = (wchar_t*) SR_GUI_MALLOC(sizeWide  * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, output, sizeWide);
	return output;
}

// Transfer output ownership to the caller.
char* _sr_gui_narrow_string(const wchar_t* wstr) {
	const int sizeNarrow = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0);
	char* output = (char*) SR_GUI_MALLOC(sizeNarrow * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, output, sizeNarrow, NULL, NULL);
	return output;
}

void sr_gui_show_message(const char* title, const char* message, int level) {
	wchar_t* titleW = _sr_gui_widen_string(title);
	wchar_t* messageW = _sr_gui_widen_string(message);

	unsigned long flags = MB_OK;
	if(level == SR_GUI_MESSAGE_LEVEL_ERROR) {
		flags |= MB_ICONERROR;
	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN) {
		flags |= MB_ICONWARNING;
	} else {
		flags |= MB_ICONINFORMATION;
	}
	
	int res = MessageBox(NULL, messageW, titleW, flags);
	(void) res;

	free(titleW);
	free(messageW);
}

void sr_gui_show_notification(const char* title, const char* message) {
	
}

int sr_gui_ask_directory(const char* title, const char* startDir, char** outPath) {
	return SR_GUI_CANCELLED;
}

int sr_gui_ask_load_files(const char* title, const char* startDir, const char* exts, char*** outPaths, int* outCount) {
	return SR_GUI_CANCELLED;
}

int sr_gui_ask_save_file(const char* title, const char* startDir, const char* exts, char** outpath) {
	return SR_GUI_CANCELLED;
}

// Non thread safe, check if SetWindowText et al. are thread safe or not.
static wchar_t* _sr_gui_button0 = nullptr;
static wchar_t* _sr_gui_button1 = nullptr;
static wchar_t* _sr_gui_button2 = nullptr;

static LRESULT __stdcall _sr_gui_change_message_captions(int nCode, WPARAM wParam, LPARAM lParam) {
	if(nCode == HCBT_ACTIVATE) {
		SetWindowText(GetDlgItem((HWND)wParam, IDYES), _sr_gui_button0);
		SetWindowText(GetDlgItem((HWND)wParam, IDNO), _sr_gui_button1);
		SetWindowText(GetDlgItem((HWND)wParam, IDCANCEL), _sr_gui_button2);
	}
	return 0;
}

int sr_gui_ask_choice(const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2) {
	wchar_t* titleW = _sr_gui_widen_string(title);
	wchar_t* messageW = _sr_gui_widen_string(message);

	_sr_gui_button0 = _sr_gui_widen_string(button0);
	_sr_gui_button1 = _sr_gui_widen_string(button1);
	_sr_gui_button2 = _sr_gui_widen_string(button2);
	unsigned long flags = MB_YESNOCANCEL;

	if(level == SR_GUI_MESSAGE_LEVEL_ERROR) {
		flags |= MB_ICONERROR;
	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN) {
		flags |= MB_ICONWARNING;
	} else {
		flags |= MB_ICONINFORMATION;
	}
	HHOOK hook = SetWindowsHookEx(WH_CBT, _sr_gui_change_message_captions, NULL, GetCurrentThreadId());
	int res = MessageBox(NULL, messageW, titleW, flags);
	UnhookWindowsHookEx(hook);

	// Cleanup.
	free(titleW);
	free(messageW);
	free(_sr_gui_button0); _sr_gui_button0 = nullptr;
	free(_sr_gui_button1); _sr_gui_button1 = nullptr;
	free(_sr_gui_button2); _sr_gui_button2 = nullptr;

	if(res == IDYES) {
		return SR_GUI_BUTTON0;
	} else if(res == IDNO) {
		return SR_GUI_BUTTON1;
	}
	return SR_GUI_BUTTON2;
}

char* _sr_gui_message_result = nullptr; // receives name of item to delete.


#define _SR_GUI_ID_MESSAGE   150
#define _SR_GUI_ID_TEXT   200

BOOL CALLBACK _sr_gui_message_callback(HWND hwndDlg, UINT message, WPARAM wParam,  LPARAM lParam){
	switch(message)
	{
		case WM_CTLCOLOR:
			break;
		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case IDOK:
				{
					wchar_t stringRes[1024];
					if( GetDlgItemText( hwndDlg, _SR_GUI_ID_TEXT, stringRes, 1024 ) ){
						if(_sr_gui_message_result != nullptr){
							SR_GUI_FREE(_sr_gui_message_result);
						}
						_sr_gui_message_result = _sr_gui_narrow_string(stringRes);

					} else {
						_sr_gui_message_result = nullptr;
					}

					EndDialog( hwndDlg, SR_GUI_VALIDATED );
					return TRUE;
				}
				case IDCANCEL:
					EndDialog( hwndDlg, SR_GUI_CANCELLED );
					return TRUE;
				default:
					break;
			}
		default:
			break;
	}
	return FALSE;
}

LPWORD _sr_gui_align_ptr( LPWORD lpIn )
{
	unsigned long long ul = (unsigned long long) lpIn;
	ul++;
	ul >>= 1;
	ul <<= 1;
	return (LPWORD) ul;
}

int sr_gui_ask_string( const char* title, const char* message, char** result ) {

	// There is no easy way to have such a prompt on Windows it seems.
	HGLOBAL hgbl = GlobalAlloc(GMEM_ZEROINIT, 4096);
	if(!hgbl){
		return SR_GUI_CANCELLED;
	}

	wchar_t* titleW = _sr_gui_widen_string(title);
	wchar_t* messageW = _sr_gui_widen_string(message);

	// Define a dialog box.
	LPDLGTEMPLATEEX lpdt = (LPDLGTEMPLATEEX) GlobalLock(hgbl);
	lpdt->dlgVer = 1;
	lpdt->signature = 0xFFFF;
	lpdt->style = DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION | DS_SHELLFONT;
	lpdt->cDlgItems = 4;  // Number of controls
	lpdt->x = 10;  lpdt->y = 10;
	lpdt->cx = 360/2; lpdt->cy = 120/2;
	lpdt->menu = 0;
	lpdt->windowClass = 0;
	lpdt->title = titleW;
	lpdt->pointsize = 12;
	lpdt->weight = FW_NORMAL;
	lpdt->italic = FALSE;
	lpdt->charset = DEFAULT_CHARSET;
	lpdt->typeface = L"MS Shell Dlg";
	// Place ourselves after the struct.
	LPWORD lpw = (LPWORD) ( &(lpdt->typeface) + strlen(L"MS Shell Dlg") * sizeof(WCHAR) + 1 ) + 1;

	// Define an OK button.
	//lpw = lpwAlign( lpw )
	LPDLGITEMTEMPLATEEX lpdit = (LPDLGITEMTEMPLATEEX) lpw;
	lpdit->x = 173 / 2; lpdit->y = 88 / 2;
	lpdit->cx = 83 / 2; lpdit->cy = 23 / 2;
	lpdit->id = IDOK;
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;
	lpdit->windowClass = (0xFFFF << 16 | 0x0080); // Button class
	lpdit->title = L"OK";
	//MultiByteToWideChar(CP_ACP, 0, "OK", -1, , 50);
	lpdit->extraCount = 0;

	lpw = (LPWORD) ( &(lpdit->extraCount) + 1 ) + 1;

	// Define a cancel button
	//lpw = lpwAlign( lpw );
	lpdit = (LPDLGITEMTEMPLATEEX) lpw;
	lpdit->x = 263 / 2; lpdit->y = 88 / 2;
	lpdit->cx = 83 / 2; lpdit->cy = 23 / 2;
	lpdit->id = IDCANCEL;
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
	lpdit->windowClass = (0xFFFF << 16 | 0x0080); // Button class
	lpdit->title = L"Cancel";
	//MultiByteToWideChar(CP_ACP, 0, "Cancel", -1, lpdit->title, 50);
	lpdit->extraCount = 0;

	lpw = (LPWORD) ( &(lpdit->extraCount) + 1 ) + 1;

	// Define the dialog message.
	//lpw = lpwAlign( lpw );
	lpdit = (LPDLGITEMTEMPLATEEX) lpw;
	lpdit->x = 10; lpdit->y = 10;
	lpdit->cx = 100 / 2; lpdit->cy = 20 / 2;
	lpdit->id = _SR_GUI_ID_MESSAGE;
	lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;
	lpdit->windowClass = (0xFFFF << 16 | 0x0082); // Label class
	lpdit->title = messageW;
	lpdit->extraCount = 0;

	lpw = (LPWORD) ( &(lpdit->extraCount) + 1 ) + 1;

	// Define the input text field.
	//lpw = lpwAlign( lpw );
	lpdit = (LPDLGITEMTEMPLATEEX) lpw;
	lpdit->x = 10; lpdit->y = 30;
	lpdit->cx = 300 / 2; lpdit->cy = 20 / 2;
	lpdit->id = _SR_GUI_ID_TEXT;
	lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;
	lpdit->windowClass = (0xFFFF << 16 | 0x0081); // Field class
	lpdit->title = L"Default value";
	//MultiByteToWideChar(CP_ACP, 0, "Default value", -1, , 50);
	lpdit->extraCount = 0;

	GlobalUnlock(hgbl);
	int ret = DialogBoxIndirect(NULL, (LPDLGTEMPLATEEX) hgbl, NULL, (DLGPROC) DialogProc);
	GlobalFree(hgbl);
	free(titleW);
	free(messageW);

	if(ret != SR_GUI_VALIDATED || (_sr_gui_message_result == nullptr)){
		return SR_GUI_CANCELLED;
	}

	// Copy result back via static variable.
	// Instead we should pass private pointer to callback in some way.
	const int resLength = strlen(_sr_gui_message_result);
	*result = (char*) SR_GUI_MALLOC(sizeof(char) * (resLength + 1));
	memcpy(*result, strRes, resLength);
	(*result)[resLength] = '\0';

	free(_sr_gui_message_result);
	_sr_gui_message_result = nullptr;

	return SR_GUI_VALIDATED;
	//printf( "%lu", dpiX, dpiY, GetDialogBaseUnits() );

}

int sr_gui_ask_color( unsigned char color[4], int askAlpha ) {

	DWORD colorD = RGB(color[0], color[1], color[2]);
	// Preserve palette of favorite colors between calls.
	static COLORREF acrCustClr[16];
	CHOOSECOLOR colorSettings;
	memset(&colorSettings, 0, sizeof(CHOOSECOLOR));
	colorSettings.lStructSize = sizeof(CHOOSECOLOR);
	colorSettings.hwndOwner = NULL;
	colorSettings.rgbResult = colorD; 
	colorSettings.lpCustColors = (LPDWORD) acrCustClr;
	colorSettings.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;

	BOOL res = ChooseColor(&colorSettings);

	if( res != TRUE ) {
		return SR_GUI_CANCELLED;
	}

	color[0] = GetRValue(colorSettings.rgbResult);
	color[1] = GetGValue(colorSettings.rgbResult);
	color[2] = GetBValue(colorSettings.rgbResult);
	color[3] = 255;

	return SR_GUI_VALIDATED;
}


#endif
