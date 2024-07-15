#include "stdafx.h"
#include "ToastWindow.h"
#include "ToastWindowManager.h"


// ----------------------------------------------------------------------------
// Static
CToastWindow::wnd_proc CToastWindow::m_childproc;


// ----------------------------------------------------------------------------
CToastWindow::CToastWindow(HWND aHwnd, CToastWindowManager& aManager)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));

	m_pToastWindowManager = &aManager;

	// Init members
	m_bMouseEnter = false;

	// Make window transparent
	::SetWindowLong(aHwnd, GWL_EXSTYLE, ::GetWindowLong(aHwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	BOOL rv = ::SetLayeredWindowAttributes(aHwnd, 0, 0, LWA_ALPHA);
	_ASSERTE(rv);

	// CWindowImpl
	SubclassWindow(aHwnd);

	// Subclass child CEF windows
	::EnumChildWindows(aHwnd, SubclassChildWindowsProc, NULL);
}

// ----------------------------------------------------------------------------
CToastWindow::~CToastWindow(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
LRESULT CToastWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	// Store "m_hWnd" for "OnFinalMessage"
	m_hWndFinal = m_hWnd;

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CToastWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	CPoint pt(LOWORD(lParam), HIWORD(lParam));

	// Create region
	CRgn rgn;
	rgn.CreateRoundRectRgn(0, 0, pt.x, pt.y, 5, 5);
	_ASSERTE(rgn.m_hRgn);

	// Pass ownership of the region handle to the system (do not make any further 
	// function calls with this region handle)
	int rv = ::SetWindowRgn(m_hWnd, rgn.Detach(), TRUE);
	_ASSERTE(rv);

	// Show the window
	m_spFadeInThread.reset(
		new boost::thread(boost::bind(&CToastWindow::FadeIn, this, PRE_FADE_IN_TIMEOUT)));

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CToastWindow::OnDisplayChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CToastWindow::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	if (m_spFadeInThread.get())
	{
		m_spFadeInThread->interrupt();
	}

	// Wait and then fade out
	m_spFadeOutThread.reset(
		new boost::thread(boost::bind(&CToastWindow::FadeOut, this, MANUAL_SHOW_TIMEOUT)));

	bHandled = FALSE;
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT CToastWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	// Signal threads to terminate
	if (m_spFadeInThread.get())
	{
		m_spFadeInThread->interrupt();
	}

	if (m_spFadeOutThread.get())
	{
		m_spFadeOutThread->interrupt();
	}

	// Wait for threads to terminate
	if (m_spFadeInThread.get())
	{
		m_spFadeInThread->join();
	}

	if (m_spFadeOutThread.get())
	{
		m_spFadeOutThread->join();
	}

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
// CMouseEnterEvent
LRESULT CToastWindow::OnMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	m_bMouseEnter = true;
	
	if (m_spFadeOutThread.get())
	{
		m_spFadeOutThread->interrupt();
	}
	FadeIn();

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
// Subclasses child CEF windows
/*static*/ BOOL CALLBACK CToastWindow::SubclassChildWindowsProc(HWND hwnd, LPARAM lParam)
{
	SCOPE_LOG();

	// Find CEF windows
	static enum { MAX_CLASS_NAME_LEN = 1000 };
	TCHAR szClassName[MAX_CLASS_NAME_LEN] = {0};
	int rv = ::GetClassName(hwnd, szClassName, MAX_CLASS_NAME_LEN - 1);
	_ASSERTE(rv);

	// Compare class names (case insensitive)
	std::basic_string<TCHAR> sClassName(szClassName);
	std::transform(sClassName.begin(), sClassName.end(), sClassName.begin(), std::tolower);
	if (sClassName == _T("webviewhost"))
	{
		// Subclass
		m_childproc[hwnd] = (WNDPROC)::SetWindowLongPtr(hwnd, GWL_WNDPROC, 
			(LPARAM)(WNDPROC)SubclassedWindowsWindowProc);
	}

	// Continue
	return TRUE;
}

// ----------------------------------------------------------------------------
// Filters messages that top window needs
/*static*/ LRESULT CALLBACK CToastWindow::SubclassedWindowsWindowProc(
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SCOPE_LOG();

	wnd_proc_itr itr = m_childproc.find(hwnd);
	if (itr != m_childproc.end())
	{
		HWND hwndRoot = ::GetAncestor(hwnd, GA_ROOT);
		_ASSERTE(hwndRoot);

		switch (uMsg)
		{
		case WM_MOUSEMOVE:
		case WM_MOUSELEAVE:
			{
				// Post message to parent window
				BOOL rv = ::PostMessage(hwndRoot, uMsg, wParam, lParam);
				_ASSERTE(rv);
			}
			break;
		}

		return ::CallWindowProc(itr->second, hwnd, uMsg, wParam, lParam);
	}
	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ----------------------------------------------------------------------------
void CToastWindow::FadeIn(unsigned int aTimeout)
{
	SCOPE_LOG();

	bool bInterrupted = false;

	if (aTimeout)
	{
		try
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(aTimeout));
		}
		catch (boost::thread_interrupted const&)
		{
			bInterrupted = true;
		}
	}

	if (!bInterrupted)
	{
		// CLayeredWindow
		SetWindowOpacity(WND_OPAQUE, FADE_IN_DURATION, &CToastWindow::FadeInComplete);
	}
}

// ----------------------------------------------------------------------------
void CToastWindow::FadeOut(unsigned int aTimeout)
{
	SCOPE_LOG();

	bool bInterrupted = false;

	if (aTimeout)
	{
		try
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(aTimeout));
		}
		catch (boost::thread_interrupted const&)
		{
			bInterrupted = true;
		}
	}

	if (!bInterrupted)
	{
		// CLayeredWindow
		SetWindowOpacity(WND_TRANSPARENT, FADE_OUT_DURATION, &CToastWindow::FadeOutComplete);
	}
}

// ----------------------------------------------------------------------------
void CToastWindow::FadeInComplete()
{
	SCOPE_LOG();

	if (!m_bMouseEnter)
	{
		// Wait and then fade out
		m_spFadeOutThread.reset(
			new boost::thread(boost::bind(&CToastWindow::FadeOut, this, AUTO_SHOW_TIMOUT)));
	}
}

// ----------------------------------------------------------------------------
void CToastWindow::FadeOutComplete()
{
	SCOPE_LOG();

	// Window may be already dead (if destroyed from Javascript)
	if (IsWindow())
	{
		PostMessage(WM_CLOSE, 0, 0);
	}
}

// ----------------------------------------------------------------------------
// CWindowImpl
/*virtual*/ void CToastWindow::OnFinalMessage(HWND hWnd)
{
	SCOPE_LOG();

	m_pToastWindowManager->RemoveWindow(m_hWndFinal);
}
