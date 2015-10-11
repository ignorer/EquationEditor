﻿#include <Windowsx.h>
#include "resource.h"
#include "View/EquationEditorWindow.h"

const wchar_t* const CEquationEditorWindow::className = L"EquationEditorWindow";

CEquationEditorWindow::CEquationEditorWindow() : hwnd( nullptr ) 
{
	presenter = std::make_shared<CEquationPresenter>(*this);
}

bool CEquationEditorWindow::RegisterClassW() 
{
	WNDCLASSEX wnd;
	ZeroMemory( &wnd, sizeof(wnd) );
	wnd.cbSize = sizeof(wnd);
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.lpfnWndProc = equationEditorWindowProc;
	wnd.hInstance = ::GetModuleHandle( nullptr );
	wnd.lpszClassName = className;
	wnd.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wnd.hCursor = ::LoadCursor( nullptr, IDC_ARROW );
	return ::RegisterClassEx( &wnd ) != 0;
}

bool CEquationEditorWindow::Create() 
{
	symbolSelectedColorref = RGB( 0xFF, 0xFF, 0xFF );	// Белый
	symbolUnselectedColorref = RGB( 0, 0, 0 );			// Черный
	bkSelectedColorref = RGB( 0x1F, 0xAE, 0xE9 );		// Голубой
	bkUnselectedColorref = RGB( 0xFF, 0xFF, 0xFF );		// Белый

	return ::CreateWindowEx( 0, className, L"Equation Editor", WS_OVERLAPPEDWINDOW | WS_EX_LAYERED, 0, 0, 500, 400,
		nullptr, nullptr, ::GetModuleHandle( nullptr ), this ) != 0;
}

void CEquationEditorWindow::Show( int cmdShow ) 
{
	::ShowWindow( hwnd, cmdShow );
}

void CEquationEditorWindow::OnDestroy() 
{
	for( auto it = fonts.begin(); it != fonts.end(); ++it ) {
		::DeleteObject( it->second );
	}
	::PostQuitMessage( 0 );
}

void CEquationEditorWindow::OnCreate() 
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(::GetWindowLong( hwnd, GWL_HINSTANCE ));
	
	// Добавляем меню
	HMENU hMenu = ::LoadMenu( hInstance, MAKEINTRESOURCE( IDR_MENU1 ) );
	::SetMenu( hwnd, hMenu );
	//добавляем контекстное меню
	hMenuPopup = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU2));
	hMenuPopup = GetSubMenu(hMenuPopup, 0);
}

void CEquationEditorWindow::OnSize( int cxSize, int cySize ) 
{
}

void CEquationEditorWindow::Redraw() 
{
	::InvalidateRect( hwnd, nullptr, TRUE );
}

int CEquationEditorWindow::GetSymbolWidth( wchar_t symbol, int symbolHeight ) 
{
	HDC hdc = GetDC( hwnd );
	HGDIOBJ oldObject = ::SelectObject( hdc, getFont(symbolHeight) );
	
	int symbolWidth = 0;
	::GetCharWidth32( hdc, symbol, symbol, &symbolWidth );
	::SelectObject( hdc, oldObject );
	::ReleaseDC( hwnd, hdc );
	return symbolWidth;
}

void CEquationEditorWindow::OnLButtonDown( int xMousePos, int yMousePos ) 
{
	presenter->SetCaret( xMousePos, yMousePos );
}

void CEquationEditorWindow::OnRButtonDown(int xMousePos, int yMousePos)
{
	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	::TrackPopupMenu(hMenuPopup, TPM_RIGHTBUTTON, windowRect.left + xMousePos, windowRect.top + yMousePos, 0, hwnd, NULL);
}

