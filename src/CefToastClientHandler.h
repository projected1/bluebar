#pragma once

#include "CefPopupClientHandler.h"
#include "IClientHandlerEvents.h"


class CefToastClientHandler :
	public CCefPopupClientHandler
{
private:
	IMPLEMENT_REFCOUNTING(CefToastClientHandler);
	IMPLEMENT_LOCKING(CefToastClientHandler);

public:
	CefToastClientHandler(IClientHandlerEvents* aClientHandlerEvents);
	virtual ~CefToastClientHandler(void);

protected:
	// CefLifeSpanHandler
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
};
