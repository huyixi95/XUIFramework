/* Generated by Together */

#include "stdafx.h"
#include "EProperty.h"
#include "EPropList.h"
#include "IPropertyHost.h"
#include "EPropListCtrl.h"

#include "SetRawPixels.h"
#include "NodeOpen.h"
#include "NodeClosed.h"
#include "ECommentPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(EPropListCtrl, EPropWnd)

BEGIN_MESSAGE_MAP(EPropListCtrl, EPropWnd)
	//{{AFX_MSG_MAP(EPropListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//afx_msg void EPropListCtrl::QQQ()
//{
//	TRACE("RR");
//}

EPropListCtrl::EPropListCtrl()
{
	m_bAutoSizeKeyColumn = false;

	m_nKeyColumnWidth = 100;
	m_nSplitWidth     = 6;


	m_nMouseDownKeyColumnWidth = -1;
	//m_nMouseDownValColumnWidth = -1;
	m_nMouseDownMousePosition  = -1;

	m_pPropertyHost   = NULL;

	m_bHScrollEnabled = false;
	m_bVScrollEnabled = false;

	m_pDrawnProperties = NULL;
	m_nDrawnPropertiesSize = 0;

}

EPropListCtrl::~EPropListCtrl()
{
	if( m_pDrawnProperties != NULL )
	{
		delete m_pDrawnProperties;
		m_pDrawnProperties = NULL;
	}
}


//#ifdef _DEBUG
//void EPropListCtrl::AssertValid() const
//{
//	CWnd::AssertValid();
//}
//
//void EPropListCtrl::Dump(CDumpContext& dc) const
//{
//	CWnd::Dump(dc);
//}
//#endif //_DEBUG


//BOOL EPropListCtrl::OnCommand(WPARAM wParam, LPARAM lParam) 
//{
//	int nBeg;
//	int nEnd;
//	GetIDRange(nBeg,nEnd);
//
////	TRACE("PropView wParam=%d lParam=%d!\n" , wParam , lParam );
//
//	if( nBeg <= wParam && wParam <= nEnd )
//	{
//		TRACE("Command within PropView range!\n" , wParam , lParam );
//
//		if( m_pActiveProperty != NULL )
//		{
//			bool bChanged = m_pActiveProperty->OnCommand(this,wParam,lParam);
//
//			if( bChanged )
//			{
//				NotifyPropertyChanged();
//			}
//
//			Invalidate();
//
//		}
//
//		return TRUE;
//	}
//
//	return CWnd::OnCommand(wParam, lParam);
//}

/*
void EPropListCtrl::OnInitialUpdate()
{
//	CWnd::OnInitialUpdate();


	m_PropList.Clear();

//	ASSERT( m_pPropertyHost   == NULL );	//was not previously deleted???
//	ASSERT( m_pActiveProperty == NULL );	//was not previously deleted???
	m_pPropertyHost   = NULL;

	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
//	SetScrollSizes(MM_TEXT, sizeTotal);
}
*/


//
// prepare drawn properties list, and properties rect's
//
// use for hittest, simplify drawing
//
int EPropListCtrl::_UpdateDrawnPropertiesList( CDC* pDC , int y0 )
{
	//
	// fit propterty array size
	//

	if( m_PropList.Size() > m_nDrawnPropertiesSize )
	{
		if( m_pDrawnProperties != NULL )
		{
			delete m_pDrawnProperties;
		}
		m_nDrawnPropertiesSize = m_PropList.Size();
		m_pDrawnProperties = new EProperty*[ m_nDrawnPropertiesSize ];
	}

	//
	//
	//

	EProperty* pOldProperty = NULL;
	int nNumProperties = m_PropList.Size();

	//
	// set has_children flags
	//

	for( int i=0 ; i<nNumProperties ; i++ )
	{
		EProperty* pProp = m_PropList.Get(i);

		pProp->m_bHasChildren = false;		//default to false

		if( pOldProperty != NULL )
		{
			if( pProp->m_nIndent > pOldProperty->m_nIndent )
			{
				pOldProperty->m_bHasChildren = true;
			}
		}

		pOldProperty = pProp;
	}

	//
	//
	//

	m_nNumDrawnProperties = 0;

	int  nLastOpenIndent   = 0;
	bool bLastPropWasShown = true;
	bool bLastPropWasOpen  = true;

	for( int i=0 ; i<nNumProperties ; i++ )
	{
		EProperty* pProp = m_PropList.Get(i);

		bool bShown = true;

		if( pProp->m_nIndent > nLastOpenIndent )
		{
			if( bLastPropWasShown && bLastPropWasOpen )
			{
			}
			else
			{
				bShown = false;
			}
		}

		if( bShown )
		{
			//
			// is this property drawn?
			//

			int nHeight = pProp->GetHeight(pDC) + m_nEdgeWidth * 2;		//height of this property panel inclusive borders

			pProp->m_Rect.SetRect( 0 , y0 , 0 , y0 + nHeight );

			m_pDrawnProperties[m_nNumDrawnProperties] = pProp;
			m_nNumDrawnProperties++;

			y0 += nHeight;

			nLastOpenIndent = pProp->m_nIndent;
		}
		else
		{
			pProp->m_Rect.SetRect( 0 , y0 , 0 , y0 );
		}

		bLastPropWasShown = bShown;
		bLastPropWasOpen  = pProp->m_bNodeOpen;
	}

	return y0;
}

