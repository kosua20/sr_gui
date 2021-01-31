

#if defined(__cplusplus)

#include "sr_gui.h"
#include "sr_gui_internal.h"
#include <stdio.h>
#include <cstdint>
#include <wchar.h>
#include <windows.h>
#include <shobjidl.h>

// Transfer output ownership to the caller.
wchar_t* _sr_gui_widen_string(const char* str) {
	const int sizeWide = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	wchar_t* output = (wchar_t*) SR_GUI_MALLOC(sizeWide  * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, output, sizeWide);
	return output;
}

// Transfer output ownership to the caller.
char* _sr_gui_narrow_string(const wchar_t* wstr) {
	const int sizeNarrow = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
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

	SR_GUI_FREE(titleW);
	SR_GUI_FREE(messageW);
}

void sr_gui_show_notification(const char* title, const char* message) {
	
}

bool _sr_gui_init_COM(){
	HRESULT res = CoInitializeEx(NULL, ::COINIT_APARTMENTTHREADED | ::COINIT_DISABLE_OLE1DDE);
	return (res == RPC_E_CHANGED_MODE || SUCCEEDED(res));
}

// deinit will be CoUninitialize()

bool _sr_gui_add_filter_extensions(const char* exts, IFileDialog* dialog){
	if(!exts){
		return true;
	}
	int extsLen = strlen(exts);
	if(extsLen == 0){
		return true;
	}
	// Count extensions.
	int count = 1;
	for(size_t cid = 0; cid < extsLen; ++cid){
		if(exts[cid] == ','){
			++count;
		}
	}
	// Add wildcard at the end.
	++count;

	COMDLG_FILTERSPEC* filterList = (COMDLG_FILTERSPEC*) SR_GUI_MALLOC(count * sizeof(COMDLG_FILTERSPEC));
	if(filterList == nullptr){
		return false;
	}
	char extBuffer[SR_GUI_MAX_STR_SIZE];
	extBuffer[0] = '*';
	extBuffer[1] = '.';

	size_t cid = 0;

	for(size_t fid = 0; fid < count-1; ++fid){
		// After the wildcard.
		size_t bid = 2;
		while(cid < extsLen && exts[cid] != ','){
			extBuffer[bid] = exts[cid];
			++bid;
			++cid;
		}
		extBuffer[bid] = '\0';
		// ...
		specList[specIdx].pszName = _sr_gui_widen_string(&extBuffer[2]);
		specList[specIdx].pszSpec = _sr_gui_widen_string(extBuffer);
		// Skip comma.
		++cid;
		if(cid >= extsLen){
			break;
		}
	}
	filterList[count-1].pszSpec = L"*.*";
	filterList[count-1].pszName = L"*.*";
	dialog->SetFileTypes( count, filterList );

	for(size_t fid = 0; fid < count-1; ++fid){
		SR_GUI_FREE(filterList[fid].pszSpec);
		SR_GUI_FREE(filterList[fid].pszName);
	}
	SR_GUI_FREE(specList);
}

bool _sr_gui_add_default_path(const char* path, IFileDialog* dialog){
	if(!path){
		return true;
	}
	if(strlen(path) == 0){
		return true;
	}
	WCHAR* startDirW = _sr_gui_widen_string(path);
	IShellItem *pathShell;
	HRESULT res = SHCreateItemFromParsingName(startDirW, NULL, IID_PPV_ARGS(&pathShell));
	SR_GUI_FREE(startDirW);

	if(!SUCCEEDED(res) && (res != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) && (result != HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE))){
		return false;
	}
	dialog->SetFolder( pathShell);
	pathShell->Release();
	return true;
}

