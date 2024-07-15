#pragma once

#include "ToastWindow.h"


class CToastWindowManager
{
private:
	// Types
	typedef std::map<HWND, CToastWindow*> wnd;
	typedef wnd::iterator wnd_itr;

public:
	CToastWindowManager(void);
	virtual ~CToastWindowManager(void);

	void AddWindow(HWND aHwnd);
	void RemoveWindow(HWND aHwnd);

private:
	// Objects
	wnd m_windows;
};
