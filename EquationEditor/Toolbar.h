#pragma comment(lib, "comctl32.lib")
#include <Windows.h>
#include <Commctrl.h>
#include "resource.h"

HWND MyCreateToolbar(HWND hWndParent)
{
	HIMAGELIST g_hImageList = NULL;
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 3;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0,
		hWndParent, NULL, ::GetModuleHandle(NULL), NULL);
	if (hWndToolbar == NULL)
		return NULL;



	// Create the image list.
	HIMAGELIST hImageList = ImageList_LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCEW(IDB_BITMAP2), 16, 0, RGB(255, 0, 255));
	ImageList_Add(hImageList, LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3)), NULL);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.

	TBBUTTON tbButtons[numButtons] =
	{
		{ 0, ID_ADD_FRAC, TBSTATE_ENABLED, buttonStyles, { 0 }, 0 },
		{ 1, ID_ADD_DEGR, TBSTATE_ENABLED, buttonStyles, { 0 }, 0 },
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}