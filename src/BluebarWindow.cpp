#include "stdafx.h"
#include "BluebarWindow.h"
#include "BluebarAnalytics.h"
#include "BlueMessageBox.h"
#include "WebAppCommandParser.h"
#include "MahineUniqueId.h"
#include "BluebarUrls.h"


// For debug build only
#define CEF_DEV_TOOLS


// Constants
const CefString kWindowName("Bluebar");
const CefString kStartupUri("http://local/startup");


// Static
CBluebarWindow::wnd_proc CBluebarWindow::m_childproc;


// ----------------------------------------------------------------------------
CBluebarWindow::CBluebarWindow(void)
{
	SCOPE_LOG();

	// Register system global messages
	WM_SET_COOKIES = ::RegisterWindowMessage(_T("AEFC919F-1F4E-4da9-B987-071D7E62B6F7"));
	WM_SHOW_BLUEBAR = ::RegisterWindowMessage(_T("E5B785CA-BE97-4ee5-800C-BD48DA306FED"));

	m_hModuleDWMAPI = NULL;
	m_bPinned = false;
	m_bDocked = true;
	m_bSlideShow = false;
	m_bTerminating = false;

	// NOTE: Disables "aero" for the OS and not just our app
	//DisableAero();
}

// ----------------------------------------------------------------------------
CBluebarWindow::~CBluebarWindow(void)
{
	SCOPE_LOG();

	if (m_hModuleDWMAPI)
	{
		::FreeLibrary(m_hModuleDWMAPI);
		m_hModuleDWMAPI = NULL;
	}
}

