﻿#pragma once

#include <utility>
#include <memory>
#include <Windows.h>

#include "Model/IBaseExprModel.h"
#include "Model/ExprControlModel.h"
#include "Model/EditControlModel.h"
#include "Model/FracControlModel.h"
#include "Model/DegrControlModel.h"
#include "Model/Utils/Caret.h"

#include "Presenter/Utils/TreeBfsProcessor.h"
#include "Presenter/Utils/TreeDfsProcessor.h"

// Интерфейс окна редактора
class IEditorView {
public:
	virtual ~IEditorView() {}

	// Отобразить текст в определенном прямоугольнике
	virtual void DrawText( std::wstring text, CRect rect ) = 0;

	// Нарисовать ломаную
	virtual void DrawPolygon( std::list<CLine> polygon ) = 0;

	// Установить положение каретки
	virtual void SetCaret( int caretPointX, int caretPointY, int height ) = 0;

	// Запустить перерисовку окна
	virtual void Redraw() = 0;

	virtual int GetCharWidth( wchar_t symbol ) = 0;
};

// Класс, размещающий прямоугольники вьюшек на экране
class CEquationPresenter {
public:
	CEquationPresenter( IEditorView& newView );
	~CEquationPresenter();

	void AddControlView( ViewType viewType );

	void InsertSymbol( wchar_t symbol );

	void DeleteSymbol();

	// Отправляет во вьюшку всё, что нужно на ней нарисовать
	void OnDraw();
	
	void SetCaret( int x, int y );
private:
    std::shared_ptr<CExprControlModel> root;
	IEditorView& view;
	CCaret caret;

	// processors
	CTreeBfsProcessor placeProcessor;
	CTreeDfsProcessor resizeProcessor;
	CTreeBfsProcessor drawer;

	void addFrac( std::shared_ptr<CExprControlModel> parent );
	void setFracRects( CRect parentRect, std::shared_ptr<CFracControlModel> fracModel );

	void addDegr( std::shared_ptr<CExprControlModel> parent );
	void setDegrRects( CRect parentRect, std::shared_ptr<CDegrControlModel> degrModel );
	
	// Ищет позицию каретки с таким x
	// Возвращает пару <координата, номер буквы>
	std::pair<int, int> findCaretPos( std::shared_ptr<CEditControlModel> editControlModel, int x );

	// не подавайте сюда корень дерева, всё сломается
	void updateTreeAfterSizeChange();
};