#include "stdafx.h"
#include "BluebarModule.h"
#include "ClientApp.h"


// ----------------------------------------------------------------------------
CBluebarModule::CBluebarModule(void)
{
	SCOPE_LOG();

	// Init controls
	::AtlInitCommonControls(ICC_STANDARD_CLASSES);

	// Populate the settings based on command line arguments
	AppGetSettings(m_cefSettings, m_cefApp);

	// Initialize CEF
	bool rv = CefInitialize(m_cefSettings, m_cefApp);
	_ASSERTE(rv);

	// Register the internal client plugin.
	InitInternalPlugins();

	// Check if running after installation
	CefRefPtr<CefCommandLine> spCommandLine = CefCommandLine::CreateCommandLine();
	spCommandLine->InitFromString(::GetCommandLineW());

	// Main window
	HWND hwndBar = m_bluebarWindow.Create(::GetDesktopWindow());
	_ASSERTE(hwndBar);
}

// ----------------------------------------------------------------------------
CBluebarModule::~CBluebarModule(void)
{
	SCOPE_LOG();

	// Shut down CEF
	CefShutdown();
}

// ----------------------------------------------------------------------------
void CBluebarModule::InitInternalPlugins()
{
	SCOPE_LOG();

	// Load plugin module
	HMODULE hPlugin = ::LoadLibrary(_T("npBluebar.dll"));
	_ASSERTE(hPlugin != NULL);
	NPError (API_CALL* NP_Initialize)(NPNetscapeFuncs*);
	NPError (API_CALL* NP_GetEntryPoints)(NPPluginFuncs*);
	NPError (API_CALL* NP_Shutdown)(void);
	*(FARPROC*)&NP_Initialize = ::GetProcAddress(hPlugin, "NP_Initialize");
	*(FARPROC*)&NP_GetEntryPoints = ::GetProcAddress(hPlugin, "NP_GetEntryPoints");
	*(FARPROC*)&NP_Shutdown = ::GetProcAddress(hPlugin, "NP_Shutdown");
	_ASSERTE(NP_Initialize != NULL);
	_ASSERTE(NP_GetEntryPoints != NULL);
	_ASSERTE(NP_Shutdown != NULL);

	// Init plugin info
	CefPluginInfo plugin_info;
	CefString(&plugin_info.display_name).FromASCII("Bluebar");
	CefString(&plugin_info.unique_name).FromASCII("npBluebar");
	CefString(&plugin_info.description).FromASCII("Bluebar Plugin");
	CefString(&plugin_info.mime_types).FromASCII("application/x-npbluebar");
	CefString(&plugin_info.file_extensions).FromASCII("dll");
	plugin_info.np_initialize = NP_Initialize;
	plugin_info.np_getentrypoints = NP_GetEntryPoints;
	plugin_info.np_shutdown = NP_Shutdown;

	// Register internal plugin
	CefRegisterPlugin(plugin_info);
}

// ----------------------------------------------------------------------------
int CBluebarModule::Run()
{
	SCOPE_LOG();

	int rv = 0;

	if (!m_cefSettings.multi_threaded_message_loop) 
	{
		// Run the CEF message loop. This function will block until the 
		// application receives a "WM_QUIT" message.
		CefRunMessageLoop();
	} 
	else 
	{
		rv = RunNative();
	}

	return rv;
}

