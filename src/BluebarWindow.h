#pragma once

#include "Ping.h"
#include "DockWindow.h"
#include "ToastWindow.h"
#include "TrayIconImpl.h"
#include "LayeredWindow.h"
#include "MouseEnterEvent.h"
#include "CefClientHandler.h"
#include "IWebAppCommandImpl.h"
#include "ToastWindowManager.h"
#include "IClientHandlerEvents.h"
#include "BluebarLocalSettings.h"


class CBluebarWindow :
	public CWindowImpl<CBluebarWindow>,
	public CTrayIconImpl<CBluebarWindow>,
	public CThemeImpl<CBluebarWindow>,
	public CMouseEnterEvent<CBluebarWindow>,
	public CLayeredWindow<CBluebarWindow>,
	public IClientHandlerEvents,
	public IWebAppCommandImpl,
	public INetObserverImpl
{
	// System global messages
	UINT WM_SET_COOKIES;
	UINT WM_SHOW_BLUEBAR;

public:
	// Constants
	enum 
	{ 
		BAR_HEIGHT			= 24,
		BAR_MARGIN_RIGHT	= 140,	// A horizontal margin, so we don't overlap fullscreen windows control buttons ([_][-][x])
		BAR_WND_STYLE		= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPED,
		BAR_WND_EX_STYLE	= WS_EX_PALETTEWINDOW | WS_EX_COMPOSITED
	};

	// Command IDs
	enum 
	{
		ID_NOTIFY_SHOW_WINDOW = 50001,
		ID_NOTIFY_HIDE_WINDOW,
		ID_NOTIFY_ONLINE,
		ID_NOTIFY_LOAD_ERROR
	};

	BEGIN_MSG_MAP(CBluebarWindow)
		// Messages
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
  		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnDisplayChange)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		
		// System global messages
		MESSAGE_HANDLER(WM_SET_COOKIES, OnSetCookies)
		MESSAGE_HANDLER(WM_SHOW_BLUEBAR, OnShowBluebar)

		// CTrayIconImpl
		MESSAGE_HANDLER(WM_TRAYICON, OnTrayIcon)
		
		// CMouseEnterEvent
		MESSAGE_HANDLER(WM_MOUSEENTER, OnMouseEnter)

		// Commands
		COMMAND_ID_HANDLER(ID_BLUEBAR_EXIT, OnBluebarExit)
		COMMAND_ID_HANDLER(ID_NOTIFY_SHOW_WINDOW, OnNotifyShowWindow)
		COMMAND_ID_HANDLER(ID_NOTIFY_ONLINE, OnNotifyOnline)

		// Chaining
		CHAIN_MSG_MAP(CTrayIconImpl<CBluebarWindow>)
		CHAIN_MSG_MAP(CThemeImpl<CBluebarWindow>)
		CHAIN_MSG_MAP(CMouseEnterEvent<CBluebarWindow>)
		CHAIN_MSG_MAP(CLayeredWindow<CBluebarWindow>)
	END_MSG_MAP()

public:
	CBluebarWindow(void);
	virtual ~CBluebarWindow(void);

	void FirstRun();

protected:
	// Message handlers
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSetFocus(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDisplayChange(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnActivate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMouseActivate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);

	// Custom messages
	LRESULT OnSlideWindow(UINT, WPARAM, LPARAM, BOOL&);
	
	// System global messages
	LRESULT OnSetCookies(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnShowBluebar(UINT, WPARAM, LPARAM, BOOL&);

	// CTrayIconImpl
	LRESULT OnTrayIcon(UINT, WPARAM, LPARAM, BOOL&);

	// CMouseEnterEvent
	LRESULT OnMouseEnter(UINT, WPARAM, LPARAM, BOOL&);

	// Command handlers
	LRESULT OnBluebarExit(WORD, WORD, HWND, BOOL&);
	LRESULT OnNotifyShowWindow(WORD, WORD, HWND, BOOL&);
	LRESULT OnNotifyOnline(WORD, WORD, HWND, BOOL&);

	// IClientHandlerEvents
	virtual void OnAddressChange(const std::string& aUri) OVERRIDE;
	virtual void OnLoadError(const std::string& aUri, int aErrorCode) OVERRIDE;
	virtual void OnLoadEnd(const std::string& aUri, CefWindowHandle aHwnd) OVERRIDE;
	virtual void OnNewWindow(CefWindowHandle aHwnd) OVERRIDE;
	virtual void OnNewToastWindow(CefWindowHandle aHwnd) OVERRIDE;
	virtual void OnPopup(const std::string& aUri) OVERRIDE;
	virtual void OnWindowResized(CefWindowHandle aHwnd, int aWidth, int aHeight) OVERRIDE;
	virtual void OnWindowResizedBy(CefWindowHandle aHwnd, int aX, int aY) OVERRIDE;
	virtual void OnWindowMoveTo(CefWindowHandle aHwnd, int aX, int aY) OVERRIDE;
	virtual void OnTitleChanged(CefWindowHandle aHwnd, const std::string& aTitle) OVERRIDE;
	virtual void OnWindowClosed(CefWindowHandle aHwnd) OVERRIDE;

	// IWebAppCommandImpl
	virtual void WebCmdNavigate(const std::string& aUrl) OVERRIDE;
	virtual void WebCmdCut(const CRect& aRect, DWORD aCombineMode) OVERRIDE;
	virtual void WenCmdCutPolygon(const std::vector<CPoint>& aPoints, DWORD aCombineMode) OVERRIDE;
	virtual void WebCmdDock(bool bState) OVERRIDE;
	virtual void WebCmdClose() OVERRIDE;

	// INetObserverImpl
	virtual void NotifyOnline() OVERRIDE;

private:
	// Helper methods
	void AppGetBrowserSettings(CefBrowserSettings& aSettings);
	void DisableAero();
	void InstallTrayIcon();
	void DockWindow(bool aState);
	bool IsWindowDocked();
	void CombineRegion(const CRgn& aRgn, DWORD aCombineMode);
	void ResetRegion();
	void ResetWindowPos();
	void SetWindowShadow(bool aState);
	void PinWindowOpacity(bool aState);
	void AddUrlParam(std::string& aUrl, const std::string& aName, const std::string& aValue);
	void SlideWindow(bool aState);
	void ShowWindow();

	// Threads
	void MouseEnterThread(unsigned int aTimeout);
	void SlideWindowThread();

	// Win32 Callbacks
	static BOOL CALLBACK SubclassChildWindowsProc(HWND hwnd, LPARAM lParam);
	static LRESULT CALLBACK SubclassedWindowsWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// Types
	typedef CWinTraits<BAR_WND_STYLE, BAR_WND_EX_STYLE> CAppWinTraits;
	typedef std::map<HWND, WNDPROC> wnd_proc;
	typedef wnd_proc::iterator wnd_proc_itr;

private:
	// Objects
	CDockWindow			 				m_dockWindow;
	CNetMonitor					 		m_netmon;
	boost::mutex						m_mutex;
	CToastWindowManager					m_toastWindowManager;
	CBluebarLocalSettings				m_settings;
	CefRefPtr<CCefClientHandler> 		m_spClientHandler;
	boost::shared_ptr<boost::thread>	m_spSlideThread;
	boost::shared_ptr<boost::thread>	m_spMouseEnterThread;

	// Simple types 
	HMODULE						 		m_hModuleDWMAPI;
	bool						 		m_bPinned;
	bool								m_bDocked;
	bool								m_bSlideShow;
	bool								m_bTerminating;

	// Static
	static wnd_proc				 		m_childproc;
};
