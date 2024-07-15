#include "stdafx.h"
#include "CefToastClientHandler.h"

// ----------------------------------------------------------------------------
CefToastClientHandler::CefToastClientHandler(IClientHandlerEvents* aClientHandlerEvents)
:CCefPopupClientHandler(aClientHandlerEvents)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CefToastClientHandler::~CefToastClientHandler(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler
void CefToastClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) 
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
	m_pClientHandlerEvents->OnNewToastWindow(browser->GetWindowHandle());
}
