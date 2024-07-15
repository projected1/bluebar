#include "stdafx.h"
#include "ToastWindowManager.h"


// ----------------------------------------------------------------------------
CToastWindowManager::CToastWindowManager(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CToastWindowManager::~CToastWindowManager(void)
{
	SCOPE_LOG();

	BOOST_FOREACH(wnd::value_type& val, m_windows)
	{
		delete val.second;
		val.second = NULL;
	}
	m_windows.clear();
}

// ----------------------------------------------------------------------------
void CToastWindowManager::AddWindow(HWND aHwnd)
{
	SCOPE_LOG();

	CToastWindow* pToastWindow = new CToastWindow(aHwnd, *this);
	_ASSERTE(pToastWindow);
	if (pToastWindow && pToastWindow->IsWindow())
	{
		m_windows[aHwnd] = pToastWindow;
	}
}

// ----------------------------------------------------------------------------
void CToastWindowManager::RemoveWindow(HWND aHwnd)
{
	SCOPE_LOG();

	wnd_itr itr = m_windows.find(aHwnd);
	if (itr != m_windows.end())
	{
		if (itr->second->IsWindow())
		{
			itr->second->PostMessage(WM_CLOSE, 0, 0);
		}
		else
		{
			// Called from "CToastWindow"
			delete itr->second;
			itr->second = NULL;
			m_windows.erase(itr);
		}
	}
}
