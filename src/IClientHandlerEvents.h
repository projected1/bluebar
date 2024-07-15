#pragma once

class IClientHandlerEvents
{
public:
	virtual void OnAddressChange(const std::string& aUri) = 0;
	virtual void OnLoadError(const std::string& aUri, int aErrorCode) = 0;
	virtual void OnLoadEnd(const std::string& aUri, CefWindowHandle aHwnd) = 0;
	virtual void OnNewWindow(CefWindowHandle aHwnd) = 0;
	virtual void OnNewToastWindow(CefWindowHandle aHwnd) = 0;
	virtual void OnPopup(const std::string& aUri) = 0;
	virtual void OnWindowResized(CefWindowHandle aHwnd, int aWidth, int aHeight) = 0;
	virtual void OnWindowResizedBy(CefWindowHandle aHwnd, int aX, int aY) = 0;
	virtual void OnWindowMoveTo(CefWindowHandle aHwnd, int aX, int aY) = 0;
	virtual void OnTitleChanged(CefWindowHandle aHwnd, const std::string& aTitle) = 0;
	virtual void OnWindowClosed(CefWindowHandle aHwnd) = 0;
};