void CEquationEditorWindow::OnWmCommand( WPARAM wParam, LPARAM lParam ) 
{
	if( HIWORD( wParam ) == 0 ) {
		switch( LOWORD( wParam ) ) {
		case ID_ADD_FRAC:
			presenter->AddControlView( FRAC );
			break;
		case ID_ADD_DEGR:
			presenter->AddControlView( DEGR );
			break;
		case ID_ADD_SUBSCRIPT:
			presenter->AddControlView( SUBSCRIPT );
			break;
		case ID_ADD_RADICAL:
			presenter->AddControlView( RADICAL );
			break;

		case ID_CUT:
			//if (EditCopy())
			presenter->DeleteSymbol();
			break;

		case ID_COPY:
			EditCopy();
			break;

		case ID_PASTE:
			EditPaste();
			break;

		case ID_DELETE:
			presenter->DeleteSymbol();
			break;
		}
	}
}

void CEquationEditorWindow::OnKeyDown( WPARAM wParam ) 
{
	switch( wParam ) {
	case VK_LEFT:   // LEFT ARROW 
		presenter->MoveCaretLeft();
		break;

	case VK_RIGHT:  // RIGHT ARROW
		presenter->MoveCaretRight();
		break;

	case VK_UP:     // UP ARROW 
	case VK_DOWN:   // DOWN ARROW 
		break;
	}
}

void CEquationEditorWindow::OnMouseMove( WPARAM wParam, int x, int y ) 
{
	if( wParam == MK_LBUTTON ) {
		presenter->SetSelection( x, y );
	}
}

void CEquationEditorWindow::OnChar( WPARAM wParam ) 
{
	switch( wParam ) {
	case 0x08:  // backspace
		presenter->DeleteSymbol();
		return;
	case 0x0A:  // linefeed 
	case 0x0D:  // enter
	case 0x1B:  // escape 
		MessageBeep( (UINT) -1 );
		return;

	case 0x09:  // tab 
		// Convert tabs to four consecutive spaces. 
		for( int i = 0; i < 4; ++i ) {
			::SendMessage( hwnd, WM_CHAR, 0x20, 0 );
		}
		return;

	default:    // displayable character
		presenter->InsertSymbol( (wchar_t) wParam );
		break;
	}
}

HFONT CEquationEditorWindow::getFont( int height ) 
{
	if( fonts[height] == 0 ) {
		fonts[height] = ::CreateFont( height, 0, 0, 0, 300, /*TRUE*/ false, false, false, DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS, CLIP_STROKE_PRECIS, CLEARTYPE_QUALITY, /*FF_ROMAN*/ FF_SCRIPT, (LPCWSTR) /*"Cambria Math"*/ "Script" );
		// Cambria Math
	}
	return fonts[height];
}

void CEquationEditorWindow::DrawString( const std::wstring& text, const CRect& textRect, bool isSelected ) 
{
	RECT rect;
	rect.bottom = textRect.Bottom( );
	rect.top = textRect.Top( );
	rect.left = textRect.Left( );
	rect.right = textRect.Right( );
	HFONT font = getFont( textRect.GetHeight() );
	HGDIOBJ oldObject = ::SelectObject( hdc, font );
	
	if( isSelected ) {
		::SetTextColor( hdc, symbolSelectedColorref );
		::SetBkColor( hdc, bkSelectedColorref );
	} else {
		::SetTextColor( hdc, symbolUnselectedColorref );
		::SetBkColor( hdc, bkUnselectedColorref );
	}
	::DrawText( hdc, text.c_str(), text.size(), &rect, DT_LEFT );
	::SelectObject( hdc, oldObject );
}

void CEquationEditorWindow::DrawPolygon( const std::list<CLine>& polygon, bool isSelected ) 
{
	HGDIOBJ oldPen = ::SelectObject( hdc, ::GetStockObject( DC_PEN ) );
	if( isSelected ) {
		::SetDCPenColor( hdc, symbolSelectedColorref );
	} else {
		::SetDCPenColor( hdc, symbolUnselectedColorref );
	}
	if( !polygon.empty() ) {
		for( CLine line : polygon ) {
			::MoveToEx( hdc, line.StartX(), line.StartY(), nullptr );
			::LineTo( hdc, line.EndX(), line.EndY() );
		}
	}
	::SelectObject( hdc, oldPen );
}