void EPropListCtrl::OnPaint()
{
	//
	//
	//

//this causes a problem with the active property, which is erased when clearing
//	if( m_pPropertyHost != NULL )
//	{
//		m_PropList.Clear();
//		m_pPropertyHost->GetProperties( m_PropList );
//	}

	//
	//
	//

	CPoint vpo;
	vpo.y = -GetScrollPos(SB_VERT);
	vpo.x = -GetScrollPos(SB_HORZ);

	CRect cr;
	GetClientRect(&cr);

	CPaintDC dc(this);
	CDC* pDC = &dc;
	pDC->SelectObject( &m_Font );
	pDC->SetBkMode(TRANSPARENT);

//	CPoint vpo = pDC->GetViewportOrg();

	int y0 = vpo.y;	//vpo.y;
	int x0 = vpo.x;	//vpo.x;
	int x1 = x0 + m_nKeyColumnWidth;
	int x2 = x1 + m_nSplitWidth;
	int x3 = x2 + m_nValColumnWidth;

//pDC->SelectClipRgn(NULL);
//pDC->FillSolidRect(0,0,1000,1000,RGB(0,255,0));
//	pDC->MoveTo( cr.left , cr.top );
//	pDC->LineTo( cr.right , cr.bottom );

	CRect r;


	//
	//
	//

	int nBottomy = _UpdateDrawnPropertiesList(pDC,y0);

	//
	//
	//
	
	if( m_PropList.Size() == 0 )
	{
		DrawFrame( pDC , cr , true , GetColorPaneKey() );
		pDC->SetTextAlign( TA_CENTER );
		
		CString s = (m_pChangeListener==NULL) ? "NO PROPERTIES" : m_pChangeListener->GetNoPropertiesMessage();

		pDC->TextOut( cr.Width()/2 , 10 , s );
	}
	else
	{
		pDC->SetTextAlign( TA_TOP );

		for( int i=0 ; i<m_nNumDrawnProperties ; i++ )
		{
			EProperty* pProp = m_pDrawnProperties[i];
			int nHeight = pProp->m_Rect.Height();

			//
			// key panel
			//

			r = pProp->m_Rect;
			r.left  = x0;
			r.right = x1;

			pDC->FillSolidRect( r.left , r.top , pProp->m_nIndent * 13 , r.Height() , GetColorPaneKey() );

			r.left += pProp->m_nIndent * 13;

			DrawFrame( pDC , r , true , pProp->GetColorKey(this) );

			if( pProp->m_bHasChildren )
			{
				CRect nr( 0 , 0 , pNodeClosed[0] , pNodeClosed[1] );

				if( true )	//right aligned
				{
					nr.OffsetRect( r.right , r.top );
					nr.OffsetRect( -4-pNodeClosed[0] ,6 );
				}
				else
				{
					nr.OffsetRect( r.left , r.top );
					nr.OffsetRect( 4 , 6 );
					r.left += pNodeClosed[0] + 4;
				}

				pProp->m_NodeOpenRect = nr;
				CPoint p = pProp->m_NodeOpenRect.TopLeft();
				CPoint vpo = pDC->OffsetViewportOrg(p.x,p.y);
				int* pPixels =  pProp->m_bNodeOpen ? pNodeOpen : pNodeClosed;
				SetRawPixels( pDC , pPixels );
				pDC->SetViewportOrg( vpo );
			}


			CString s = pProp->GetName();								//key text

			int nTexty = r.top + ( r.Height() - GetFontHeight() ) / 2;

			pDC->TextOut( r.left + m_nEdgeWidth + 4 , nTexty , s );									//draw key text

			//
			// value panel
			//

			r = pProp->m_Rect;
			r.left  = x2;
			r.right = x3;

			DrawFrame( pDC , r , true , -1 );

			r.DeflateRect(m_nEdgeWidth,m_nEdgeWidth);
			pProp->Draw( pDC , this ,  r );
			r.InflateRect(m_nEdgeWidth,m_nEdgeWidth);
		}

		//
		// splitter
		//

		r.left   = x1;
		r.right  = x2;
		r.top    = 0;
		r.bottom = r.bottom;

		DrawFrame( pDC , r , true , GetColorPaneKey() );		//splitter

		m_SplitRectCP = r;
		m_SplitRectCP.OffsetRect( pDC->GetViewportOrg() );

		//
		// empty bottom
		//

		int nViewBottom = cr.bottom - nBottomy;

		if( nBottomy < cr.bottom )
		{
			r = cr;
			r.top = nBottomy;
			DrawFrame( pDC , r , true , GetColorPaneKey() );
		}


	}
}




