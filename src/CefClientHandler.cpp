#include "stdafx.h"
#include "CefClientHandler.h"
#include "CefPopupClientHandler.h"
#include "CefToastClientHandler.h"

// ----------------------------------------------------------------------------
CCefClientHandler::CCefClientHandler(IClientHandlerEvents* aClientHandlerEvents)
{
	SCOPE_LOG();

	m_spBrowser = NULL;
	m_pClientHandlerEvents = aClientHandlerEvents;
}

// ----------------------------------------------------------------------------
CCefClientHandler::~CCefClientHandler()
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CefRefPtr<CefBrowser> CCefClientHandler::GetBrowser()
{
	SCOPE_LOG();

	return m_spBrowser;
}

// ----------------------------------------------------------------------------
void CCefClientHandler::SetCookie(const CefString& aUrl, const CefString& aCookie)
{
	SCOPE_LOG();

	try
	{
		// Convert to "std::string" to comply with BOOST
		std::string sCookie = aCookie;

		// Parse cookie values
		CefCookie cookie;

		// Parse name and value
		boost::smatch matches;
		boost::regex expr("([^=]*)=([^;]*)", boost::regex::icase);
		boost::regex_search(sCookie, matches, expr);
		_ASSERTE(matches.size() == 3);
		CefString(&cookie.name) = matches[1].str();
		CefString(&cookie.value) = matches[2].str();

		// Parse path
		expr.assign("path=([^;]*)");
		boost::regex_search(sCookie, matches, expr);
		if (matches.size() == 2)
		{
			CefString(&cookie.path) = matches[1].str();
		}
		else
		{
			// Default
			CefString(&cookie.path).FromASCII("/");
		}

		// Parse domain
		expr.assign("domain=([^;]*)");
		boost::regex_search(sCookie, matches, expr);
		_ASSERTE(matches.size() == 2);
		CefString(&cookie.domain) = matches[1].str();

		// Parse access restriction
		cookie.httponly = (sCookie.find("httponly") != std::string::npos);

		// Parse HTTPS restriction
		cookie.secure = (sCookie.find("secure") != std::string::npos);

		// Parse expiration
		expr.assign("expires=([^;]*)");
		boost::regex_search(sCookie, matches, expr);
		if (matches.size() == 2)
		{
			cookie.has_expires = true;

			// Get UNIX timestamp
			time_t time = curl_getdate(matches[1].str().c_str(), NULL);

			// BOOST way. Year: [1900, ...], Month: [0, 11]
			//boost::posix_time::ptime ptime = boost::posix_time::from_time_t(time);
			//std::tm tm = boost::posix_time::to_tm(ptime);

			// CEF way
			cef_time_t cef_time = {0};
			int rv = cef_time_from_doublet(time, &cef_time);
			_ASSERTE(rv == 1);

			// Time
			cookie.expires.hour			= cef_time.hour;
			cookie.expires.minute		= cef_time.minute;
			cookie.expires.second		= cef_time.second;

			// Date
			cookie.expires.year			= cef_time.year;
			cookie.expires.month		= cef_time.month;
			cookie.expires.day_of_week	= cef_time.day_of_week;
			cookie.expires.day_of_month	= cef_time.day_of_month;
		}
		else
		{
			// Default
			cookie.has_expires = false;
		}

 		CefRefPtr<CefCookieManager> spCookieManager = CefCookieManager::GetGlobalManager();
 		_ASSERTE(spCookieManager.get() != NULL);

		// Set cookie on the IO thread
		CefRefPtr<CefTask> spTask = NewCefRunnableMethod(
			spCookieManager.get(), &CefCookieManager::SetCookie, aUrl, cookie);
		_ASSERTE(spTask.get() != NULL);

		bool rv = CefPostTask(TID_IO, spTask);
		_ASSERTE(rv == true);

	}
	catch (boost::regex_error& e)
	{
#ifdef DEBUG
		std::strstream ssError;
		ssError << "Error. " << e.what() << ". " << e.position();
		::OutputDebugStringA(ssError.str());
#endif
	}
	catch (const std::exception& e) 
	{
#ifdef DEBUG
		std::strstream ssError;
		ssError << "Error. " << e.what();
		::OutputDebugStringA(ssError.str());
#endif
	}
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
bool CCefClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser,
									  const CefPopupFeatures& popupFeatures,
									  CefWindowInfo& windowInfo,
									  const CefString& url,
									  CefRefPtr<CefClient>& client,
									  CefBrowserSettings& settings) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// Filter URLs. Allow "thebluebar.com", "facebook.com" oauth and CEF dev tools
	CefURLParts parts;
	bool rv = CefParseURL(url, parts);
	_ASSERTE(rv == true);

	if (parts.host.str && CefString(parts.host.str) == _T("www.thebluebar.com") &&
		parts.query.str && CefString(parts.query.str).ToString().find("toast=1") == 0)
	{
		// TODO: This is a platform dependent hack. Window size should be deducted
		//       from the HTML elements size.
		// Adjust window size and styles
		windowInfo.m_dwStyle = WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP;
		windowInfo.m_dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_COMPOSITED;

		// Assign it's own handler
		client = new CefToastClientHandler(m_pClientHandlerEvents);

		// Allow popup window
		return false;
	}
	else if (parts.host.str && CefString(parts.host.str) == _T("www.thebluebar.com") &&
			 parts.path.str && CefString(parts.path.str).ToString().find("/desktop-apps") == 0 ||
			 parts.host.str && CefString(parts.host.str) == _T("www.facebook.com") &&
			 parts.path.str && CefString(parts.path.str) == _T("/dialog/oauth"))
	{
		// TODO: This is a platform dependent hack. Window size should be deducted
		//       from the HTML elements size.
		// Adjust window size and make it non-resizable
		windowInfo.m_nHeight += 50;
		windowInfo.m_dwStyle ^= WS_THICKFRAME;
		
		// Not working.
		// CEF bug: code.google.com/p/chromiumembedded/issues/detail?id=538
		//windowInfo.m_windowName = _T("Bluebar Messenger");

		// Assign it's own handler
		client = new CCefPopupClientHandler(m_pClientHandlerEvents);

		// Allow popup window
		return false;
	}
	else if (parts.host.str && CefString(parts.host.str) == _T("devtools") && 
			 parts.path.str && CefString(parts.path.str) == _T("/devtools.html"))
	{
		// TODO: Not working. Styles get overridden after window is created.
		// ...
		// Add min/max buttons
		windowInfo.m_dwStyle |= WS_MINIMIZEBOX;
		windowInfo.m_dwStyle |= WS_MAXIMIZEBOX;
		// Make is not-topmost
		windowInfo.m_dwExStyle &= ~WS_EX_TOPMOST;

		// Assign it's own handler
		client = new CCefPopupClientHandler(m_pClientHandlerEvents);

		// Allow popup window
		return false;
	}
	else
	{
		m_pClientHandlerEvents->OnPopup(url.ToString());
	}

	// Block popup window
	return true;
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
void CCefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	AutoLock lock_scope(this);

	// Keep the main browser window
	if (!m_spBrowser.get())
	{
		m_spBrowser = browser;
	}

	// Notify window container
	m_pClientHandlerEvents->OnNewWindow(browser->GetWindowHandle());
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
bool CCefClientHandler::DoClose(CefRefPtr<CefBrowser> browser) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	if (m_spBrowser.get() &&
		m_spBrowser->GetWindowHandle() == browser->GetWindowHandle()) 
	{
		// Don't close the contained window directly. The containing window
		// should close instead.
		return true;
	}

	// A popup browser window is not contained in another window, so we can let
	// these windows close by themselves.
	return false;
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
void CCefClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	if (m_spBrowser.get() &&
		m_spBrowser->GetWindowHandle() == browser->GetWindowHandle()) 
	{
		// Free the browser pointer so that the browser can be destroyed
		m_spBrowser = NULL;
	}
}

