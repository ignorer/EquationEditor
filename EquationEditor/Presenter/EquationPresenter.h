﻿#pragma once
#include <utility>

#include "Model/IBaseExprModel.h"
#include "Model/ExprControlModel.h"
#include "Model/EditControlModel.h"
#include "Model/FracControlModel.h"

enum ViewType { TEXT, EXPR, FRAC };

// Интерфейс окна редактора
class IEditorView {
public:
	virtual ~IEditorView() = 0;

	// Отобразить текст в определенном прямоугольнике
	virtual void DrawText( HDC hdc, std::wstring text, RECT rect ) = 0;

	// Нарисовать ломаную
	virtual void DrawPolygon( HDC hdc, std::list<CLine> polygon ) = 0;

	// Установить положение каретки
	virtual void SetCaret( POINT caretPoint, int height ) = 0;

	// Запустить перерисовку окна
	virtual void Redraw() = 0;

	virtual int GetCharWidth( wchar_t symbol ) = 0;
};

inline IEditorView::~IEditorView() {}

struct CCaret {
	// Текущий edit control, на котором стоит каретка
	CEditControlModel* curEdit;
	// Координаты каретки на экране
	POINT caretPoint;
	// Номер символа, за которым стоит каретка
	int offset;

	CCaret() : offset( 0 ) {
		caretPoint.x = 0;
		caretPoint.y = 0;
		curEdit = nullptr;
	}
};

// Класс, размещающий прямоугольники вьюшек на экране
class CEquationPresenter {
public:
	CEquationPresenter( IEditorView* view );

	void AddControlView( ViewType viewType );

	void InsertSymbol( wchar_t symbol );

	void DeleteSymbol();

	void Draw( HDC hdc );
	
	void SetCaret( int x, int y );
private:
    CExprControlModel* root;
	CCaret caret;
	IEditorView* view;

	void addFrac( CExprControlModel* parent );
	void setFracRects( RECT parentRect, CFracControlModel* fracModel );

	void setRect( IBaseExprModel* model, RECT rect );

	bool isInTheRect( int x, int y, RECT rect );
	
	// Ищет позицию каретки с таким x
	// Возвращает пару <координата, номер буквы>
	std::pair<int, int> findCaretPos( CEditControlModel* editControlModel, int x );

	void updateTreeAfterSizeChange(IBaseExprModel* startVert);
};