/*
void EPropListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE("view keydown\n");
	if( nChar == 27 )	//escape
	{
		if( m_pActiveProperty != NULL )
		{
			m_pActiveProperty->CancelEdit();
			m_pActiveProperty = NULL;
			Invalidate();
		}
	}
	else
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}
*/

void EPropListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_nMouseDownMousePosition  = -1;
	m_nMouseDownKeyColumnWidth = -1;
	//m_nMouseDownValColumnWidth = -1;
	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

//void EPropListCtrl::OnLButtonDblClk( UINT nFlags , CPoint point)
//{
//	TRACE("Double Click\n");
//	OnLButtonDown( nFlags , point );
//}


void EPropListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	int x0 = m_nKeyColumnWidth  - GetScrollPos( SB_HORZ );
	int x1 = x0 + m_nSplitWidth;	// - GetScrollPos( SB_VERT );

	if( m_PropList.Size() == 0 )
	{
		return;
	}

	if( m_SplitRectCP.PtInRect(point) )			//begin split move?
	{
		m_nMouseDownKeyColumnWidth = m_nKeyColumnWidth;
		m_nMouseDownMousePosition  = point.x;
		SetCapture();
	}
	else										//begin edit another property?
	{
		int nHeight = - GetScrollPos( SB_VERT);

		EProperty* pNewProperty = NULL;

		CPaintDC dc(this);
		dc.SelectObject(&m_Font);
		
		for( int i=0 ; i<m_PropList.Size() ; i++ )
		{
			EProperty* pProp = m_PropList.Get(i);

			//nHeight += pProp->GetHeight(&dc) + m_nEdgeWidth * 2;	//add up height so far...
			//nHeight += pProp->m_Rect.Height();

			if( pProp->m_Rect.Height() > 0 )
			{
				if( (pProp->m_Rect.top<=point.y) &&  (point.y<pProp->m_Rect.bottom) )
				{
					pNewProperty = pProp;

					if( pProp->m_bHasChildren )
					{
						if( pProp->m_NodeOpenRect.PtInRect( point ) )
						{
							if( pProp->m_bNodeOpen )
							{
								pProp->m_bNodeOpen = false;
								Invalidate();
							}
							else
							{
								OpenNode( pProp );
							}
						}
					}

					break;
				}
			}
		}

		//
		//
		//

		if( pNewProperty != NULL )
		{
			if( pNewProperty->IsSelectable()==false )
			{
				pNewProperty = NULL;
			}
		}

		SetActiveProperty(pNewProperty);
		EnsureVisibility( pNewProperty );

		if( m_pActiveProperty != NULL )
		{
			if( m_pActiveProperty->OnLButtonDown( this , point ) )
			{
				NotifyPropertyChanged();
			}
		}

		Invalidate();
	}
}


void EPropListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( nFlags & MK_LBUTTON )					//left button down?
	{
		if( m_nMouseDownMousePosition != -1 )	//dragging splitter?
		{
			int nDelta = point.x - m_nMouseDownMousePosition;
			m_nKeyColumnWidth = m_nMouseDownKeyColumnWidth + nDelta;

			if( m_nKeyColumnWidth < 0 )
			{
				m_nKeyColumnWidth = 0;
			}

			CRect cr;
			GetClientRect(&cr);
			SetScrollPage(cr.Width(),cr.Height());
			Invalidate();
		}

	}
	else
	{
		m_nMouseDownKeyColumnWidth = -1;
		m_nMouseDownMousePosition = -1;
		ReleaseCapture();
	}

	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnMouseMove(nFlags, point);
}