void CEquationEditorWindow::DrawSelectedRect( const CRect& selectedRect ) 
{
	RECT rect;
	rect.bottom = selectedRect.Bottom();
	rect.top = selectedRect.Top();
	rect.left = selectedRect.Left();
	rect.right = selectedRect.Right();

	HBRUSH selectedHBrush = ::CreateSolidBrush( bkSelectedColorref );
	::FillRect( hdc, &rect, selectedHBrush );
	::DeleteObject( selectedHBrush );
}

void CEquationEditorWindow::DrawHighlightedRect( const CRect& controlRect, bool isSelected ) 
{
	HBRUSH highlightedHBrush = ::CreateSolidBrush( RGB( 0xF0, 0xF0, 0xF0 ) );
	HBRUSH oldBrush = static_cast<HBRUSH>( ::SelectObject( hdc, highlightedHBrush ) );
	::Rectangle( hdc, controlRect.Left(), controlRect.Bottom(), controlRect.Right(), controlRect.Top() );
	::SelectObject( hdc, oldBrush );
	::DeleteObject( highlightedHBrush );
}

void CEquationEditorWindow::SetCaret( int caretPointX, int caretPointY, int height ) 
{
	::DestroyCaret();
	::CreateCaret( hwnd, (HBITMAP) nullptr, -1, height );
	::SetCaretPos( caretPointX, caretPointY );
	::ShowCaret( hwnd );
}

#ifndef POPUPMENU
	#define BOX_ELLIPSE  0
	#define BOX_RECT     1

	#define CCH_MAXLABEL 80
	#define CX_MARGIN    12

	typedef struct tagLABELBOX {
		RECT rcText;    // координаты прямоугольника с текстом
		BOOL fSelected; // TRUE если выделен label
		BOOL fEdit;     // TRUE если текст выделен
		int nType;      // прямоугольное или овальное
		int ichCaret;   // позиция каретки
		int ichSel;     // with ichCaret, delimits selection
		int nXCaret;    // window position corresponding to ichCaret
		int nXSel;      // window position corresponding to ichSel
		int cchLabel;   // длина текста в atchLabel
		TCHAR atchLabel[CCH_MAXLABEL];
	} LABELBOX, *PLABELBOX;

	#define POPUPMENU
#endif

void CEquationEditorWindow::InitMenu(HMENU hmenu)
{
	int  cMenuItems = GetMenuItemCount(hmenu);
	int  nPos;
	UINT id;
	UINT fuFlags;
	
	PLABELBOX pbox = (hwnd == NULL) ? NULL :
		(PLABELBOX)GetWindowLong(hwnd, 0);

	//делаем пункты меню активными и неактивными (черные и серые)
	for (nPos = 0; nPos < cMenuItems; nPos++)
	{
		id = GetMenuItemID(hmenu, nPos);

		switch (id)
		{
		case ID_CUT:
		case ID_COPY:
		case ID_DELETE:
			if (pbox == NULL || !pbox->fSelected)
				fuFlags = MF_BYCOMMAND | MF_GRAYED;
			else if (pbox->fEdit)
				fuFlags = (id != ID_DELETE && pbox->ichSel
				== pbox->ichCaret) ?
				MF_BYCOMMAND | MF_GRAYED :
				MF_BYCOMMAND | MF_ENABLED;
			else
				fuFlags = MF_BYCOMMAND | MF_ENABLED;

			EnableMenuItem(hmenu, id, fuFlags);
			break;

		case ID_PASTE:
			if (pbox != NULL && pbox->fEdit)
				EnableMenuItem(hmenu, id,
				IsClipboardFormatAvailable(CF_TEXT) ?
				MF_BYCOMMAND | MF_ENABLED :
				MF_BYCOMMAND | MF_GRAYED
				);
			/*else
				EnableMenuItem(hmenu, id,
				IsClipboardFormatAvailable(uLabelFormat) ? 
				MF_BYCOMMAND | MF_ENABLED :
				MF_BYCOMMAND | MF_GRAYED
				);*/

		}
	}
}


