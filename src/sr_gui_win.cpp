

#if defined(__cplusplus)

#include "sr_gui.h"
#include "sr_gui_internal.h"
#include <stdio.h>
#include <wchar.h>
#include <windows.h>

// Transfer output ownership to the caller.
wchar_t* _sr_gui_widen_string( const char* str ) {
	const int sizeWide = MultiByteToWideChar( CP_UTF8, 0, str, -1, NULL, 0 );
	wchar_t* output = (wchar_t*) SR_GUI_MALLOC( sizeWide * sizeof( wchar_t ) );
	MultiByteToWideChar( CP_UTF8, 0, str, -1, output, sizeWide );
	return output;
}

void sr_gui_show_message( const char* title, const char* message, int level ) {
	wchar_t* titleW = _sr_gui_widen_string( title );
	wchar_t* messageW = _sr_gui_widen_string( message );
	unsigned long flags = MB_OK;
	if( level == SR_GUI_MESSAGE_LEVEL_ERROR ) {
		flags |= MB_ICONERROR;
	}
	else if( level == SR_GUI_MESSAGE_LEVEL_WARN ) {
		flags |= MB_ICONWARNING;
	}
	else {
		flags |= MB_ICONINFORMATION;
	}
	
	int res = MessageBox(NULL, messageW, titleW, flags);
	(void) res;

	free( titleW );
	free( messageW );
}

void sr_gui_show_notification( const char* title, const char* message ) {
	
}

int sr_gui_ask_directory( const char* title, const char* startDir, char** outPath ) {
	return SR_GUI_CANCELLED;
}

int sr_gui_ask_load_files( const char* title, const char* startDir, const char* exts, char*** outPaths, int* outCount) {
	return SR_GUI_CANCELLED;
}

int sr_gui_ask_save_file( const char* title, const char* startDir, const char* exts, char** outpath ) {
	return SR_GUI_CANCELLED;
}

// Non thread safe, check if SetWindowText et al. are thread safe or not.
static wchar_t* _sr_gui_button0 = nullptr;
static wchar_t* _sr_gui_button1 = nullptr;
static wchar_t* _sr_gui_button2 = nullptr;

static LRESULT __stdcall _sr_gui_change_message_captions( int nCode, WPARAM wParam, LPARAM lParam )
{
	if( nCode == HCBT_ACTIVATE ) {
		SetWindowText( GetDlgItem( (HWND) wParam, IDYES ), _sr_gui_button0 );
		SetWindowText( GetDlgItem( (HWND) wParam, IDNO ), _sr_gui_button1 );
		SetWindowText( GetDlgItem( (HWND) wParam, IDCANCEL ), _sr_gui_button2 );
	}
	return 0;
}

int sr_gui_ask_choice( const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2 ) {
	wchar_t* titleW = _sr_gui_widen_string( title );
	wchar_t* messageW = _sr_gui_widen_string( message );
	_sr_gui_button0 = _sr_gui_widen_string( button0 );
	_sr_gui_button1 = _sr_gui_widen_string( button1 );
	_sr_gui_button2 = _sr_gui_widen_string( button2 );
	unsigned long flags = MB_YESNOCANCEL;

	if( level == SR_GUI_MESSAGE_LEVEL_ERROR ) {
		flags |= MB_ICONERROR;
	}
	else if( level == SR_GUI_MESSAGE_LEVEL_WARN ) {
		flags |= MB_ICONWARNING;
	}
	else {
		flags |= MB_ICONINFORMATION;
	}
	HHOOK hook = SetWindowsHookEx( WH_CBT, _sr_gui_change_message_captions, NULL, GetCurrentThreadId() );
	int res = MessageBox( NULL, messageW, titleW, flags );
	UnhookWindowsHookEx( hook );

	free( titleW );
	free( messageW );
	free( _sr_gui_button0 ); _sr_gui_button0 = nullptr;
	free( _sr_gui_button1 ); _sr_gui_button1 = nullptr;
	free( _sr_gui_button2 ); _sr_gui_button2 = nullptr;

	if( res == IDYES ) {
		return SR_GUI_BUTTON0;
	}
	else if( res == IDNO ) {
		return SR_GUI_BUTTON1;
	}
	return SR_GUI_BUTTON2;
}

#define ID_MESSAGE   150
#define ID_TEXT   200
WCHAR szItemName[80]; // receives name of item to delete. 

BOOL CALLBACK DialogProc( HWND hwndDlg,
							  UINT message,
							  WPARAM wParam,
							  LPARAM lParam )
{
	switch( message )
	{
	case WM_COMMAND:
	switch( LOWORD( wParam ) )
	{
	case IDOK:
	if( !GetDlgItemText( hwndDlg, ID_TEXT, szItemName, 80 ) )
		*szItemName = 0;

	wprintf( L"%s\n", szItemName );
   // Fall through. 

	case IDCANCEL:
	EndDialog( hwndDlg, wParam );
	return TRUE;
	}
	}
	return FALSE;
}

LPWORD lpwAlign( LPWORD lpIn )
{
	ULONG ul;

	ul = (ULONG) lpIn;
	/*ul++;
	ul >>= 1;
	ul <<= 1;*/
	return (LPWORD) ul;
}