BOOL EPropListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
//	pDC->SelectStockObject( BLACK_PEN );
//	pDC->SelectStockObject( WHITE_BRUSH );
//	pDC->Rectangle( 0,0,1000,1000 );
//	return TRUE;

}



CSize EPropListCtrl::GetScrollPageSize( int nViewWidth , int nViewHeight )
{
	CSize ScrollPage;

	if( m_PropList.Size() == 0 )
	{
		ScrollPage.cx = 0;
		ScrollPage.cy = 0;
	}
	else
	{
		CRect cr;
		GetClientRect(&cr);

		CDC* pDC = GetDC();
		pDC->SelectObject( &m_Font );

		int nScrollPageHeight = 0;
		m_nValColumnWidth = 0;

		for( int i=0 ; i<m_PropList.Size() ; i++ )
		{
			EProperty* pProp = m_PropList.Get(i);

			int w = pProp->GetPreferedWidth( pDC );

			if( w > m_nValColumnWidth )
			{
				m_nValColumnWidth = w;
			}

			nScrollPageHeight += pProp->GetHeight(pDC) + m_nEdgeWidth * 2;
		}

		int nScrollPageWidth = m_nKeyColumnWidth + m_nSplitWidth + m_nValColumnWidth;


		if( nScrollPageWidth < cr.Width() )
		{
			m_nValColumnWidth = cr.Width() - m_nKeyColumnWidth - m_nSplitWidth;
		}

		ScrollPage.cx = nScrollPageWidth;
		ScrollPage.cy = nScrollPageHeight;

		ReleaseDC(pDC);
	}

	return ScrollPage;
}




void EPropListCtrl::SetActiveTab(int i)
{
	if( i == m_PropList.GetActiveTab() )
	{
	}
	else
	{
		SetActiveProperty(NULL);
		m_PropList.SetActiveTab(i);
		CRect cr;
		GetClientRect(&cr);
		SetScrollPage(cr.Width(),cr.Height());
		if (m_bAutoSizeKeyColumn)
			AutoSizeKeyColumn();
		Invalidate();
	}
}

EPropList* EPropListCtrl::GetPropertyList()
{
	return &m_PropList;
}


void EPropListCtrl::SetPropertyHost( IPropertyHost* pHost )
{
	//
	// first apply and close old active property
	//

	OnDoPropApply();
	OnDoPropClose();
	ASSERT( m_pActiveProperty == NULL );

	//
	// if it's the same propertyhost, try to remember active tab
	//

	CString sOldActiveTab;

	if( m_PropList.HasUserTabs() )
	{
		int n = m_PropList.GetActiveTab();
		if( n>=0 )
		{
			EPropList::ETabGroup* pTab = m_PropList.GetTab(n);
			sOldActiveTab = *pTab->GetName();
		}
	}

	//
	//
	//
	
	m_PropList.Clear();

	ASSERT( m_PropList.Size() == 0 );
	ASSERT( m_PropList.GetTabCount() == 0 );
	ASSERT( m_PropList.HasUserTabs() == false );
	ASSERT( m_PropList.GetActiveTab() == -1 );

//m_pactive property has been deleted during proplist clearing
//	if( m_pActiveProperty != NULL )
//	{
//		m_pActiveProperty->OnPropClose();
//		m_pActiveProperty = NULL;
//	}

	m_pActiveProperty = NULL;
	m_pPropertyHost   = pHost;

	if( m_pPropertyHost == NULL )
	{
		if( m_pCommentPane != NULL )
		{
			m_pCommentPane->SetComment( "No Selection..." );
		}
	}
	else
	{
		m_pPropertyHost->GetProperties( m_PropList );

		//
		// search for tab with same name as old list..
		//

		int nActiveTab = 0;
		int n = m_PropList.GetTabCount();
		for( int i=0 ; i<n ; i++ )
		{
			if( m_PropList.GetTab(i)->GetName()->CompareNoCase(sOldActiveTab)==0 )
			{
				nActiveTab = i;
			}
		}
		m_PropList.SetActiveTab(nActiveTab);

		if (m_bAutoSizeKeyColumn)
		{
			AutoSizeKeyColumn();
		}

		if( m_pCommentPane != NULL )
		{
			m_pCommentPane->SetComment( m_PropList.m_sHostComment );
		}
	}


	CRect cr;
	GetClientRect(&cr);
	SetScrollPage(cr.Width(),cr.Height());
	Invalidate();
}