int sr_gui_ask_directory(const char* title, const char* startDir, char** outPath) {
	if(!_sr_gui_init_COM()){
		return SR_GUI_CANCELLED;
	}
	IFileDialog *dialog = NULL;
	HRESULT res = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}
	// Set default path.
	bool success = _sr_gui_add_default_path(startDir, dialog);
	if(!success){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Set title.
	WCHAR* titleW = _sr_gui_widen_string(title);
	res = dialog->SetTitle(titleW);
	SR_GUI_FREE(titleW);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Options: pick only directories.
	DWORD options = 0;
	res = dialog->GetOptions(&options);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}
	res = dialog->SetOptions(options | FOS_PICKFOLDERS);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Present
	res = dialog->Show(nullptr);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	IShellItem* selectedItem = nullptr;
	res = fileDialog->GetResult(&selectedItem);
	dialog->Release();
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}

	WCHAR* selectedPath = nullptr;
	res = selectedItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &selectedPath);
	selectedItem->Release();
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}
	*outPath = _sr_gui_narrow_string(selectedPath);
	CoTaskMemFree(selectedPath);
	return SR_GUI_VALIDATED;
}

int sr_gui_ask_load_files(const char* title, const char* startDir, const char* exts, char*** outPaths, int* outCount) {
	*outCount = 0;
	*outPaths = NULL;

	if(!_sr_gui_init_COM()){
		return SR_GUI_CANCELLED;
	}
	IFileDialog *dialog = NULL;
	HRESULT res = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}
	// Set default path.
	bool success = _sr_gui_add_default_path(startDir, dialog);
	if(!success){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Set extension filter.
	success = _sr_gui_add_filter_extensions(exts, dialog);
	if(!success){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Set title.
	WCHAR* titleW = _sr_gui_widen_string(title);
	res = dialog->SetTitle(titleW);
	SR_GUI_FREE(titleW);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}
	// Options: multiple selection allowed.
	DWORD options = 0;
	res = dialog->GetOptions(&options);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}
	res = dialog->SetOptions(options | FOS_ALLOWMULTISELECT);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Present
	res = dialog->Show(nullptr);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}
	IShellItemArray* selectedItems = nullptr;
	res = fileDialog->GetResults(&selectedItems);
	dialog->Release();
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}

	DWORD itemsCount;
	res = selectedItems->GetCount(&itemsCount);
	*outCount = 0;
	if(*itemsCount > 0){
		*outPaths = (char**)SR_GUI_MALLOC(sizeof(char*) * itemsCount);
		for(int i = 0; i < itemsCount; ++i){
			IShellItem *selectedItem = nullptr;
			selectedItems->GetItemAt(iid, &selectedItems);
			// Skip non-file elements.
			SFGAOF attributes;
			res = selectedItem->GetAttributes( SFGAO_FILESYSTEM, &attributes );
			if(!(attribs & SFGAO_FILESYSTEM)){
				continue;
			}
			(*outCount)++;
			WCHAR* selectedPath = nullptr;
			res = selectedItem->GetDisplayName(SIGDN_FILESYSPATH, &selectedPath);
			*outPaths[i] = _sr_gui_narrow_string(selectedPath);
			CoTaskMemFree(selectedPath);
		}
	}
	selectedItems->Release();

	return SR_GUI_VALIDATED;
}

int sr_gui_ask_save_file(const char* title, const char* startDir, const char* exts, char** outpath) {
	if(!_sr_gui_init_COM()){
		return SR_GUI_CANCELLED;
	}
	IFileDialog *dialog = NULL;
	HRESULT res = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}
	// Set default path.
	bool success = _sr_gui_add_default_path(startDir, dialog);
	if(!success){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Set extension filter.
	success = _sr_gui_add_filter_extensions(exts, dialog);
	if(!success){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Set title.
	WCHAR* titleW = _sr_gui_widen_string(title);
	res = dialog->SetTitle(titleW);
	SR_GUI_FREE(titleW);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	// Present
	res = dialog->Show(nullptr);
	if(!SUCCEEDED(res)){
		dialog->Release();
		return SR_GUI_CANCELLED;
	}

	IShellItem* selectedItem = nullptr;
	res = fileDialog->GetResult(&selectedItem);
	dialog->Release();
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}

	WCHAR* selectedPath = nullptr;
	res = selectedItem->GetDisplayName(SIGDN_FILESYSPATH, &selectedPath);
	selectedItem->Release();
	if(!SUCCEEDED(res)){
		return SR_GUI_CANCELLED;
	}
	*outPath = _sr_gui_narrow_string(selectedPath);
	CoTaskMemFree(selectedPath);
	return SR_GUI_VALIDATED;
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
	SR_GUI_FREE(titleW);
	SR_GUI_FREE(messageW);
	SR_GUI_FREE(_sr_gui_button0); _sr_gui_button0 = nullptr;
	SR_GUI_FREE(_sr_gui_button1); _sr_gui_button1 = nullptr;
	SR_GUI_FREE(_sr_gui_button2); _sr_gui_button2 = nullptr;

	if(res == IDYES) {
		return SR_GUI_BUTTON0;
	} else if(res == IDNO) {
		return SR_GUI_BUTTON1;
	}
	return SR_GUI_BUTTON2;
}

