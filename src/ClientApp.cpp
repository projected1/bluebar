#include "stdafx.h"
#include "ClientApp.h"


// ----------------------------------------------------------------------------
CClientApp::CClientApp(cef_proxy_type_t proxy_type, const CefString& proxy_config)
:m_nProxyType(proxy_type), m_sProxyConfig(proxy_config) 
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CClientApp::~CClientApp(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
// CefProxyHandler
void CClientApp::GetProxyForUrl(const CefString& url, CefProxyInfo& proxy_info)
{
	SCOPE_LOG();

	proxy_info.proxyType = m_nProxyType;
	if (!m_sProxyConfig.empty())
	{
		CefString(&proxy_info.proxyList) = m_sProxyConfig;
	}
}