void EPropListCtrl::AutoSizeKeyColumn()
{
	//
	// try calculate default key column size
	//
	int nActiveTab = m_PropList.GetActiveTab();
	if ( nActiveTab>=0 )
	{
		int n = m_PropList.GetTab(nActiveTab)->Size();

		CPaintDC dc(this);
		dc.SelectObject(&m_Font);

		int nMargin = 4 * 2;
		for( int i=0 ; i<n ; i++ )
		{
			EProperty* pProp = m_PropList.GetTab(nActiveTab)->Get(i);
			CString s = pProp->GetName();
			CSize size = dc.GetTextExtent(s);
			if( size.cx + nMargin > m_nKeyColumnWidth )
			{
				m_nKeyColumnWidth = size.cx + nMargin;
			}
		}
	}
}

void EPropListCtrl::RefreshHostList()
{
	SetPropertyHost(m_pPropertyHost);
}

void EPropListCtrl::SetPropertyChangeListener( IPropertyChangeListener* pListener )
{
	m_pChangeListener = pListener;
}







void EPropListCtrl::StepSelection( int nStep )
{
//------------
//	int nNumProperties = m_PropList.Size();
//
//	for( int i=0 ; i<nNumProperties ; i++ )
//	{
//		EProperty* pProp = m_PropList.Get(i);
//		if( pProp == m_pActiveProperty )
//		{
//			i += nStep;
//
//			int i0 = 0;
//			int i1 = nNumProperties - 1;
//
//			if( i<i0 ) i=i0;
//			if( i>i1 ) i=i1;
//
//			EProperty* pNewActive = m_PropList.Get(i);
//
//			SetActiveProperty( pNewActive );
//			EnsureVisibility( pNewActive );
//			break;
//		}
//	}
//-------------

	for( int i=0 ; i<m_nNumDrawnProperties ; i++ )
	{
		if( m_pDrawnProperties[i] == m_pActiveProperty )
		{
			i += nStep;

			int i0 = 0;
			int i1 = m_nNumDrawnProperties - 1;

			//if( i<i0 ) i=i0;
			//if( i>i1 ) i=i1;

			EProperty* pNewActive = NULL;	//null is stepping outside because tab on last entry should also work, instead of "apply" being canceled

			if( i0<=i && i<=i1 )
			{
				pNewActive = m_pDrawnProperties[i];
			}

			SetActiveProperty( pNewActive );
			EnsureVisibility( pNewActive );

			break;
		}
	}

}

void EPropListCtrl::EnsureVisibility( EProperty* pProperty )
{
	if( pProperty!=NULL )
	{
		CRect r = pProperty->m_Rect;
		r.InflateRect(1,1);
		EnsureVisibility(r);
	}
}

void EPropListCtrl::EnsureVisibility( const CRect& r )
{
	CRect cr;
	GetClientRect(&cr);

	int nCurrentScrollPos = GetScrollPos( SB_VERT );

	int nTop    = r.top;
	int nDeltay = 0;

	if( r.bottom > cr.bottom )
	{
		nDeltay -= (r.bottom - cr.bottom);	//ryk lidt op
		nTop    += nDeltay;
	}

	if( nTop < 0 )							//hvis toppen kom over �verste..
	{
		nDeltay -= nTop;
	}

	if( nDeltay != 0 )
	{
		SetScrollPos( SB_VERT , nCurrentScrollPos - nDeltay );
	}
}