// ----------------------------------------------------------------------------
int CBluebarModule::RunNative()
{
	SCOPE_LOG();

	HACCEL hAcc = ::LoadAccelerators(_AtlBaseModule.m_hInst, MAKEINTRESOURCE(IDR_BLUEBAR));
	MSG msg = {0};
	int rv = 1;
	while (rv = ::GetMessage(&msg, NULL, 0, 0)) 
	{
		if (-1 == rv)
		{
			// TODO: Handle the error and possibly exit
			// ...
		}
		else if (hAcc && !::TranslateAccelerator(msg.hwnd, hAcc, &msg)) 
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	rv = static_cast<int>(msg.wParam);
	return rv;
}

// ----------------------------------------------------------------------------
void CBluebarModule::AppGetSettings(CefSettings& settings, CefRefPtr<CefApp>& app)
{
	SCOPE_LOG();

	CefRefPtr<CefCommandLine> spCommandLine = CefCommandLine::CreateCommandLine();
	spCommandLine->InitFromString(::GetCommandLineW());

	// Override the cache path settings for app cache, cookies and local storage
	//CefString(&settings.cache_path) = spCommandLine->GetSwitchValue(cefclient::kCachePath);
	CefString(&settings.cache_path) = "bluebar.cache";

	settings.multi_threaded_message_loop = spCommandLine->HasSwitch(cefclient::kMultiThreadedMessageLoop);
	CefString(&settings.user_agent) = spCommandLine->GetSwitchValue(cefclient::kUserAgent);
	CefString(&settings.product_version) = spCommandLine->GetSwitchValue(cefclient::kProductVersion);
	CefString(&settings.locale) = spCommandLine->GetSwitchValue(cefclient::kLocale);
	CefString(&settings.log_file) = spCommandLine->GetSwitchValue(cefclient::kLogFile);

	std::string str = spCommandLine->GetSwitchValue(cefclient::kLogSeverity);
	bool invalid = false;
	if (!str.empty()) 
	{
		if (str == cefclient::kLogSeverity_Verbose) settings.log_severity = LOGSEVERITY_VERBOSE;
		else if (str == cefclient::kLogSeverity_Info) settings.log_severity = LOGSEVERITY_INFO;
		else if (str == cefclient::kLogSeverity_Warning) settings.log_severity = LOGSEVERITY_WARNING;
		else if (str == cefclient::kLogSeverity_Error) settings.log_severity = LOGSEVERITY_ERROR;
		else if (str == cefclient::kLogSeverity_ErrorReport) settings.log_severity = LOGSEVERITY_ERROR_REPORT;
		else if (str == cefclient::kLogSeverity_Disable) settings.log_severity = LOGSEVERITY_DISABLE;
		else invalid = true;
	}
	if (str.empty() || invalid) 
	{
#ifndef _DEBUG
		// Only log error messages and higher in release build
		settings.log_severity = LOGSEVERITY_DISABLE;
#endif
	}

	str = spCommandLine->GetSwitchValue(cefclient::kGraphicsImpl);
	if (!str.empty()) 
	{
		if (str == cefclient::kGraphicsImpl_Angle) settings.graphics_implementation = ANGLE_IN_PROCESS;
		else if (str == cefclient::kGraphicsImpl_AngleCmdBuffer) settings.graphics_implementation = ANGLE_IN_PROCESS_COMMAND_BUFFER;
		else if (str == cefclient::kGraphicsImpl_Desktop) settings.graphics_implementation = DESKTOP_IN_PROCESS;
		else if (str == cefclient::kGraphicsImpl_DesktopCmdBuffer) settings.graphics_implementation = DESKTOP_IN_PROCESS_COMMAND_BUFFER;
	}

	settings.local_storage_quota = atoi(((std::string)spCommandLine->GetSwitchValue(cefclient::kLocalStorageQuota)).c_str());
	settings.session_storage_quota = atoi(((std::string)spCommandLine->GetSwitchValue(cefclient::kSessionStorageQuota)).c_str());
	CefString(&settings.javascript_flags) = spCommandLine->GetSwitchValue(cefclient::kJavascriptFlags);
	CefString(&settings.resources_dir_path) = spCommandLine->GetSwitchValue(cefclient::kResourcesDirPath);
	CefString(&settings.locales_dir_path) = spCommandLine->GetSwitchValue(cefclient::kLocalesDirPath);
	settings.pack_loading_disabled = spCommandLine->HasSwitch(cefclient::kPackLoadingDisabled);

	// Retrieve command-line proxy configuration, if any
	bool has_proxy = false;
	cef_proxy_type_t proxy_type = CEF_PROXY_TYPE_DIRECT;
	CefString proxy_config;

	if (spCommandLine->HasSwitch(cefclient::kProxyType)) 
	{
		std::string str = spCommandLine->GetSwitchValue(cefclient::kProxyType);
		if (str == cefclient::kProxyType_Direct) 
		{
			has_proxy = true;
			proxy_type = CEF_PROXY_TYPE_DIRECT;
		} 
		else if (str == cefclient::kProxyType_Named || str == cefclient::kProxyType_Pac) 
		{
			proxy_config = spCommandLine->GetSwitchValue(cefclient::kProxyConfig);
			if (!proxy_config.empty()) 
			{
				has_proxy = true;
				proxy_type = (str == cefclient::kProxyType_Named ? CEF_PROXY_TYPE_NAMED : CEF_PROXY_TYPE_PAC_STRING);
			}
		}
	}

	if (has_proxy) 
	{
		// Provide a ClientApp instance to handle proxy resolution
		app = new CClientApp(proxy_type, proxy_config);
	}
}