VOID CEquationEditorWindow::EditPaste( VOID )
{
	PLABELBOX pbox;
	HGLOBAL hglb;
	LPTSTR lptstr;
	PLABELBOX pboxCopy;
	int cx, cy;
	//HWND hwnd;

	pbox = hwnd == NULL ? NULL :
		(PLABELBOX)GetWindowLong(hwnd, 0);

	// Если приложение в режиме редактирования,
	// получите текст буфера обмена

	if (pbox != NULL && pbox->fEdit)
	{
		if (!IsClipboardFormatAvailable(CF_TEXT))
			return;
		if (!OpenClipboard(hwnd))
			return;

		hglb = GetClipboardData(CF_TEXT);
		if (hglb != NULL)
		{
			lptstr = (LPTSTR) GlobalLock(hglb);
			if (lptstr != NULL)
			{
				// Обращаемся к определяемой прикладной программой функции
				// ReplaceSelection, чтобы вставить текст и перерисовать окно.

				//ReplaceSelection(hwnd, pbox, lptstr); -> РЕАЛИЗОВАТЬ
				GlobalUnlock(hglb);
			}
		}
		CloseClipboard();
		return;
	}

	// Если приложение не в режиме редактирования,
	// создаем метку окна.
	/*
	if (!IsClipboardFormatAvailable(uLabelFormat))
		return;
	if (!OpenClipboard(hwndMain))
		return;

	hglb = GetClipboardData(uLabelFormat);
	if (hglb != NULL)
	{
		pboxCopy = GlobalLock(hglb);
		if (pboxCopy != NULL)
		{
			cx = pboxCopy->rcText.right + CX_MARGIN;
			cy = pboxCopy->rcText.top * 2 + cyText;
			hwnd = CreateWindowEx(
				WS_EX_NOPARENTNOTIFY | WS_EX_TRANSPARENT,
				atchClassChild, NULL, WS_CHILD, 0, 0, cx, cy,
				hwndMain, NULL, hinst, NULL);

			if (hwnd != NULL)
			{
				pbox = (PLABELBOX)GetWindowLong(hwnd, 0);
				memcpy(pbox, pboxCopy, sizeof(LABELBOX));
				ShowWindow(hwnd, SW_SHOWNORMAL);
				SetFocus(hwnd);
			}
			GlobalUnlock(hglb);
		}
	}
	*/
	CloseClipboard();
}

BOOL CEquationEditorWindow::EditCopy(VOID)
{
	PLABELBOX pbox;
	LPTSTR  lptstrCopy;
	HGLOBAL hglbCopy;
	int ich1, ich2, cch;

	if (hwnd == NULL)
		return FALSE;

	// Открываем буфер обмена и очищаем его.

	if (!OpenClipboard(hwnd))
		return FALSE;
	EmptyClipboard();

	// Получаем указатель на структуру LABELBOX.

	pbox = (PLABELBOX)GetWindowLong(hwnd, 0);

	// Если текст выделен, то копируем его используя формат CF_TEXT.

	if (pbox->fEdit)
	{
		if (pbox->ichSel == pbox->ichCaret)     // нулевая длина
		{
			CloseClipboard();                   // выделение
			return FALSE;
		}

		if (pbox->ichSel < pbox->ichCaret)
		{
			ich1 = pbox->ichSel;
			ich2 = pbox->ichCaret;
		}
		else
		{
			ich1 = pbox->ichCaret;
			ich2 = pbox->ichSel;
		}
		cch = ich2 - ich1;

		// Выделяем память для текста.

		hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
			(cch + 1) * sizeof(TCHAR));
		if (hglbCopy == NULL)
		{
			CloseClipboard();
			return FALSE;
		}

		// Блокируем хэндл и копируем текст в буфер.

		lptstrCopy = ( LPTSTR ) GlobalLock(hglbCopy);
		memcpy(lptstrCopy, &pbox->atchLabel[ich1],
			cch * sizeof(TCHAR));
		lptstrCopy[cch] = (TCHAR)0;    // нулевой символ
		GlobalUnlock(hglbCopy);

		// Помещаем хэндл в буфер обмена.

		SetClipboardData(CF_TEXT, hglbCopy);
	}

	// Если текст не выделен, то копируем весь label.

	else
	{
		// Сохраняем копию выделенного лабела в локальной памяти.
		// С ней мы будем работать и освобождать в ответ на
		// сообщение WM_DESTROYCLIPBOARD.

		PLABELBOX pboxLocalClip = (PLABELBOX)LocalAlloc(
			LMEM_FIXED,
			sizeof(LABELBOX)
			);
		if (pboxLocalClip == NULL)
		{
			CloseClipboard();
			return FALSE;
		}
		memcpy(pboxLocalClip, pbox, sizeof(LABELBOX));
		pboxLocalClip->fSelected = FALSE;
		pboxLocalClip->fEdit = FALSE;

		// Помещаем в буфер обмена данные трёх форматов.

		//SetClipboardData(uLabelFormat, NULL);
		SetClipboardData(CF_OWNERDISPLAY, NULL);
		SetClipboardData(CF_TEXT, NULL);
	}

	// Закрываем буфер обмена.

	CloseClipboard();

	return TRUE;
}