// ----------------------------------------------------------------------------
// CefLoadHandler
void CCefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
								  CefRefPtr<CefFrame> frame,
								  int httpStatusCode) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// Notify main window
	CefString sUrl = frame->GetURL();
	if (frame->IsMain())
	{
		if (httpStatusCode < HTTP_ERROR)
		{
			m_pClientHandlerEvents->OnLoadEnd(sUrl, browser->GetWindowHandle());
		}
		else
		{
			m_pClientHandlerEvents->OnLoadError(sUrl, httpStatusCode);
		}
	}
}

// ----------------------------------------------------------------------------
// CefLoadHandler
bool CCefClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,	
									CefRefPtr<CefFrame> frame, 
									ErrorCode errorCode, 
									const CefString& failedUrl, 
									CefString& errorText)
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// Notify main window
	m_pClientHandlerEvents->OnLoadError(failedUrl, errorCode);

	// TODO: Handle errors. See 'cef_handler_errorcode_t' for supported error code values.
	//       Return 'true' if error text should be overridden by a custom error.
	// ...
	// errorText = _T("");
	
	return false;
}

// ----------------------------------------------------------------------------
// CefDisplayHandler
void CCefClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
										CefRefPtr<CefFrame> frame,
										const CefString& url) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// TODO: Decode the URI
	// ...
	std::string sUrl = (std::string)url;
	m_pClientHandlerEvents->OnAddressChange(sUrl);
}