// ----------------------------------------------------------------------------
// Returns the application browser settings based on command line arguments
void CBluebarWindow::AppGetBrowserSettings(CefBrowserSettings& aSettings) 
{
	SCOPE_LOG();

	CefRefPtr<CefCommandLine> spCommandLine = CefCommandLine::CreateCommandLine();;
	spCommandLine->InitFromString(::GetCommandLineW());

#ifdef _DEBUG

	aSettings.drag_drop_disabled						= spCommandLine->HasSwitch(cefclient::kDragDropDisabled);
	aSettings.load_drops_disabled						= spCommandLine->HasSwitch(cefclient::kLoadDropsDisabled);
	aSettings.history_disabled							= spCommandLine->HasSwitch(cefclient::kHistoryDisabled);
	aSettings.animation_frame_rate						= atoi(((std::string)spCommandLine->GetSwitchValue(cefclient::kAnimationFrameRate)).c_str());
	aSettings.remote_fonts_disabled						= spCommandLine->HasSwitch(cefclient::kRemoteFontsDisabled);
	CefString(&aSettings.default_encoding)				= spCommandLine->GetSwitchValue(cefclient::kDefaultEncoding);
	aSettings.encoding_detector_enabled					= spCommandLine->HasSwitch(cefclient::kEncodingDetectorEnabled);
	aSettings.javascript_disabled						= spCommandLine->HasSwitch(cefclient::kJavascriptDisabled);
	aSettings.javascript_open_windows_disallowed 		= spCommandLine->HasSwitch(cefclient::kJavascriptOpenWindowsDisallowed);
	aSettings.javascript_close_windows_disallowed		= spCommandLine->HasSwitch(cefclient::kJavascriptCloseWindowsDisallowed);
	aSettings.javascript_access_clipboard_disallowed	= spCommandLine->HasSwitch(cefclient::kJavascriptAccessClipboardDisallowed);
	aSettings.dom_paste_disabled						= spCommandLine->HasSwitch(cefclient::kDomPasteDisabled);
	aSettings.caret_browsing_enabled					= spCommandLine->HasSwitch(cefclient::kCaretBrowsingDisabled);
	aSettings.java_disabled								= spCommandLine->HasSwitch(cefclient::kJavaDisabled);
	aSettings.plugins_disabled							= spCommandLine->HasSwitch(cefclient::kPluginsDisabled);
	aSettings.universal_access_from_file_urls_allowed	= spCommandLine->HasSwitch(cefclient::kUniversalAccessFromFileUrlsAllowed);
	aSettings.file_access_from_file_urls_allowed 		= spCommandLine->HasSwitch(cefclient::kFileAccessFromFileUrlsAllowed);
	aSettings.web_security_disabled						= spCommandLine->HasSwitch(cefclient::kWebSecurityDisabled);
	aSettings.xss_auditor_enabled						= spCommandLine->HasSwitch(cefclient::kXssAuditorEnabled);
	aSettings.image_load_disabled						= spCommandLine->HasSwitch(cefclient::kImageLoadingDisabled);
	aSettings.shrink_standalone_images_to_fit			= spCommandLine->HasSwitch(cefclient::kShrinkStandaloneImagesToFit);
	aSettings.site_specific_quirks_disabled				= spCommandLine->HasSwitch(cefclient::kSiteSpecificQuirksDisabled);
	aSettings.text_area_resize_disabled					= spCommandLine->HasSwitch(cefclient::kTextAreaResizeDisabled);
	aSettings.page_cache_disabled						= spCommandLine->HasSwitch(cefclient::kPageCacheDisabled);
	aSettings.tab_to_links_disabled						= spCommandLine->HasSwitch(cefclient::kTabToLinksDisabled);
	aSettings.hyperlink_auditing_disabled				= spCommandLine->HasSwitch(cefclient::kHyperlinkAuditingDisabled);
	aSettings.user_style_sheet_enabled					= spCommandLine->HasSwitch(cefclient::kUserStyleSheetEnabled);
	CefString(&aSettings.user_style_sheet_location)		= spCommandLine->GetSwitchValue(cefclient::kUserStyleSheetLocation);
	aSettings.author_and_user_styles_disabled			= spCommandLine->HasSwitch(cefclient::kAuthorAndUserStylesDisabled);
	aSettings.local_storage_disabled					= spCommandLine->HasSwitch(cefclient::kLocalStorageDisabled);
	aSettings.databases_disabled						= spCommandLine->HasSwitch(cefclient::kDatabasesDisabled);
	aSettings.application_cache_disabled				= spCommandLine->HasSwitch(cefclient::kApplicationCacheDisabled);
	aSettings.webgl_disabled							= spCommandLine->HasSwitch(cefclient::kWebglDisabled);
	aSettings.accelerated_compositing_enabled			= spCommandLine->HasSwitch(cefclient::kAcceleratedCompositingEnabled);
	aSettings.accelerated_layers_disabled				= spCommandLine->HasSwitch(cefclient::kAcceleratedLayersDisabled);
	aSettings.accelerated_video_disabled				= spCommandLine->HasSwitch(cefclient::kAcceleratedVideoDisabled);
	aSettings.accelerated_2d_canvas_disabled			= spCommandLine->HasSwitch(cefclient::kAcceledated2dCanvasDisabled);
	aSettings.accelerated_painting_disabled				= spCommandLine->HasSwitch(cefclient::kAcceleratedPaintingDisabled);
	aSettings.accelerated_filters_disabled				= spCommandLine->HasSwitch(cefclient::kAcceleratedFiltersDisabled);
	aSettings.accelerated_plugins_disabled				= spCommandLine->HasSwitch(cefclient::kAcceleratedPluginsDisabled);
	aSettings.developer_tools_disabled					= spCommandLine->HasSwitch(cefclient::kDeveloperToolsDisabled);
	aSettings.fullscreen_enabled						= spCommandLine->HasSwitch(cefclient::kFullscreenEnabled);

#else // !_DEBUG

	aSettings.drag_drop_disabled						= false;
	aSettings.load_drops_disabled						= false;
	aSettings.history_disabled							= false;
	aSettings.animation_frame_rate						= 0;
	aSettings.remote_fonts_disabled						= false;
	CefString(&aSettings.default_encoding)				= "";
	aSettings.encoding_detector_enabled					= false;
	aSettings.javascript_disabled						= false;
	aSettings.javascript_open_windows_disallowed 		= false;
	aSettings.javascript_close_windows_disallowed		= false;
	aSettings.javascript_access_clipboard_disallowed	= false;
	aSettings.dom_paste_disabled						= false;
	aSettings.caret_browsing_enabled					= false;
	aSettings.java_disabled								= false;
	aSettings.plugins_disabled							= false;
	aSettings.universal_access_from_file_urls_allowed	= false;
	aSettings.file_access_from_file_urls_allowed 		= false;
	aSettings.web_security_disabled						= false;
	aSettings.xss_auditor_enabled						= false;
	aSettings.image_load_disabled						= false;
	aSettings.shrink_standalone_images_to_fit			= false;
	aSettings.site_specific_quirks_disabled				= false;
	aSettings.text_area_resize_disabled					= false;
	aSettings.page_cache_disabled						= false;
	aSettings.tab_to_links_disabled						= false;
	aSettings.hyperlink_auditing_disabled				= false;
	aSettings.user_style_sheet_enabled					= false;
	CefString(&aSettings.user_style_sheet_location)		= "";
	aSettings.author_and_user_styles_disabled			= false;
	aSettings.local_storage_disabled					= false;
	aSettings.databases_disabled						= false;
	aSettings.application_cache_disabled				= false;
	aSettings.webgl_disabled							= false;
	aSettings.accelerated_compositing_enabled			= false;
	aSettings.accelerated_layers_disabled				= false;
	aSettings.accelerated_video_disabled				= false;
	aSettings.accelerated_2d_canvas_disabled			= false;
	aSettings.accelerated_painting_disabled				= false;
	aSettings.accelerated_filters_disabled				= false;
	aSettings.accelerated_plugins_disabled				= false;
	aSettings.developer_tools_disabled					= false;
	aSettings.fullscreen_enabled						= false;

#endif // !_DEBUG

	// Always disable JAVA
	aSettings.java_disabled = true;

	// Workaround. Set custom background color to avoid [default] white flashes.
	// 'aHRtbCxib2R5e2JhY2tncm91bmQtY29sb3I6IzNiNTk5ODtjb2xvcjojM2I1OTk4fQ==' is base64 encoding of 'html,body{background-color:#3b5998;color:#3b5998}'
	LPCSTR strCss = "data:text/css;charset=utf-8;base64,aHRtbCxib2R5e2JhY2tncm91bmQtY29sb3I6IzNiNTk5ODtjb2xvcjojM2I1OTk4fQ==";
	CefString(&aSettings.user_style_sheet_location).FromASCII(strCss);
	aSettings.user_style_sheet_enabled					= true;
	aSettings.universal_access_from_file_urls_allowed	= true;
	aSettings.file_access_from_file_urls_allowed		= true;

	// TODO: This is a patch for some warnings that are being generated
	//		 by the Facebook SDK. This presents a security hole.
	//		 Please review and find a better solution.
	// Allow XSS
	//aSettings.web_security_disabled						= true;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	// Init secondary controls
	InstallTrayIcon();

	// (CWindow) Set window styles
	SetWindowLong(GWL_STYLE, CAppWinTraits::GetWndStyle(0));
	SetWindowLong(GWL_EXSTYLE, CAppWinTraits::GetWndExStyle(0));

	// (CWindow) Set window caption
	SetWindowText(kWindowName.ToWString().c_str());

	// Init window size, position and docking state
	ResetWindowPos();
	ResetRegion();
	DockWindow(true);

	// Init window info to the defaults for a child window
	RECT rcClient = {0};
	GetClientRect(&rcClient);
	CefWindowInfo info;
	info.SetAsChild(m_hWnd, rcClient);
	info.SetTransparentPainting(TRUE);

	// Populate the settings based on command line arguments
	CefBrowserSettings settings;
	AppGetBrowserSettings(settings);

	// Create a handler
	m_spClientHandler = new CCefClientHandler(this);

	// Create a new child browser window
	CefBrowser::CreateBrowserSync(info, m_spClientHandler.get(), kStartupUri, settings);

#if defined _DEBUG && defined CEF_DEV_TOOLS
	// Show browser development tools
	m_spClientHandler->GetBrowser()->ShowDevTools();
#endif

	// Subclass child CEF windows
	::EnumChildWindows(m_hWnd, SubclassChildWindowsProc, NULL);

	// Log event
	BluebarAnalytics::LogScreenResolution();

	bHandled = FALSE;
	return 0;
}

