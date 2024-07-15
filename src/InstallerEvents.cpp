#include "stdafx.h"
#include "InstallerEvents.h"
#include "BluebarAnalytics.h"


// ----------------------------------------------------------------------------
CInstallerEvents::CInstallerEvents(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CInstallerEvents::~CInstallerEvents(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
void CInstallerEvents::OnInstallReady()
{
	SCOPE_LOG();

	BluebarAnalytics::LogUpdateReady();
}
