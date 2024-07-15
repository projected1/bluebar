#pragma once

#include "MouseEnterEvent.h"
#include "LayeredWindow.h"


// Forward reference
class CToastWindowManager;

class CToastWindow :
	public CWindowImpl<CToastWindow>,
	public CMouseEnterEvent<CToastWindow>,
	public CLayeredWindow<CToastWindow>
{
public:
	enum 
	{
		FADE_IN_DURATION	= 500,		// 0.5 second
		FADE_OUT_DURATION	= 1000,		// 1 second
		AUTO_SHOW_TIMOUT	= 10000,	// 10 seconds
		MANUAL_SHOW_TIMEOUT	= 5000,		// 5 seconds
		PRE_FADE_IN_TIMEOUT	= 2000		// 2 seconds
	};

public:
	BEGIN_MSG_MAP(CToastWindow)
		// Messages
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnDisplayChange)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)

		// CMouseEnterEvent
		MESSAGE_HANDLER(WM_MOUSEENTER, OnMouseEnter)

		// Chaining
		CHAIN_MSG_MAP(CMouseEnterEvent<CToastWindow>)
		CHAIN_MSG_MAP(CLayeredWindow<CToastWindow>)
	END_MSG_MAP()

	IMPLEMENT_REFCOUNTING(CToastWindow);

public:
	CToastWindow(HWND aHwnd, CToastWindowManager& aManager);
	virtual ~CToastWindow(void);

protected:
	// Message handlers
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDisplayChange(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL&);

	// CMouseEnterEvent
	LRESULT OnMouseEnter(UINT, WPARAM, LPARAM, BOOL&);

private:
	// CWindowImpl
	virtual void OnFinalMessage(HWND hWnd);

	// Helper methods
	void FadeIn(unsigned int aTimeout = 0);
	void FadeOut(unsigned int aTimeout = 0);
	void FadeInComplete();
	void FadeOutComplete();
	void MonitorWindow();

	// Win32 Callbacks
	static BOOL CALLBACK SubclassChildWindowsProc(HWND hwnd, LPARAM lParam);
	static LRESULT CALLBACK SubclassedWindowsWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// Types
	typedef std::map<HWND, WNDPROC> wnd_proc;
	typedef wnd_proc::iterator wnd_proc_itr;

private:
	// Objects
	boost::shared_ptr<boost::thread>	m_spFadeInThread;
	boost::shared_ptr<boost::thread>	m_spFadeOutThread;
	CToastWindowManager*				m_pToastWindowManager;		// Aggregated

	// Simple types
	bool								m_bMouseEnter;
	HWND								m_hWndFinal;

	// Static
	static wnd_proc				 		m_childproc;
};
