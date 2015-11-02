﻿#include <Windows.h>
#include "RibbonFramework.h"
#include "View/EquationEditorWindow.h"
#include "resource.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	HRESULT hr = CoInitialize(NULL);
    CEquationEditorWindow::RegisterClass();
    CEquationEditorWindow wnd;
    wnd.Create();
    wnd.Show(nCmdShow);

	HACCEL table = ::LoadAccelerators( hInstance, MAKEINTRESOURCE( IDR_ACCELERATOR1 ) );
    MSG msg;
    while (::GetMessage(&msg, (HWND)nullptr, 0, 0) ) {
		if( !::TranslateAccelerator( wnd.GetHandle(), table, &msg ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
    }

    return 0;
}