// ----------------------------------------------------------------------------
// Subclasses child CEF windows
/*static*/ BOOL CALLBACK CBluebarWindow::SubclassChildWindowsProc(HWND hwnd, LPARAM lParam)
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

	// TODO: Subclassing "cefbrowserwindow" is not necessary. Verify and remove.
	// ...
	if (sClassName == _T("cefbrowserwindow") || sClassName == _T("webviewhost"))
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
/*static*/ LRESULT CALLBACK CBluebarWindow::SubclassedWindowsWindowProc(
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
LRESULT CBluebarWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	if (m_spClientHandler.get()) 
	{
		// Browser hwnd may not be valid yes since it's created asynchronously
		CefWindowHandle hwndBrowser = m_spClientHandler->GetBrowser()->GetWindowHandle();
		if (hwndBrowser)
		{
			// Resize the browser window to match the new frame window size
			CRect rc;
			GetClientRect(&rc);

			HDWP hdwp = ::BeginDeferWindowPos(1);
			_ASSERTE(hdwp);

			hdwp = ::DeferWindowPos(hdwp, hwndBrowser, NULL,
				rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
			_ASSERTE(hdwp);

			// If "DeferWindowPos" failed, abandon the operation - don't call "EndDeferWindowPos":
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms632681(v=vs.85).aspx
			if (!hdwp) return 1;

			// Update position and size of CEF window
			::EndDeferWindowPos(hdwp);

			bHandled = TRUE;
			return 0;
		}
	}
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	if (m_spClientHandler.get() && m_spClientHandler->GetBrowser()) 
	{
		if (m_spClientHandler->GetBrowser()->GetWindowHandle()) 
		{
			// Don't erase the background if the browser window has been loaded
			// (this avoids flashing)
			return 0;
			bHandled = TRUE;
		}
	}
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	if (m_spClientHandler.get()) 
	{
		// Browser hwnd may not be valid yes since it's created asynchronously
		CefWindowHandle hwndBrowser = m_spClientHandler->GetBrowser()->GetWindowHandle();
		if (hwndBrowser)
		{
			// Forward this message to the CEF browser window
			::PostMessage(hwndBrowser, WM_SETFOCUS, wParam, lParam);

			bHandled = TRUE;
			return 0;
		}
	}
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnDisplayChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	ResetWindowPos();

	// "ResetRegion" should be called in addition to web-app call so it's able to
	// support cases where web-app is not loaded yet (desktop loader, index.php etc).
	ResetRegion();

	// Log event
	BluebarAnalytics::LogScreenResolution();

	bHandled = TRUE;
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam); 

	CRect rc;
	GetWindowRect(&rc);

	// Discard all input on window area that overlaps Windows window controls on the x axis
	// TODO: "BAR_MARGIN_RIGHT" depends on screen DPI, thus it should be computed programmatically
	if (xPos > rc.right - BAR_MARGIN_RIGHT)
	{
		// TODO: Changing cursor position fails when a window, that belongs to an elevated
		//		 process, has focus.
		//		 Note that this process is not elevated.
		// ...

		// Move mouse cursor out of this window
		BOOL rv = ::SetCursorPos(xPos, rc.top + BAR_HEIGHT);
		_ASSERTE(rv);
	}
	else
	{
		// If window is not hidden and it's not docked, wait for a while before
		// making UI responsive (user may have accidentally moved the mouse cursor
		// to the top of the screen).
		unsigned int nTimeout = 0;
		if (!m_settings.hidden && !IsWindowDocked())
		{
			nTimeout = 500;
		}

		// Check if thread exists
		if (!m_spMouseEnterThread.get())
		{
			m_spMouseEnterThread.reset(new boost::thread(boost::bind(&CBluebarWindow::MouseEnterThread, this, nTimeout)));
		}
		else
		{
			// Check if thread is running
			if (!m_spMouseEnterThread->timed_join(boost::posix_time::milliseconds(nTimeout)))
			{
				// Stop thread
				m_spMouseEnterThread->interrupt();
				m_spMouseEnterThread->join();
			}

			m_spMouseEnterThread.reset(new boost::thread(boost::bind(&CBluebarWindow::MouseEnterThread, this, nTimeout)));
 		}
	}

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	// Check if thread is running
	if (m_spMouseEnterThread.get() &&
		!m_spMouseEnterThread->timed_join(boost::posix_time::milliseconds(0)))
	{
		// Stop thread
		m_spMouseEnterThread->interrupt();
		m_spMouseEnterThread->join();
	}

	POINT pt = {0};
	::GetCursorPos(&pt);

	RECT rc = {0};
	::GetWindowRect(m_hWnd, &rc);

	// Ensure mouse cursor actually left the window area
	if (pt.y < rc.top || pt.y > rc.top + BAR_HEIGHT)
	{
		if (!m_bPinned)
		{
			if (!IsWindowDocked())
			{
				// CLayeredWindow
				SetWindowOpacity(1);

				SlideWindow(false);
			}
			else
			{
				// CLayeredWindow
				SetWindowOpacity(60);
			}
		}
	}

	bHandled = FALSE;
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	switch (wParam)
	{
	case WA_INACTIVE:
		// "lParam" is the handle to the window being activated
		if (!::IsChild(m_hWnd, (HWND)lParam) &&
			m_hWnd != ::GetParent((HWND)lParam))
		{
			if (m_dockWindow.IsWindow())
			{
				if (m_dockWindow.m_hWnd != (HWND)lParam)
				{
					PinWindowOpacity(false);
				}
			}
			else
			{
				PinWindowOpacity(false);
			}
		}
		break;
	}

	bHandled = TRUE;
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	PinWindowOpacity(true);

	bHandled = TRUE;
	return MA_ACTIVATE;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	m_bTerminating = true;

	// Signal threads to terminate
	if (m_spSlideThread.get())
	{
		m_spSlideThread->interrupt();
	}

	if (m_spMouseEnterThread.get())
	{
		m_spMouseEnterThread->interrupt();
	}

	// Wait for threads to terminate
	if (m_spSlideThread.get())
	{
		m_spSlideThread->join();
	}

	if (m_spMouseEnterThread.get())
	{
		m_spMouseEnterThread->join();
	}

	// Notify client handler that we are about to terminate
	if (m_spClientHandler.get()) 
	{
		m_spClientHandler->GetBrowser()->ParentWindowWillClose();
	}

	// Undock this window
	DockWindow(false);

	// Destroy this window
	BOOL rv = DestroyWindow();
	_ASSERTE(rv);
	if (rv && !IsWindow())
	{
		// Patch for ATL bug
		m_hWnd = NULL;
	}

	bHandled = TRUE;
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	// The frame window has exited
	::PostQuitMessage(0);

	bHandled = TRUE;
	return 0;
}