BOOL EPropListCtrl::PreTranslateMessage( MSG* pMsg )
{
	BOOL bHandled = FALSE;

	if( pMsg->message == WM_KEYDOWN )
	{
		bHandled = TRUE;

		bool bControl = (GetKeyState(VK_CONTROL)&0x80000000) != 0;
		bool bShift   = (GetKeyState(VK_SHIFT)  &0x80000000) != 0;

		if( pMsg->wParam == VK_RETURN )
		{
			OnDoPropClose();
		}
		else if(pMsg->wParam == VK_ESCAPE)
		{
			m_bApplyOnKillFocus = false;
			OnDoPropClose();
		}
		else if(pMsg->wParam == VK_UP)
		{
			StepSelection( -1 );
		}
		else if(pMsg->wParam == VK_DOWN )
		{
			StepSelection( 1 );
		}
		else if(pMsg->wParam == VK_END )
		{
			bHandled = FALSE;
			if( m_pActiveProperty != NULL )
			{
				if( GetFocus() == this )
				{
					StepSelection( (int)1E6 );
					bHandled = TRUE;
				}
			}
		}
		else if(pMsg->wParam == VK_HOME )
		{
			bHandled = FALSE;
			if( m_pActiveProperty != NULL )
			{
				if( GetFocus() == this )
				{
					StepSelection( (int)-1E6 );
					bHandled = TRUE;
				}
			}
		}
		else if(pMsg->wParam == VK_NEXT )
		{
			StepSelection( 3 );
		}
		else if(pMsg->wParam == VK_PRIOR )
		{
			StepSelection( -3 );
		}
		else if(pMsg->wParam == VK_TAB )
		{
			if(bShift)
			{
				StepSelection( -1 );
			}
			else
			{
				StepSelection(  1 );
			}
		}
		else if(pMsg->wParam == VK_RIGHT )
		{
			bHandled = FALSE;
			
			if( m_pActiveProperty != NULL )
			{
				if( m_pActiveProperty->m_bHasChildren )
				{
					if( GetFocus() == this )		//not the edit
					{
						if( m_pActiveProperty->m_bNodeOpen == false )
						{
							OpenNode( m_pActiveProperty );
							bHandled = TRUE;
						}
					}
				}
			}
		}
		else if(pMsg->wParam == VK_LEFT )
		{
			bHandled = FALSE;
			if( m_pActiveProperty != NULL )
			{
				if( m_pActiveProperty->m_bHasChildren )
				{
					if( GetFocus() == this )		//not the edit
					{
						if( m_pActiveProperty->m_bNodeOpen == true )
						{
							m_pActiveProperty->m_bNodeOpen = false;
							Invalidate();
							bHandled = TRUE;
						}
					}
				}
			}
		}
		else if(pMsg->wParam == VK_SPACE )
		{
			bHandled = FALSE;
			if( m_pActiveProperty != NULL )
			{
				if( GetFocus() == this )
				{
					if( m_pActiveProperty->OnSpaceKey() )
					{
						NotifyPropertyChanged();
						Invalidate();
						bHandled = TRUE;
					}

					if( m_pActiveProperty->m_bHasChildren )
					{
						if( m_pActiveProperty->m_bNodeOpen )
						{
							m_pActiveProperty->m_bNodeOpen = false;
							Invalidate();
						}
						else
						{
							OpenNode( m_pActiveProperty );
						}
						bHandled = TRUE;
					}

				}
			}
		}
		else
		{
			bHandled = FALSE;
		}
	}

	if( bHandled == FALSE )
	{
		bHandled = CWnd::PreTranslateMessage(pMsg);
	}

	return bHandled;
}



void EPropListCtrl::OpenNode( EProperty* pProp )
{
	if( pProp == NULL )
	{
		return;
	}

	if( pProp->m_bHasChildren == false )
	{
		return;
	}

	pProp->m_bNodeOpen = true;

	//
	//
	//

	int n = m_PropList.Size();

	CDC* pDC = GetDC();
	pDC->SelectObject( &m_Font );

	for( int i=0 ; i<n ; i++ )
	{
		EProperty* pCur = m_PropList.Get(i);
		if( pCur==pProp )
		{
			int nIndent = pProp->m_nIndent;

			i++;

			CRect r = pProp->m_Rect;
			r.bottom += m_nEdgeWidth;

			bool bReady = false;

			while(i<n)
			{
				pCur = m_PropList.Get(i);

				if( nIndent == pCur->m_nIndent )
				{
					bReady = true;
				}
				else
				{
					r.bottom += pCur->GetHeight(pDC) + 2 * m_nEdgeWidth;
				}

				i++;

				if( i==n )
				{
					bReady = true;
				}

				if( bReady )
				{
					EnsureVisibility(r);
					break;
				}
			}


		}
	}

	ReleaseDC(pDC);


	//
	//
	//

	Invalidate();
}

void EPropListCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_pActiveProperty != NULL )
	{
		if( m_pActiveProperty->m_bHasChildren )
		{
			if( m_pActiveProperty->m_bNodeOpen )
			{
				m_pActiveProperty->m_bNodeOpen = false;
			}
			else
			{
				OpenNode( m_pActiveProperty );
			}
			Invalidate();
		}
	}
}
