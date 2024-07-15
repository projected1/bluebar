#pragma once

#include "IClientHandlerEvents.h"


class CCefPopupClientHandler :
	public CefClient,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefDisplayHandler,
	public CefV8ContextHandler,
	public CefV8Handler,
	public CefDOMVisitor
{
private:
	static enum
	{
		HTTP_FIRST			= 100,
		HTTP_INFORMATIONAL	= 100,
		HTTP_SUCCESS		= 200,
		HTTP_REDIRECTION	= 300,
		HTTP_ERROR			= 400,
		HTTP_CLIENT_ERROR	= 400,
		HTTP_SERVER_ERROR	= 500,
		HTTP_LAST			= 599
	};

private:
	IMPLEMENT_REFCOUNTING(CCefPopupClientHandler);
	IMPLEMENT_LOCKING(CCefPopupClientHandler);

public:
	CCefPopupClientHandler(IClientHandlerEvents* aClientHandlerEvents);
	virtual ~CCefPopupClientHandler(void);

protected:
	// CefClient
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefV8ContextHandler> GetV8ContextHandler() OVERRIDE { return this; }

	// CefLifeSpanHandler
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, const CefString& url, CefRefPtr<CefClient>& client, CefBrowserSettings& settings) OVERRIDE;
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
	virtual bool OnLoadError(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& failedUrl, CefString& errorText) OVERRIDE;

	// CefDisplayHandler
	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;

	// CefV8ContextHandler
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

	// CefV8Handler
	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) OVERRIDE;

	// CefDOMVisitor
	virtual void Visit(CefRefPtr<CefDOMDocument> document) OVERRIDE;

protected:
	CefRefPtr<CefBrowser>	m_spBrowser;
	IClientHandlerEvents*	m_pClientHandlerEvents;		// Aggregation
};