// Not thread safe, might be passed as data somewhere.
char* _sr_gui_message_result = nullptr;

#define _SR_GUI_ID_MESSAGE   150
#define _SR_GUI_ID_TEXT   200

BOOL CALLBACK _sr_gui_message_callback(HWND hwndDlg, UINT message, WPARAM wParam,  LPARAM lParam){
	switch(message)
	{
		/*case WM_CTLCOLOR:
			break;*/
		case WM_COMMAND:
			switch( LOWORD( wParam ) )
			{
				case IDOK:
				{
					if( _sr_gui_message_result != nullptr ) {
						SR_GUI_FREE( _sr_gui_message_result );
					}

					wchar_t stringRes[SR_GUI_MAX_STR_SIZE];
					if( GetDlgItemText( hwndDlg, _SR_GUI_ID_TEXT, stringRes, SR_GUI_MAX_STR_SIZE ) ){
						_sr_gui_message_result = _sr_gui_narrow_string(stringRes);

					} else {
						_sr_gui_message_result = (char*)SR_GUI_MALLOC( 1 * sizeof( char ) );
						if( _sr_gui_message_result == nullptr ) {
							EndDialog( hwndDlg, SR_GUI_CANCELLED );
							return TRUE;
						}
						_sr_gui_message_result[0] = '\0';
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

uintptr_t _sr_gui_align_ptr_dword(uintptr_t ptr)
{
	uintptr_t rem = ptr % sizeof(DWORD);
	return rem == 0 ? ptr : (ptr + sizeof(DWORD) - rem);
}

uintptr_t _sr_gui_align_ptr_word(uintptr_t ptr)
{
	uintptr_t rem = ptr % sizeof(WORD);
	return rem == 0 ? ptr : (ptr + sizeof(WORD) - rem);
}

uintptr_t _sr_gui_insert_string( uintptr_t head, const WCHAR* str ) {
	// Strings have to be WORD aligned.
	head = _sr_gui_align_ptr_word( head );
	// Copy string in place.
	const size_t strSizeByte = ( wcslen( str ) + 1 ) * sizeof( WCHAR );
	WCHAR* strBegin = (WCHAR*) ( head );
	memcpy( strBegin, str, strSizeByte );
	// Move after the string.
	return head + strSizeByte;
}

#pragma pack(push, 1)
typedef struct {
	WORD      dlgVer;
	WORD      signature;
	DWORD     helpID;
	DWORD     exStyle;
	DWORD     style;
	WORD      cDlgItems;
	short     x;
	short     y;
	short     cx;
	short     cy;
	WORD	menu;
	WORD	windowClass;
	// Here goes the title of variable length.
	// WCHAR	title[titleLen]
	// Then the font info (see below)
	// _sr_gui_dialog_template_tail fontInfos.
} _sr_gui_dialog_template_head;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	WORD      pointsize;
	WORD      weight;
	BYTE      italic;
	BYTE      charset;
	// Here goes the typename of variable length. 
	//WCHAR     typeface[typefaceLen];
} _sr_gui_dialog_template_tail;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	DWORD     helpID;
	DWORD     exStyle;
	DWORD     style;
	short     x;
	short     y;
	short     cx;
	short     cy;
	DWORD     id;
	WORD windowClass[2];
	// Here goes the name of the button of variable length. 
	// WCHAR title[titleLength];
	// Then the end of the struct.
	// _sr_gui_control_template_tail end;
} _sr_gui_control_template_head;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	WORD	extraCount;
} _sr_gui_control_template_tail;
#pragma pack(pop)


struct _sr_gui_control_infos
{
	const WCHAR* txt;
	DWORD id;
	DWORD style;
	WORD type;
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
};

void _sr_gui_get_font_scaling(const WCHAR* name, int size, int* w, int* h ) {
	static const WCHAR letters[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q', 'r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H', 'I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z', 0 };
	// Default values.
	*w = 4; *h = 8;

	HDC context = GetDC( 0 );
	const int fontHeight = -MulDiv( size, GetDeviceCaps( context, LOGPIXELSY ), 72 );
	HFONT font = CreateFont( fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY, FF_DONTCARE, name );
	if( font == nullptr ) {
		return;
	}
	SelectObject( context, font );
	
	SIZE outSize;
	if( GetTextExtentPointW( context, letters, 52, &outSize ) ) {
		*h = outSize.cy;
		*w = ( outSize.cx / 26 + 1 ) / 2;
	}
	DeleteObject(font);
}


int sr_gui_ask_string( const char* title, const char* message, char** result ) {

#define _SR_GUI_CONTROL_COUNT 4

	// There is no easy way to have such a prompt on Windows it seems.

	// Initialize all strings.
	WCHAR* titleW = _sr_gui_widen_string( title );
	WCHAR* messageW = _sr_gui_widen_string( message );
	const WCHAR* fontW = L"MS Shell Dlg 2";
	const WCHAR* okW = L"OK";
	const WCHAR* cancelW = L"Cancel";
	const WCHAR* defaultW = L"Default value";

	// Size of each struct in memory (dialog + n * control).
	size_t maxSize = sizeof( _sr_gui_dialog_template_head ) + sizeof( _sr_gui_dialog_template_tail );
	maxSize += _SR_GUI_CONTROL_COUNT * ( sizeof( _sr_gui_control_template_head ) + sizeof( _sr_gui_control_template_tail ) );
	// Padding for the strings (at most).
	maxSize += sizeof( WORD ) * ( 2 + _SR_GUI_CONTROL_COUNT * 1 );
	// Padding for the control structs (at most).
	maxSize += sizeof( DWORD ) * ( _SR_GUI_CONTROL_COUNT );
	// Size of all strings.
	maxSize += (wcslen( titleW ) + wcslen(messageW) + wcslen(okW) + wcslen( fontW ) + wcslen( cancelW ) + wcslen( defaultW )) * sizeof(WCHAR);
	// Extra padding.
	maxSize += 1024;

	HGLOBAL hgbl = GlobalAlloc( GMEM_ZEROINIT, maxSize );
	if( !hgbl ) {
		SR_GUI_FREE( messageW );
		SR_GUI_FREE( titleW );
		return SR_GUI_CANCELLED;
	}

	// Retrieve pixel scaling.
	int w, h;
	_sr_gui_get_font_scaling( fontW, 8, &w, &h );

	// Define a dialog box.
	_sr_gui_dialog_template_head* dialog = (_sr_gui_dialog_template_head*) GlobalLock( hgbl );
	if( dialog == nullptr ) {
		SR_GUI_FREE( messageW );
		SR_GUI_FREE( titleW );
		return SR_GUI_CANCELLED;
	}

	dialog->dlgVer = 1;
	dialog->signature = 0xFFFF;
	dialog->style = DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION | DS_SHELLFONT;
	dialog->cDlgItems = 4;  // Number of controls
	dialog->x = 0;  dialog->y = 0;
	dialog->cx = MulDiv( 360, 4, w ); dialog->cy = MulDiv( 110, 8, h );
	dialog->menu = 0;
	dialog->windowClass = 0;
	// Oh boy. Move after the window class. 
	uintptr_t head = (uintptr_t) ( &( dialog->windowClass ) );
	head += sizeof( WORD );
	// Insert the string afterward, properly aligned.
	head = _sr_gui_insert_string( head, titleW );

	_sr_gui_dialog_template_tail* dialogEnd = (_sr_gui_dialog_template_tail*) ( head );
	dialogEnd->pointsize = 8;
	dialogEnd->weight = FW_NORMAL;
	dialogEnd->italic = FALSE;
	dialogEnd->charset = DEFAULT_CHARSET;
	// Move after the charset.
	head = (uintptr_t) ( &( dialogEnd->charset ) );
	head += sizeof( BYTE );
	// Insert the font fixed name afterward, properly aligned.
	head = _sr_gui_insert_string( head, fontW );

	// Move to the controls. 
	_sr_gui_control_infos controls[4];
	controls[0] = { okW, IDOK, BS_DEFPUSHBUTTON | WS_TABSTOP, 0x0080,		170, 81, 83, 23 };
	controls[1] = { cancelW, IDCANCEL, BS_PUSHBUTTON | WS_TABSTOP, 0x0080,	262, 81, 83, 23};
	controls[2] = { messageW, _SR_GUI_ID_MESSAGE, 0, 0x0082,				32, 17, 300, 30 };
	controls[3] = { defaultW, _SR_GUI_ID_TEXT, WS_TABSTOP, 0x0081,			32, 47, 300, 15 };

	for(size_t cid = 0; cid < 4; ++cid ){
		// Each should be aligned on a dword.
		const _sr_gui_control_infos& cinf = controls[cid];
		head = _sr_gui_align_ptr_dword( head );
		_sr_gui_control_template_head* control = (_sr_gui_control_template_head*) head;
		control->x = MulDiv( cinf.x, 4, w );
		control->y = MulDiv( cinf.y, 8, h );
		control->cx = MulDiv( cinf.w, 4, w );
		control->cy = MulDiv( cinf.h, 8, h );
		control->id = cinf.id; // Standard ID.
		control->style = WS_CHILD | WS_VISIBLE | cinf.style;
		control->windowClass[0] = 0xFFFF;
		control->windowClass[1] = cinf.type; // Button class
		// Move after the windowClass.
		head = (uintptr_t) ( control->windowClass );
		head += 2 * sizeof( WORD );
		head = _sr_gui_insert_string( head, cinf.txt );
		// Then the tail.
		_sr_gui_control_template_tail* controlEnd = (_sr_gui_control_template_tail*) ( head );
		controlEnd->extraCount = 0;
		head += sizeof( WORD );
	}

	GlobalUnlock(hgbl);
	LRESULT ret = DialogBoxIndirect(NULL, (LPDLGTEMPLATE) hgbl, NULL, (DLGPROC) _sr_gui_message_callback);
	GlobalFree(hgbl);
	SR_GUI_FREE(titleW);
	SR_GUI_FREE(messageW);

	if(ret != SR_GUI_VALIDATED || (_sr_gui_message_result == nullptr)){
		return SR_GUI_CANCELLED;
	}

	// Copy result back via static variable.
	// Instead we should pass private pointer to callback in some way.
	const size_t resLength = strlen(_sr_gui_message_result);
	*result = (char*) SR_GUI_MALLOC(sizeof(char) * (resLength + 1));
	if( result == nullptr ) {
		return SR_GUI_CANCELLED;
	}
	memcpy(*result, _sr_gui_message_result, resLength);
	(*result)[resLength] = '\0';

	SR_GUI_FREE(_sr_gui_message_result);
	_sr_gui_message_result = nullptr;
	return SR_GUI_VALIDATED;
}

int sr_gui_ask_color( unsigned char color[3] ) {

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

	return SR_GUI_VALIDATED;
}


#endif
