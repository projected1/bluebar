#include "stdafx.h"
#include "BluebarAnalytics.h"

namespace BluebarAnalytics
{

// ----------------------------------------------------------------------------
void LogUpdate()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("Update");
}

// ----------------------------------------------------------------------------
void LogStart()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("Start");
}

// ----------------------------------------------------------------------------
void LogEnd()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("End");
}

// ----------------------------------------------------------------------------
void LogAlreadyRunning()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("AlreadyRunning");
}

// ----------------------------------------------------------------------------
void LogScreenResolution()
{
	SCOPE_LOG();

	RECT rcDesktop = {0};
	::GetWindowRect(GetDesktopWindow(), &rcDesktop);
	CStringA sResolution;
	sResolution.Format("%dx%d", rcDesktop.right, rcDesktop.bottom);
	CEventLogger::event_args args;
	args["WxH"] = sResolution.GetString();
	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("ScreenResolution", args);
}

// ----------------------------------------------------------------------------
void LogExitTray()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("ExitTray");
}

// ----------------------------------------------------------------------------
void LogUpdateReady()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("UpdateReady");
}

// ----------------------------------------------------------------------------
void LogLoginDialogSubmit()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("LoginGialogSubmit");
}

// ----------------------------------------------------------------------------
void LogLoginDialogClose()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("LoginDialogClose");
}

// ----------------------------------------------------------------------------
void LogMissingAffiliate()
{
	SCOPE_LOG();

	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("MissingAffiliate");
}

// ----------------------------------------------------------------------------
void LogHttp404(const std::string& aUri)
{
	SCOPE_LOG();

	CEventLogger::event_args args;
	args["URI"] = aUri;
	CEventLogger& eventlogger = CEventLogger::get_mutable_instance();
	eventlogger.Log("Http404", args);
}

} // namespace BluebarAnalytics