// ----------------------------------------------------------------------------
// Command handler
LRESULT CBluebarWindow::OnBluebarExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SCOPE_LOG();

	// Log event
	BluebarAnalytics::LogExitTray();

	// Close
	PostMessage(WM_CLOSE, 0, 0);

	return 0;
}

// ----------------------------------------------------------------------------
// Command handler
LRESULT CBluebarWindow::OnNotifyShowWindow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SCOPE_LOG();

	// Show the window
	static_cast<CWindowImpl*>(this)->ShowWindow(SW_SHOW);
	Invalidate();
	UpdateWindow();

	return 0;
}

// ----------------------------------------------------------------------------
// Command handler
LRESULT CBluebarWindow::OnNotifyOnline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SCOPE_LOG();

	CefString sCurUri = m_spClientHandler->GetBrowser()->GetMainFrame()->GetURL();
	if (sCurUri == kStartupUri)
	{
		m_netmon.Unsubscribe(this);

		std::string sUri;
		sUri = sUri + BluebarUrls::kHomeUri + m_settings.affiliate_id + "/";
		AddUrlParam(sUri, "affiliate_id", m_settings.affiliate_id);

		// Add machine ID to home URL
		std::string sMachineUniqueId;
		Machine::GenerateMachineUniqueIdHash(sMachineUniqueId);
		AddUrlParam(sUri, "machine_id", sMachineUniqueId);

		// Add file version to home URL
		TCHAR szModulePath[MAX_PATH] = {0};
		DWORD rv = ::GetModuleFileName(_AtlBaseModule.m_hInst, szModulePath, MAX_PATH - 2);
		_ASSERTE(rv);
		CefString sVersion = Module::GetModuleVersion(szModulePath);
		AddUrlParam(sUri, "v", sVersion.ToString());

		if (m_settings.after_install)
		{
			m_settings.after_install = false;
			AddUrlParam(sUri, "after_install", "1");
		}

		if (m_settings.after_update)
		{
			m_settings.after_update = false;
			AddUrlParam(sUri, "after_update", "1");
		}

		// IWebAppCommandImpl
		WebCmdNavigate(sUri);
	}

	return 0;
}