// ----------------------------------------------------------------------------
// CefRequestHandler
bool CCefClientHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, 
											 CefRefPtr<CefRequest> request, 
											 CefString& redirectUrl, 
											 CefRefPtr<CefStreamReader>& resourceStream, 
											 CefRefPtr<CefResponse> response, 
											 int loadFlags)
{
	SCOPE_LOG();

	REQUIRE_IO_THREAD();

	std::string sUrl = request->GetURL();
	if (sUrl == "http://local/startup") 
	{
		resourceStream = GetHtmlResourceReader(IDR_HTML_STARTUP);
		_ASSERTE(resourceStream);

		response->SetMimeType("text/html");
		response->SetStatus(200);
	}

	return false; // "true" cancels loading
}

// ----------------------------------------------------------------------------
// CefMenuHandler
bool CCefClientHandler::OnBeforeMenu(CefRefPtr<CefBrowser> browser, 
									 const CefMenuInfo& menuInfo)
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// Deprecate the default context menu
	return true;
}

// ----------------------------------------------------------------------------
// CefGeolocationHandler
void CCefClientHandler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, 
													   const CefString& requesting_url, 
													   int request_id, 
													   CefRefPtr<CefGeolocationCallback> callback)
{
	SCOPE_LOG();

	// Filter URLs. Allow only "thebluebar.com"
	CefURLParts parts;
	bool rv = CefParseURL(requesting_url, parts);
	_ASSERTE(rv == true);

	if (CefString(parts.host.str) == _T("www.thebluebar.com"))
	{
		callback->Continue(true);
	}
	else
	{
		callback->Continue(false);
	}
}

// ----------------------------------------------------------------------------
bool CCefClientHandler::LoadHtmlResource(int aResourceId, DWORD &aSize, LPBYTE &aBytes) 
{
	SCOPE_LOG();

	HINSTANCE hModuleInst = _AtlBaseModule.m_hInst;
	HRSRC hRsrc = ::FindResource(hModuleInst, MAKEINTRESOURCE(aResourceId), RT_HTML);
	if (hRsrc) 
	{
		HGLOBAL hRsrcData = ::LoadResource(hModuleInst, hRsrc);
		if (hRsrcData) 
		{
			aSize = ::SizeofResource(hModuleInst, hRsrc);
			aBytes = (LPBYTE)::LockResource(hRsrcData);
			if (aSize > 0 && aBytes)
			{
				return true;
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
CefRefPtr<CefStreamReader> CCefClientHandler::GetHtmlResourceReader(int aResourceId) 
{
	SCOPE_LOG();

	DWORD nSize = 0;
	LPBYTE pBytes = NULL;
	bool bLoaded = LoadHtmlResource(aResourceId, nSize, pBytes);
	_ASSERT(bLoaded);
	if (bLoaded) 
	{
		return CefStreamReader::CreateForHandler(
			new CefByteReadHandler(pBytes, nSize, NULL));
	}
	return NULL;
}

// ----------------------------------------------------------------------------
void CCefClientHandler::HideScrollbars(CefRefPtr<CefFrame> aFrame)
{
	SCOPE_LOG();

	_ASSERTE(aFrame.get() != NULL);

	// Hide scrollbars
	CefString sScript("(function(){function a(){document.documentElement.style.overflow='hidden';}document.addEventListener('DOMContentLoaded',a,false);if(document.readyState==='complete'){document.removeEventListener('DOMContentLoaded',a,false);a();}})();");
	CefString sUrl = aFrame->GetURL();
	aFrame->ExecuteJavaScript(sScript, sUrl, 0);
}

// TODO: Implement as needed
// ...
// ----------------------------------------------------------------------------
// CefDragHandler
// bool CCefClientHandler::OnDragStart(CefRefPtr<CefBrowser> browser,
// 								 CefRefPtr<CefDragData> dragData,
// 								 DragOperationsMask mask) 
// {
//	SCOPE_LOG();
//
// 	REQUIRE_UI_THREAD();
// 
// 	// Forbid dragging of image files.
// 	if (dragData->IsFile()) 
// 	{
// 		std::string fileExt = dragData->GetFileExtension();
// 		if (fileExt == ".png" || fileExt == ".jpg" || fileExt == ".gif")
// 		{
// 			return true;
// 		}
// 	}
// 
// 	return false;
// }
// 
// ----------------------------------------------------------------------------
// CefDragHandler
// bool CCefClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
// 								 CefRefPtr<CefDragData> dragData,
// 								 DragOperationsMask mask) 
// {
//	SCOPE_LOG();
//
// 	REQUIRE_UI_THREAD();
// 
// 	// Forbid dragging of link URLs.
// 	if (dragData->IsLink())
// 	{
// 		return true;
// 	}
// 
// 	return false;
// }
