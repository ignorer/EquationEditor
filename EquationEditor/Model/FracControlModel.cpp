﻿#include "Model/FracControlModel.h"
#include "Model/EditControlModel.h"
#include "Model/Utils/GeneralFunct.h"

#include <string>

CFracControlModel::CFracControlModel( CRect rect, std::weak_ptr<IBaseExprModel> parent ) {
	this->parent = parent;
	this->rect = CRect( 0, 0, 0, rect.GetHeight() ); // нас интересует только высота, остальное исправится сразу же после инвалидации дерева
	this->params.polygon.push_back( CLine( rect.Left( ), rect.GetHeight( ) / 2, rect.Right( ), rect.GetHeight( ) / 2 ) );
}

void CFracControlModel::Resize( )
{
	int width = MAX( firstChild->GetRect().GetWidth(), secondChild->GetRect().GetWidth());
	int height = firstChild->GetRect().GetHeight() + secondChild->GetRect().GetHeight() + 5; // +5 для промежутка между числителем и знаменателем

	rect.Right() = rect.Left() + width;
	rect.Bottom() = rect.Top() + height;
}

void CFracControlModel::PlaceChildren( )
{
	CRect newRect;
	int middle = (rect.Right() + rect.Left()) / 2;
	
	CRect oldRect = firstChild->GetRect( );
	newRect.Top() = rect.Top();
	newRect.Bottom() = rect.Top() + oldRect.GetHeight();
	newRect.Left() = middle - oldRect.GetWidth() / 2;
	newRect.Right() = middle + oldRect.GetWidth() / 2;
	firstChild->SetRect( newRect );
	
	oldRect = secondChild->GetRect( );
	newRect.Bottom() = rect.Bottom();
	newRect.Top() = rect.Bottom() - oldRect.GetHeight();
	newRect.Left() = middle - oldRect.GetWidth() / 2;
	newRect.Right() = middle + oldRect.GetWidth() / 2;
	secondChild->SetRect( newRect );
	params.polygon.front( ).Set( rect.Left( ), rect.Top( ) + GetMiddle( ), rect.Right( ), rect.Top( ) + GetMiddle( ) );
}

int CFracControlModel::GetMiddle( ) const
{
	return (firstChild->GetRect().GetHeight() + (rect.GetHeight() - secondChild->GetRect().GetHeight())) / 2;
}

void CFracControlModel::InitializeChildren() 
{
	CRect childRect = CRect( 0, 0, 0, rect.GetHeight() );
	firstChild = std::make_shared<CExprControlModel>( CExprControlModel( childRect, std::weak_ptr<IBaseExprModel>( shared_from_this() ) ) );
	firstChild->InitializeChildren();

	secondChild = std::make_shared<CExprControlModel>( CExprControlModel( childRect, std::weak_ptr<IBaseExprModel>( shared_from_this() ) ) );
	secondChild->InitializeChildren();

	Resize();
	PlaceChildren();
}

std::list<std::shared_ptr<IBaseExprModel>> CFracControlModel::GetChildren() const {
	return std::list<std::shared_ptr<IBaseExprModel>> { firstChild, secondChild };
}

void CFracControlModel::SetRect( CRect rect ) {
	this->rect = rect;
	params.polygon.front().Set( rect.Left(), rect.Top() + GetMiddle(), rect.Right(), rect.Top() + GetMiddle() );
	::OutputDebugString( std::to_wstring( GetRect().Top() + GetMiddle() ).c_str() );
	::OutputDebugString( (LPCWSTR) " " );
	::OutputDebugString( std::to_wstring( firstChild->GetRect().Bottom() ).c_str() );
	::OutputDebugString( (LPCWSTR) " " );
	::OutputDebugString( std::to_wstring( secondChild->GetRect().Top() ).c_str() );
	::OutputDebugString( (LPCWSTR) " " );
	::OutputDebugString( std::to_wstring( rect.Top() ).c_str() );
	::OutputDebugString( (LPCWSTR) "\n" );
}

ViewType CFracControlModel::GetType() const {
	return FRAC;
}

void CFracControlModel::MoveBy( int dx, int dy ) {
	rect.MoveBy( dx, dy );
	params.polygon.front().MoveBy( dx, dy );
}