// ----------------------------------------------------------------------------
// CTrayIconImpl
LRESULT CBluebarWindow::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	switch (LOWORD(lParam))
	{
	case WM_RBUTTONUP:
		{
			// Delegate to parent (CTrayIconImpl) who shows the default tray menu
			return static_cast<CTrayIconImpl<CBluebarWindow>*>(this)->OnTrayIcon(uMsg, wParam, lParam, bHandled);
		}
		break;

	case WM_LBUTTONUP:
		{
			ShowWindow();
		}
		break;
	}
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnSetCookies(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	const char* szUrl = reinterpret_cast<const char*>(wParam);
 	_ASSERTE(szUrl != NULL);

	const char* szCookieStr = reinterpret_cast<const char*>(lParam);
	_ASSERTE(szCookieStr != NULL);

	if (szUrl && szCookieStr)
	{
		// Parse cookies string
		std::vector<std::string> vecCookies;
		boost::algorithm::split_regex(vecCookies, std::string(szCookieStr), boost::regex("Set-Cookie: "));
		BOOST_FOREACH(std::string sCookie, vecCookies)
		{
			m_spClientHandler->SetCookie(szUrl, sCookie);
		}
		
		bHandled = TRUE;
		return 0;
	}

	bHandled = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
LRESULT CBluebarWindow::OnShowBluebar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SCOPE_LOG();

	ShowWindow();

	// Notify that app is already running
	CBlueMessageBox mb("Bluebar", "Bluebar is at the top of your screen.");
	mb.DoModal(m_hWnd);

	bHandled = TRUE;
	return 0;
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnAddressChange(const std::string& aUri)
{
	SCOPE_LOG();

	CWebAppCommandParser commandparser(aUri, this);
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
// Error could be either an HTTP error or a CEF error. Either way,
// handling every discreet error may be too much. Instead, we subscribe
// to network monitor and when network is available, everything is reset.
void CBluebarWindow::OnLoadError(const std::string& aUri, int aErrorCode)
{
	SCOPE_LOG();

	// Reset
	m_spClientHandler->GetBrowser()->GetMainFrame()->LoadURL(kStartupUri);

	// Wait for network
	if (!m_netmon.IsOnline())
	{
		m_netmon.Subscribe(this);
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnLoadEnd(const std::string& aUri, CefWindowHandle aHwnd)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd) && m_spClientHandler.get())
	{
		if (m_spClientHandler->GetBrowser()->GetWindowHandle() == aHwnd)
		{
			// Main window
			if (kStartupUri == aUri)
			{
				// Wait for network to be online before attempting to load the web-app
				m_netmon.Subscribe(this);
			}
			else
			{
				SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			PostMessage(WM_COMMAND, ID_NOTIFY_SHOW_WINDOW);
		}
		else
		{
			// Popup window
			::SetWindowPos(aHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnNewWindow(CefWindowHandle aHwnd)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		if (!m_netmon.IsOnline())
		{
			// Close the new child window
			if (m_hWnd != ::GetAncestor(aHwnd, GA_ROOT))
			{
				::DestroyWindow(aHwnd);
			}

			// IWebAppCommandImpl
			WebCmdNavigate(kStartupUri);

			m_netmon.Subscribe(this);
		}
		else
		{
			// Set small icon
			HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.m_hInst, 
				MAKEINTRESOURCE(IDI_BLUEBAR), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), 
				::GetSystemMetrics(SM_CYSMICON), 0);
			_ASSERTE(hIcon != NULL);
			::SendMessage(aHwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

			// Set large icon
			hIcon = (HICON)::LoadImage(_AtlBaseModule.m_hInst, 
				MAKEINTRESOURCE(IDI_BLUEBAR), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), 
				::GetSystemMetrics(SM_CYICON), 0);
			_ASSERTE(hIcon != NULL);
			::SendMessage(aHwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			// Remove minimize and maximize buttons
			DWORD nStyles = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			::SetWindowLong(aHwnd, GWL_STYLE, ::GetWindowLong(aHwnd, GWL_STYLE) & ~nStyles);

			// Set focus
			::SetFocus(aHwnd);
		}
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnNewToastWindow(CefWindowHandle aHwnd)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		m_toastWindowManager.AddWindow(aHwnd);
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnPopup(const std::string& aUri)
{
	SCOPE_LOG();

	// Open link in default browser
	::ShellExecuteA(NULL, "open", aUri.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnWindowResized(CefWindowHandle aHwnd, int aWidth, int aHeight)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		BOOL rv = ::SetWindowPos(aHwnd, HWND_TOP, 0, 0, aWidth, aHeight, SWP_NOMOVE);
		_ASSERTE(rv);
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnWindowResizedBy(CefWindowHandle aHwnd, int aX, int aY)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		CRect rc;
		::GetWindowRect(aHwnd, &rc);
		::SetWindowPos(aHwnd, HWND_TOP, 0, 0, rc.Width() + aX, rc.Height() + aY, SWP_NOMOVE);
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnWindowMoveTo(CefWindowHandle aHwnd, int aX, int aY)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		::SetWindowPos(aHwnd, HWND_TOP, aX, aY, 0, 0, SWP_NOSIZE);
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnTitleChanged(CefWindowHandle aHwnd, const std::string& aTitle)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		// Set caption
		::SetWindowTextA(aHwnd, aTitle.c_str());
	}
}

// ----------------------------------------------------------------------------
// IClientHandlerEvents
void CBluebarWindow::OnWindowClosed(CefWindowHandle aHwnd)
{
	SCOPE_LOG();

	_ASSERTE(::IsWindow(aHwnd));
	if (::IsWindow(aHwnd))
	{
		::PostMessage(aHwnd, WM_CLOSE, 0, 0);
	}
}

// ----------------------------------------------------------------------------
// IWebAppCommandImpl
void CBluebarWindow::WebCmdNavigate(const std::string& aUrl)
{
	SCOPE_LOG();

	m_spClientHandler->GetBrowser()->GetMainFrame()->LoadURL(aUrl);
}

// ----------------------------------------------------------------------------
// IWebAppCommandImpl
void CBluebarWindow::WebCmdCut(const CRect& aRect, DWORD aCombineMode)
{
	SCOPE_LOG();

	// All zeros region means restore original state (show only the bar)
	if (0 == aRect.left  &&
		0 == aRect.top   &&
		0 == aRect.right &&
		0 == aRect.bottom)
	{
		ResetRegion();
	}
	else
	{
		// Create region
		CRgn rgn;
		rgn.CreateRoundRectRgn(aRect.left, aRect.top, 
			aRect.right + 1, aRect.bottom + 1, 3, 3);
		_ASSERTE(rgn.m_hRgn);

		CombineRegion(rgn, aCombineMode);
	}
}

// ----------------------------------------------------------------------------
// IWebAppCommandImpl
void CBluebarWindow::WenCmdCutPolygon(const std::vector<CPoint>& aPoints, DWORD aCombineMode)
{
	SCOPE_LOG();

	// All zeros region means restore original state (show only the bar)
	if (aPoints.size() < 3)
	{
		ResetRegion();
	}
	else
	{
		// Create region
		CRgn rgn;
		rgn.CreatePolygonRgn((POINT*)&aPoints.front(), aPoints.size(), ALTERNATE);
		_ASSERTE(rgn.m_hRgn);

		CombineRegion(rgn, aCombineMode);
	}
}

// ----------------------------------------------------------------------------
// IWebAppCommandImpl
void CBluebarWindow::WebCmdDock(bool bState)
{
	SCOPE_LOG();

	m_bDocked = bState;
	DockWindow(bState);
}

// ----------------------------------------------------------------------------
// IWebAppCommandImpl
void CBluebarWindow::WebCmdClose()
{
	SCOPE_LOG();

	// Store hidden state
	m_settings.hidden = true;

	DockWindow(false);

	BOOL rv = static_cast<CWindowImpl*>(this)->ShowWindow(SW_HIDE);
	_ASSERTE(rv);
}

// ----------------------------------------------------------------------------
void CBluebarWindow::CombineRegion(const CRgn& aRgn, DWORD aCombineMode)
{
	SCOPE_LOG();

	// Combine regions
	CRgn rgn;
	rgn.CreateRectRgn(0, 0, 0, 0);
	_ASSERTE(rgn.m_hRgn);
	int rv = ::GetWindowRgn(m_hWnd, rgn);
	_ASSERTE(rv);

	rv = rgn.CombineRgn(rgn, aRgn, aCombineMode);
	_ASSERTE(rv);
	_ASSERTE(rv != NULLREGION);

	// Pass ownership of the region handle to the system (do not make any further 
	// function calls with this region handle)
	rv = ::SetWindowRgn(m_hWnd, rgn.Detach(), TRUE);
	_ASSERTE(rv);
}

// ----------------------------------------------------------------------------
// NOTE: Aero breaks "SetLayeredWindowAttributes" API
void CBluebarWindow::DisableAero() 
{
	SCOPE_LOG();

	typedef HRESULT (WINAPI *pfnDwmEnableComposition)(__in UINT uCompositionAction);

	m_hModuleDWMAPI = ::LoadLibrary(_T("dwmapi.dll"));
	if (m_hModuleDWMAPI)
	{
		pfnDwmEnableComposition DwmEnableComposition;
		DwmEnableComposition = (pfnDwmEnableComposition)::GetProcAddress(m_hModuleDWMAPI, "DwmEnableComposition");
		_ASSERTE(DwmEnableComposition);
		if (DwmEnableComposition)
		{
			HRESULT hr = DwmEnableComposition(0); // #define DWM_EC_DISABLECOMPOSITION 0
			_ASSERTE(SUCCEEDED(hr));
		}
	}
}

// ----------------------------------------------------------------------------
void CBluebarWindow::InstallTrayIcon()
{
	SCOPE_LOG();
	
#ifdef _DEBUG
	// Get small icon
	HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.m_hInst, MAKEINTRESOURCE(IDI_BLUEBAR_DISABLED), 
	IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	_ASSERTE(hIcon);
#else
	// Get small icon
	HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.m_hInst, MAKEINTRESOURCE(IDI_BLUEBAR), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	_ASSERTE(hIcon);
#endif

	// (CTrayIconImpl) Install the tray icon
	bool rv = InstallIcon(kWindowName.ToWString().c_str(), hIcon, IDR_MENU_TRAY);
	_ASSERTE(rv);
}

// ----------------------------------------------------------------------------
void CBluebarWindow::DockWindow(bool aState)
{
	SCOPE_LOG();

	bool bShouldDock = (!m_settings.hidden && aState);
	if (bShouldDock)
	{
		if (!IsWindowDocked())
		{
			// The docking is performed by another window, because this window
			// is actually the size of the whole screen and docking it will push
			// everything else off the screen.
			RECT rcPlaceholder = {0};
			BOOL rv = GetClientRect(&rcPlaceholder);
			_ASSERTE(rv);

			rcPlaceholder.bottom = rcPlaceholder.top + BAR_HEIGHT;

			HWND hwndPlaceholder = m_dockWindow.Create(::GetDesktopWindow(), rcPlaceholder);
			_ASSERTE(hwndPlaceholder);

			CRect rc;
			m_dockWindow.GetWindowRect(&rc);
			SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
	else
	{
		if (IsWindowDocked())
		{
			BOOL rv = m_dockWindow.DestroyWindow();
			_ASSERTE(rv);
			if (rv && !m_dockWindow.IsWindow())
			{
				// Patch for ATL bug
				m_dockWindow.m_hWnd = NULL;
			}
		}
	}
}

// ----------------------------------------------------------------------------
bool CBluebarWindow::IsWindowDocked()
{
	SCOPE_LOG();

	return (m_dockWindow.IsWindow() == TRUE);
}

// ----------------------------------------------------------------------------
void CBluebarWindow::ResetRegion()
{
	SCOPE_LOG();

	CRect rc;
	GetClientRect(&rc);

	CRgn rgn;
	rgn.CreateRectRgn(rc.left, rc.top, rc.right, BAR_HEIGHT);
	_ASSERTE(rgn.m_hRgn);

	// Pass ownership of the region handle to the system (do not make any further 
	// function calls with this region handle)
	int rv = ::SetWindowRgn(m_hWnd, rgn.Detach(), TRUE);
	_ASSERTE(rv);
}

// ----------------------------------------------------------------------------
void CBluebarWindow::ResetWindowPos()
{
	SCOPE_LOG();

	// Hidden takes precedence
	if (m_settings.hidden) return;

	// Get the working area that is not obscured by the taskbar
	CRect rcWorkArea;
	BOOL rv = ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	_ASSERTE(rv);

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	if (rcWindow.Height())
	{
		rcWorkArea.top = rcWindow.top;
	}

	DWORD nExStyle = GetExStyle();
	HWND hwndPos = (nExStyle & WS_EX_TOPMOST) ? HWND_TOPMOST : HWND_TOP;
	SetWindowPos(hwndPos, &rcWorkArea, 0);
 	UpdateWindow();
}

// ----------------------------------------------------------------------------
void CBluebarWindow::SetWindowShadow(bool aState)
{
	SCOPE_LOG();

	if (::IsWindow(m_hWnd))
	{
		ULONG_PTR nClassStyle = ::GetClassLongPtr(m_hWnd, GCL_STYLE);
		if (aState && (nClassStyle ^ CS_DROPSHADOW))
		{
			::SetClassLongPtr(m_hWnd, GCL_STYLE, nClassStyle | CS_DROPSHADOW);
			if (IsWindowVisible())
			{
				static_cast<CWindowImpl*>(this)->ShowWindow(SW_HIDE);
				static_cast<CWindowImpl*>(this)->ShowWindow(SW_SHOW);
			}
		}
		else if (!aState && (nClassStyle & CS_DROPSHADOW))
		{
			::SetClassLongPtr(m_hWnd, GCL_STYLE, nClassStyle ^ CS_DROPSHADOW);
			if (IsWindowVisible())
			{
				static_cast<CWindowImpl*>(this)->ShowWindow(SW_HIDE);
				static_cast<CWindowImpl*>(this)->ShowWindow(SW_SHOW);
			}
		}
	}
}

// ----------------------------------------------------------------------------
// INetObserverImpl
void CBluebarWindow::NotifyOnline()
{
	SCOPE_LOG();

	PostMessage(WM_COMMAND, ID_NOTIFY_ONLINE);
}

// ----------------------------------------------------------------------------
void CBluebarWindow::PinWindowOpacity(bool aState)
{
	SCOPE_LOG();

	if (aState)
	{
		m_bPinned = true;

		// CLayeredWindow
		SetWindowOpacity(WND_OPAQUE);

		if (!IsWindowDocked())
		{
			SlideWindow(true);
		}
	}
	else
	{
		m_bPinned = false;

		if (!IsWindowDocked())
		{
			// CLayeredWindow
			SetWindowOpacity(1);

			SlideWindow(false);
		}
		else
		{
			// CLayeredWindow
			SetWindowOpacity(60);
		}
	}
}

// ----------------------------------------------------------------------------
void CBluebarWindow::AddUrlParam(std::string& aUrl, const std::string& aName, const std::string& aValue)
{
	SCOPE_LOG();

	// Get the path
	std::string sUrl;
	std::string::size_type nPos = aUrl.find("?");
	std::string sDelim("&");
	if (nPos == aUrl.npos)
	{
		sDelim = "?";
	}
	nPos = aUrl.find("#");
	if (nPos != aUrl.npos)
	{
		sUrl = aUrl.substr(0, nPos) + sDelim + aName + "=" + aValue + aUrl.substr(nPos);
	}
	else
	{
		sUrl = aUrl + sDelim + aName + "=" + aValue;
	}

	// Out param
	aUrl = sUrl;
}

// ----------------------------------------------------------------------------
void CBluebarWindow::SlideWindow(bool aState)
{
	SCOPE_LOG();

	m_bSlideShow = aState;

	// Lazy init
	if (!m_spSlideThread.get())
	{
		m_spSlideThread.reset(
			new boost::thread(boost::bind(&CBluebarWindow::SlideWindowThread, this)));
	}
}

// ----------------------------------------------------------------------------
void CBluebarWindow::MouseEnterThread(unsigned int aTimeout)
{
	SCOPE_LOG();

	try
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(aTimeout));

		// CLayeredWindow
		SetWindowOpacity(WND_OPAQUE);

		if (!m_bPinned && !IsWindowDocked())
		{
			SlideWindow(true);
		}
		else
		{
			// Window can get stuck in the middle of sliding animation (BUG).
			// Reset window position to ensure it's at the right place.
			ResetWindowPos();
		}
	}
	catch (boost::thread_interrupted const&)
	{
		// Do nothing
	}
}

// ----------------------------------------------------------------------------
void CBluebarWindow::SlideWindowThread()
{
	SCOPE_LOG();

	// Initial state
	static bool bShow = !m_bSlideShow;

	static enum
	{
		HIDE_TICKS = 200,
		SHOW_TICKS = 100,
		HIDDEN_BAR_HEIGHT = 1
	};

	while (!m_bTerminating)
	{
		if (bShow != m_bSlideShow)
		{
			{
				// Scope lock
				boost::lock_guard<boost::mutex> guard(m_mutex);
				if (bShow != m_bSlideShow)
				{
					bShow = m_bSlideShow;
				}
				else
				{
					continue;
				}
			}

			int dy = HIDDEN_BAR_HEIGHT - BAR_HEIGHT;
			if (bShow)
			{
				dy = - dy;
			}

			HANDLE hThread = ::GetCurrentThread();
			int nThreadPriority = ::GetThreadPriority(hThread);
			::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

			CRect rc;
			GetWindowRect(&rc);

			int dt = bShow ? SHOW_TICKS : HIDE_TICKS;
			int t0 = ::GetTickCount();
			int t = 0, x = 0, y = 0;
			while ((t = ::GetTickCount()) < t0 + dt && 
					y >= (HIDDEN_BAR_HEIGHT - BAR_HEIGHT) && y <= 0 &&	// y: [-23, 0]
					bShow == m_bSlideShow &&							// Sliding direction
					!m_bTerminating)									// Thread is terminating
			{
				x = rc.left;
				y = rc.top + dy * (t - t0) / dt;

				// Update position
				SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

				if (bShow)
				{
					UpdateWindow();
				}
				else
				{
					::UpdateWindow(GetDesktopWindow());
				}
			}
			::SetThreadPriority(hThread, nThreadPriority);

			// Set final position
			SetWindowPos(NULL, rc.left, (bShow ? 0 : dy), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}

		try
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
		catch (boost::thread_interrupted const&)
		{
			// Do nothing
		}
	}
}

// ----------------------------------------------------------------------------
void CBluebarWindow::ShowWindow()
{
	SCOPE_LOG();

	// TODO: This code piece is bad. See "ResetWindowPos" and it's references
	//       to workout a better solution.
	bool bIsHidden = m_settings.hidden;
	m_settings.hidden = false;
	if (bIsHidden)
	{
		ResetWindowPos();
	}

	// Show window and move it to the foreground
	static_cast<CWindowImpl*>(this)->ShowWindow(SW_SHOW);
	::SetForegroundWindow(m_hWnd);
	SlideWindow(true);

	// CLayeredWindow
	SetWindowOpacity(WND_OPAQUE);

	if (m_bDocked)
	{
		DockWindow(true);
	}
	else
	{
		PinWindowOpacity(true);
	}
}