void CEquationEditorWindow::OnDraw() 
{
	PAINTSTRUCT ps;
	HDC curhdc = ::BeginPaint( hwnd, &ps );

	RECT rect;
	::GetClientRect( hwnd, &rect );
	hdc = ::CreateCompatibleDC( curhdc );
	HBITMAP backbuffer = ::CreateCompatibleBitmap( curhdc, rect.right - rect.left, rect.bottom - rect.top );
	::SelectObject( hdc, backbuffer );
	::FillRect( hdc, &rect, ::CreateSolidBrush( bkUnselectedColorref ) );
	
	presenter->OnDraw();

	::BitBlt( curhdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hdc, 0, 0, SRCCOPY );

	::DeleteObject( backbuffer );
	::DeleteDC( hdc );

	::EndPaint( hwnd, &ps );
}

LRESULT CEquationEditorWindow::equationEditorWindowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	CEquationEditorWindow *wnd = nullptr;

	if( message == WM_NCCREATE ) {
		// Получаем указатель на экземпляр нашего окна, который мы передали в функцию CreateWindowEx
		wnd = static_cast<CEquationEditorWindow*>(LPCREATESTRUCT( lParam )->lpCreateParams);
		// И сохраняем в поле GWL_USERDATA
		::SetWindowLong( handle, GWL_USERDATA, reinterpret_cast<LONG>(LPCREATESTRUCT( lParam )->lpCreateParams) );
		// Запоминаем handle
		wnd->hwnd = handle;
	}
	// Теперь получаем указатель на наш экземпляр окна, но уже из поля GWL_USERDATA
	wnd = reinterpret_cast<CEquationEditorWindow*>(::GetWindowLong( handle, GWL_USERDATA ));

	switch( message ) {
	case WM_DESTROY:
		wnd->OnDestroy();
		return 0;

	case WM_CREATE:
		wnd->OnCreate();
		return 0;

	case WM_PAINT:
		wnd->OnDraw();
		return 0;

	case WM_SIZE:
		wnd->OnSize( LOWORD( lParam ), HIWORD( lParam ) );
		return 0;

	case WM_CHAR:
		wnd->OnChar( wParam );
		return 0;

	case WM_LBUTTONDOWN:
		wnd->OnLButtonDown( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
		return 0;

	case WM_RBUTTONDOWN:
	{
		wnd->OnRButtonDown( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
	}

	case WM_MOUSEMOVE:
		wnd->OnMouseMove( wParam, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
		return 0;

	case WM_COMMAND:
		wnd->OnWmCommand( wParam, lParam );
		return 0;

	case WM_KEYDOWN:
		wnd->OnKeyDown( wParam );
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_INITMENUPOPUP:
		wnd->InitMenu( (HMENU)wParam );
		return 0;
	}
	return ::DefWindowProc( handle, message, wParam, lParam );
}