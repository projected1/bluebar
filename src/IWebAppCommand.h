#pragma once

#include <string>
#include <vector>
#include <atltypes.h>


class IWebAppCommand
{
public:
	virtual void WebCmdToast() = 0;
	virtual void WebCmdLog() = 0;
	virtual void WebCmdDefault() = 0;
	virtual void WebCmdNavigate(const std::string& aUrl) = 0;
	virtual void WebCmdPopup() = 0;
	virtual void WebCmdCut(const CRect& aRect, DWORD aCombineMode) = 0;
	virtual void WenCmdCutPolygon(const std::vector<CPoint>& aPoints, DWORD aCombineMode) = 0;
	virtual void WebCmdDock(bool bState) = 0;
	virtual void WebCmdClose() = 0;
};