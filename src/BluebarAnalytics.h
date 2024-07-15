#pragma once


namespace BluebarAnalytics
{

void LogUpdate();
void LogStart();
void LogEnd();
void LogAlreadyRunning();
void LogScreenResolution();
void LogExitTray();
void LogUpdateReady();
void LogLoginDialogSubmit();
void LogLoginDialogClose();
void LogMissingAffiliate();
void LogHttp404(const std::string& aUri);

} // namespace BluebarAnalytics