#include "stdafx.h"
#include "CefPopupClientHandler.h"

// ----------------------------------------------------------------------------
CCefPopupClientHandler::CCefPopupClientHandler(IClientHandlerEvents* aClientHandlerEvents)
{
	SCOPE_LOG();

	m_pClientHandlerEvents = aClientHandlerEvents;
}

// ----------------------------------------------------------------------------
CCefPopupClientHandler::~CCefPopupClientHandler(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
bool CCefPopupClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser,
										   const CefPopupFeatures& popupFeatures,
										   CefWindowInfo& windowInfo,
										   const CefString& url,
										   CefRefPtr<CefClient>& client,
										   CefBrowserSettings& settings) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// Block all popup windows. No second-layer of popups is allowed.
	return true;
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
void CCefPopupClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
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
void CCefPopupClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) 
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
void CCefPopupClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
									   CefRefPtr<CefFrame> frame,
									   int httpStatusCode) 
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	if (httpStatusCode < HTTP_ERROR && frame->IsMain())
	{
		// Visit DOM to get the page title
		frame->VisitDOM(this);

		// Notify main window
		CefString sUrl = frame->GetURL();
		m_pClientHandlerEvents->OnLoadEnd(sUrl, browser->GetWindowHandle());
	}
}

// ----------------------------------------------------------------------------
// CefLoadHandler
bool CCefPopupClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,	
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
void CCefPopupClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
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
// CefV8ContextHandler
void CCefPopupClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, 
											  CefRefPtr<CefFrame> frame, 
											  CefRefPtr<CefV8Context> context)
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	// Create bindings for the missing Javascript functions
	CefRefPtr<CefV8Value> spGlobal = context->GetGlobal();
	
	CefRefPtr<CefV8Value> spResizeTo = CefV8Value::CreateFunction("resizeTo", this);
	spGlobal->SetValue("resizeTo", spResizeTo, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Value> spResizeBy = CefV8Value::CreateFunction("resizeBy", this);
	spGlobal->SetValue("resizeBy", spResizeBy, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Value> spMoveTo = CefV8Value::CreateFunction("moveTo", this);
	spGlobal->SetValue("moveTo", spMoveTo, V8_PROPERTY_ATTRIBUTE_NONE);
}

// ----------------------------------------------------------------------------
// CefV8Handler
bool CCefPopupClientHandler::Execute(const CefString& name, 
									 CefRefPtr<CefV8Value> object, 
									 const CefV8ValueList& arguments, 
									 CefRefPtr<CefV8Value>& retval, 
									 CefString& exception)
{
	SCOPE_LOG();

	if (name == "resizeTo")
	{
		_ASSERTE(arguments.size() == 2);
		int nWidth = arguments[0]->GetIntValue();
		int nHeight = arguments[1]->GetIntValue();

		m_pClientHandlerEvents->OnWindowResized(m_spBrowser->GetWindowHandle(), nWidth, nHeight);

		// Return "true" if handled
		return true;
	}
	else if (name == "resizeBy")
	{
		_ASSERTE(arguments.size() == 2);
		int x = arguments[0]->GetIntValue();
		int y = arguments[1]->GetIntValue();

		m_pClientHandlerEvents->OnWindowResizedBy(m_spBrowser->GetWindowHandle(), x, y);

		// Return "true" if handled
		return true;
	} 
	else if (name == "moveTo")
	{
		_ASSERTE(arguments.size() == 2);
		int x = arguments[0]->GetIntValue();
		int y = arguments[1]->GetIntValue();

		m_pClientHandlerEvents->OnWindowMoveTo(m_spBrowser->GetWindowHandle(), x, y);

		// Return "true" if handled
		return true;
	} 

	// Return "false" if unhandled
	return false;
}

// ----------------------------------------------------------------------------
// CefDOMVisitor
void CCefPopupClientHandler::Visit(CefRefPtr<CefDOMDocument> document)
{
	SCOPE_LOG();

	REQUIRE_UI_THREAD();

	_ASSERTE(document.get());
	CefString sTitle = document->GetTitle();
	if (!sTitle.ToString().empty())
	{
		m_pClientHandlerEvents->OnTitleChanged(m_spBrowser->GetWindowHandle(), sTitle.ToString());
	}
}
