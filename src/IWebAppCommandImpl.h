#pragma once

#include "IWebAppCommand.h"


class IWebAppCommandImpl :
	public IWebAppCommand
{
public:
	virtual void WebCmdToast() OVERRIDE {}
	virtual void WebCmdLog() OVERRIDE {}
	virtual void WebCmdDefault() OVERRIDE {}
	virtual void WebCmdNavigate(const std::string& aUrl) OVERRIDE {}
	virtual void WebCmdPopup() OVERRIDE {}
	virtual void WebCmdCut(const CRect& aRect, DWORD aCombineMode) OVERRIDE {}
	virtual void WenCmdCutPolygon(const std::vector<CPoint>& aPoints, DWORD aCombineMode) OVERRIDE {}
	virtual void WebCmdDock(bool bState) OVERRIDE {}
	virtual void WebCmdClose() OVERRIDE {}
};