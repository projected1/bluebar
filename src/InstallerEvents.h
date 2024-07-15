#pragma once

#include "Installer.h"


class CInstallerEvents :
	public IInstallProgressEventsImpl
{
public:
	CInstallerEvents(void);
	virtual ~CInstallerEvents(void);

	// IInstallProgressEvents
	virtual void OnInstallReady();
};
