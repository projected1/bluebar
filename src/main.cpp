#include "stdafx.h"
#include "BluebarModule.h"
#include "InstallerEvents.h"
#include "BluebarAnalytics.h"
#include "BluebarUrls.h"
#include "BluebarLocalSettings.h"


// Globals
CComModule _ComModule;
HINSTANCE hInst;		// NOTE: CEF expects to find an instance handle via "hInst" global


// Constants
const char kChannel[] = "release";
const char kRsaPubKey[] = "**********";
const char kDefaultAffiliate[] = "c847c13154e140b6a8c3c8387b968399";


// ----------------------------------------------------------------------------
bool EnsureOneAppInstance()
{
	SCOPE_LOG();

	HANDLE hMutex = ::CreateMutex(NULL, TRUE, _T("Local\\af4ee184dcb94188a33b9c5043e15158"));
	_ASSERT(hMutex);

	DWORD nError = ::GetLastError();
	return (ERROR_ALREADY_EXISTS != nError);
}

// ----------------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	SCOPE_LOG();

#ifdef _DEBUG
	// Dump command line
	::OutputDebugString(lpCmdLine);

	// Enable run-time memory check for debug builds
	// TODO: "_CRTDBG_LEAK_CHECK_DF" always generates dumps in COM/ATL projects
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_CRT_DF);
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// For cefclient internals
	hInst = hInstance;

	// Main process exit code
	int rv = 0;

	// Parse command line
	CBluebarLocalSettings config;
	CefRefPtr<CefCommandLine> spCommandLine = CefCommandLine::CreateCommandLine();
	spCommandLine->InitFromString(::GetCommandLineW());
	config.after_install = spCommandLine->HasSwitch("after-install");
	config.after_update = spCommandLine->HasSwitch("after-update");
	if (spCommandLine->HasSwitch("affiliate-id"))
	{
		config.affiliate_id = spCommandLine->GetSwitchValue("affiliate-id");
	}

	// Init event logger
	static enum { CONTEXT_DESKTOP_CLIENT = 5 };
	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Init(BluebarUrls::kEventLoggerUri, CONTEXT_DESKTOP_CLIENT, config.affiliate_id);

	// TODO: Shouldn't be able to run without affiliate. However, there is a problem
	//		 migrating to this version with this restriction since we originally had
	//		 no affiliates.
	if (config.affiliate_id.empty())
	{
		// Log event
		BluebarAnalytics::LogMissingAffiliate();

		// Fallback to (our) default affiliate
		config.affiliate_id = kDefaultAffiliate;
	}

	// Init again, this time with the default affiliate
	eventlogger.Init(BluebarUrls::kEventLoggerUri, CONTEXT_DESKTOP_CLIENT, config.affiliate_id);

	if (EnsureOneAppInstance()) 
	{
		// Set updater command line
		std::string sCmdLine;
		sCmdLine = sCmdLine + "/aid=" + config.affiliate_id;

		// Init installer
		CUpdater updater(BluebarUrls::kManifestUri, kChannel, kRsaPubKey, sCmdLine);
		if (updater.IsPendingUpdate() && updater.InstallPendingUpdate())
		{
			// Log event
 			BluebarAnalytics::LogUpdate();
		}
		else
		{
			// Subscribe for updater events and start updater in background
			CInstallerEvents updateEvents;
			updater.Subscribe(&updateEvents);
			updater.Go();

			// Log event
			BluebarAnalytics::LogStart();

			// Go
			CBluebarModule module;
			rv = module.Run();

			// Log event
			BluebarAnalytics::LogEnd();
 		}
	}
	else
	{
		// Log event
		BluebarAnalytics::LogAlreadyRunning();

		// TODO: If there's no window, wait for a while and then kill the target process
		//       that may be stuck (BUG) and continue running from here.
		// ...

		// Show Bluebar
		HWND hwnd = ::FindWindowA(NULL, "Bluebar");
		_ASSERTE(::IsWindow(hwnd));

		if (::IsWindow(hwnd))
		{
			UINT WM_SHOW_BLUEBAR = ::RegisterWindowMessage(_T("E5B785CA-BE97-4ee5-800C-BD48DA306FED"));
			::PostMessage(hwnd , WM_SHOW_BLUEBAR, 0, 0);
		}

		// Error
		rv = 1;
	}

	// TODO: "CEventLogger" should take care of this internally
	// Wait for all the events to finish
	eventlogger.AwaitCompletion();

	return rv;
}