LRESULT DisplayMyMessage()
{
	HGLOBAL hgbl;
	LPDLGTEMPLATE lpdt;
	LPDLGITEMTEMPLATE lpdit;
	LPWORD lpw;
	LPWSTR lpwsz;
	LRESULT ret;
	int nchar;

	hgbl = GlobalAlloc( GMEM_ZEROINIT, 4096 );
	if( !hgbl )
		return -1;

	lpdt = (LPDLGTEMPLATE) GlobalLock( hgbl );

	// Define a dialog box.

	lpdt->style = DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION ;
	lpdt->cdit = 4;  // Number of controls
	lpdt->x = 10;  lpdt->y = 10;
	lpdt->cx = 360/2; lpdt->cy = 120/2;

	lpw = (LPWORD) ( lpdt + 1 );
	*lpw++ = 0;             // No menu
	*lpw++ = 0;             // Predefined dialog box class (by default)

	lpwsz = (LPWSTR) lpw;
	nchar = 1 + MultiByteToWideChar( CP_ACP, 0, "My Dialog", -1, lpwsz, 50 );
	lpw += nchar;

	//-----------------------
	// Define an OK button.
	//-----------------------
	//lpw = lpwAlign( lpw );    // Align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE) lpw;
	lpdit->x = 173 / 2; lpdit->y = 88 / 2;
	lpdit->cx = 83 / 2; lpdit->cy = 23 / 2;
	lpdit->id = IDOK;       // OK button identifier
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;

	lpw = (LPWORD) ( lpdit + 1 );
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080;        // Button class

	lpwsz = (LPWSTR) lpw;
	nchar = 1 + MultiByteToWideChar( CP_ACP, 0, "OK", -1, lpwsz, 50 );
	lpw += nchar;
	*lpw++ = 0;             // No creation data

	//-----------------------
	// Define a cancel button.
	//-----------------------
	//lpw = lpwAlign( lpw );    // Align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE) lpw;
	lpdit->x = 263 / 2; lpdit->y = 88 / 2;
	lpdit->cx = 83 / 2; lpdit->cy = 23 / 2;
	lpdit->id = IDCANCEL;  
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;

	lpw = (LPWORD) ( lpdit + 1 );
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080;        // Button class atom

	lpwsz = (LPWSTR) lpw;
	nchar = 1 + MultiByteToWideChar( CP_ACP, 0, "Cancel", -1, lpwsz, 50 );
	lpw += nchar;
	*lpw++ = 0;             // No creation data

	//-----------------------
	// Define a static text control.
	//-----------------------
	//lpw = lpwAlign( lpw );    // Align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE) lpw;
	lpdit->x = 10; lpdit->y = 10;
	lpdit->cx = 100 / 2; lpdit->cy = 20 / 2;
	lpdit->id = ID_MESSAGE;    // Text identifier
	lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

	lpw = (LPWORD) ( lpdit + 1 );
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0082;        // Static class

	WCHAR* lpszMessage = L"Input data.";
	for( lpwsz = (LPWSTR) lpw; *lpwsz++ = (WCHAR) *lpszMessage++;);
	lpw = (LPWORD) lpwsz;
	*lpw++ = 0;             // No creation data

	//-----------------------
	// Define a static text control.
	//-----------------------
	//lpw = lpwAlign( lpw );    // Align DLGITEMTEMPLATE on DWORD boundary
	lpdit = (LPDLGITEMTEMPLATE) lpw;
	lpdit->x = 10; lpdit->y = 30;
	lpdit->cx = 300 / 2; lpdit->cy = 20 / 2;
	lpdit->id = ID_TEXT;    // Text identifier
	lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

	lpw = (LPWORD) ( lpdit + 1 );
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0081;        // Static class

	WCHAR* lpszMessage1 = L"THIS is my mess.";
	for( lpwsz = (LPWSTR) lpw; *lpwsz++ = (WCHAR) *lpszMessage1++;);
	lpw = (LPWORD) lpwsz;
	*lpw++ = 0;             // No creation data

	GlobalUnlock( hgbl );
	ret = DialogBoxIndirect( NULL,
							 (LPDLGTEMPLATE) hgbl,
							 NULL,
							 (DLGPROC) DialogProc );
	GlobalFree( hgbl );


	int dpiX = 0;
	int dpiY = 0;
	HDC hdc = GetDC( NULL );
	if( hdc )
	{
		dpiX = GetDeviceCaps( hdc, LOGPIXELSX );
		dpiY = GetDeviceCaps( hdc, LOGPIXELSY );
		ReleaseDC( NULL, hdc );
	}
	printf( "%d %d %lu", dpiX, dpiY, GetDialogBaseUnits() );
	return ret;
}
int sr_gui_ask_string( const char* title, const char* message, char** result ) {

	LRESULT test = DisplayMyMessage();
	/*const int resLength = strlen( strRes );

	*result = (char*) SR_GUI_MALLOC( sizeof( char ) * ( resLength + 1 ) );
	memcpy( *result, strRes, resLength );
	( *result )[resLength] = '\0';

	[alert release] ;
	[field release] ;*/
	return SR_GUI_CANCELLED;
}

int sr_gui_ask_color( unsigned char color[4], int askAlpha ) {

	DWORD colorD = RGB( color[0], color[1], color[2] );
	// Preserve palette of favorite colors between calls.
	static COLORREF acrCustClr[16];
	CHOOSECOLOR colorSettings;
	memset( &colorSettings, 0, sizeof( CHOOSECOLOR ) );
	colorSettings.lStructSize = sizeof( CHOOSECOLOR );
	colorSettings.hwndOwner = NULL;
	colorSettings.rgbResult = colorD; 
	colorSettings.lpCustColors = (LPDWORD) acrCustClr;
	colorSettings.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;

	BOOL res = ChooseColor( &colorSettings );

	if( res != TRUE ) {
		return SR_GUI_CANCELLED;
	}

	color[0] = GetRValue( colorSettings.rgbResult );
	color[1] = GetGValue( colorSettings.rgbResult );
	color[2] = GetBValue( colorSettings.rgbResult );
	color[3] = 255;

	return SR_GUI_VALIDATED;
}


#endif
