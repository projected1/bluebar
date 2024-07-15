#pragma once

#include "IClientHandlerEvents.h"


class CCefClientHandler : 
	public CefClient,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefDisplayHandler,
	public CefRequestHandler,
	public CefMenuHandler,
	public CefGeolocationHandler

	// TODO: Implement
	//public CefDragHandler
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
	IMPLEMENT_REFCOUNTING(CCefClientHandler);
	IMPLEMENT_LOCKING(CCefClientHandler);

public:
	CCefClientHandler(IClientHandlerEvents* aClientHandlerEvents);
	virtual ~CCefClientHandler();

	CefRefPtr<CefBrowser> GetBrowser();
	void SetCookie(const CefString& aUrl, const CefString& aCookie);

protected:
	// CefClient
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefMenuHandler> GetMenuHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE { return this; }

	// CefLifeSpanHandler
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, const CefString& url, CefRefPtr<CefClient>& client, CefBrowserSettings& settings) OVERRIDE;
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
	virtual bool OnLoadError(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& failedUrl, CefString& errorText) OVERRIDE;

	// CefDisplayHandler
	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;

	// CefRequestHandler
	virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefRequest> request, CefString& redirectUrl, CefRefPtr<CefStreamReader>& resourceStream, CefRefPtr<CefResponse> response, int loadFlags) OVERRIDE;

	// CefMenuHandler
	virtual bool OnBeforeMenu(CefRefPtr<CefBrowser> browser, const CefMenuInfo& menuInfo) OVERRIDE;

	// CefGeolocationHandler
	virtual void OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

private:
	bool LoadHtmlResource(int aResourceId, DWORD &aSize, LPBYTE &aBytes);
	CefRefPtr<CefStreamReader> GetHtmlResourceReader(int aResourceId);
	void HideScrollbars(CefRefPtr<CefFrame> aFrame);

private:
	CefWindowHandle			m_hwndPopup;
	CefRefPtr<CefBrowser>	m_spBrowser;
	IClientHandlerEvents*	m_pClientHandlerEvents;		// Aggregation
};
