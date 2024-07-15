#pragma once

#include "BluebarWindow.h"
#include "DockWindow.h"
#include "BluebarLocalSettings.h"


class CBluebarModule
{
public:
	CBluebarModule(void);
	~CBluebarModule(void);

	int Run();

private:
	int RunNative();
	void AppGetSettings(CefSettings& settings, CefRefPtr<CefApp>& app);
	void InitInternalPlugins();

protected:
	CBluebarWindow		m_bluebarWindow;

private:
	CefRefPtr<CefApp>	m_cefApp;
	CefSettings			m_cefSettings;
};
