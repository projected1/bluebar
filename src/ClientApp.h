#pragma once

#include "include\cef_app.h"
#include "include\cef_proxy_handler.h"


class CClientApp :
	public CefApp,
	public CefProxyHandler
{
private:
	IMPLEMENT_REFCOUNTING(CClientApp);

public:
	CClientApp(cef_proxy_type_t proxy_type, const CefString& proxy_config);
	~CClientApp(void);

protected:
	// CefApp
	virtual CefRefPtr<CefProxyHandler> GetProxyHandler() OVERRIDE { return this; }

	// CefProxyHandler
	virtual void GetProxyForUrl(const CefString& url, CefProxyInfo& proxy_info) OVERRIDE;

protected:
	cef_proxy_type_t m_nProxyType;
	CefString m_sProxyConfig;
};
