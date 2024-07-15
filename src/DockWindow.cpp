#include "stdafx.h"
#include "DockWindow.h"


// ----------------------------------------------------------------------------
CDockWindow::CDockWindow(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CDockWindow::~CDockWindow(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
// Message handler
LRESULT CDockWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	// CWindow
	SetWindowLong(GWL_STYLE, CAppWinTraits::GetWndStyle(0));
	SetWindowLong(GWL_EXSTYLE, CAppWinTraits::GetWndExStyle(0));
	SetWindowText(_T("Placeholder"));
	
	// Change background color
	enum { COLOR_WHITE = RGB(255, 255, 255) };
	HBRUSH hbrush = ::CreateSolidBrush(COLOR_WHITE);
	::SetClassLongPtr(m_hWnd, GCL_HBRBACKGROUND, (LONG_PTR)hbrush);

	// Place the window at the bottom of the UI stack
	SetWindowPos(HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// (CAppBar) Init the docking bar
	InitAppBar(APPBAR_DOCKING_TOP);
	DockAppBar(APPBAR_DOCKING_TOP);

	bHandled = TRUE;
	return 0;
}
