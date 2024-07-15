#include "stdafx.h"
#include "BluebarLocalSettings.h"

// ----------------------------------------------------------------------------
const std::string kConfigFileName("config");

// ----------------------------------------------------------------------------
CBluebarLocalSettings::CBluebarLocalSettings(void)
:CLocalSettingsIni(GetSettingsFileName())
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
CBluebarLocalSettings::~CBluebarLocalSettings(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
/*static*/ std::string CBluebarLocalSettings::GetSettingsFileName()
{
	SCOPE_LOG();

	DWORD nDirSize = ::GetCurrentDirectoryA(0, NULL);
	_ASSERTE(nDirSize);
	boost::shared_ptr<char> spCurDir((char*)calloc(nDirSize + 1, sizeof(char)), free);
	_ASSERTE(spCurDir.get());
	DWORD rv = ::GetCurrentDirectoryA(nDirSize, spCurDir.get());
	_ASSERTE(rv);
	std::string sSettingsFile;
	sSettingsFile = sSettingsFile + spCurDir.get() + "\\" + kConfigFileName;
	return sSettingsFile;
}

// ----------------------------------------------------------------------------
bool CBluebarLocalSettings::GetHidden()
{
	SCOPE_LOG();

	return (GetState("hidden"));
}

// ----------------------------------------------------------------------------
void CBluebarLocalSettings::PutHidden(bool aState)
{
	SCOPE_LOG();

	PutState("hidden", aState);
}

// ----------------------------------------------------------------------------
bool CBluebarLocalSettings::GetAfterInstall()
{
	SCOPE_LOG();

	return (GetState("after_install"));
}

// ----------------------------------------------------------------------------
void CBluebarLocalSettings::PutAfterInstall(bool aState)
{
	SCOPE_LOG();

	PutState("after_install", aState);
}

// ----------------------------------------------------------------------------
bool CBluebarLocalSettings::GetAfterUpdate()
{
	SCOPE_LOG();

	return (GetState("after_update"));
}

// ----------------------------------------------------------------------------
void CBluebarLocalSettings::PutAfterUpdate(bool aState)
{
	SCOPE_LOG();

	PutState("after_update", aState);
}

// ----------------------------------------------------------------------------
std::string CBluebarLocalSettings::GetAffiliateId()
{
	SCOPE_LOG();

	return (GetVal("affiliate_id"));
}

// ----------------------------------------------------------------------------
void CBluebarLocalSettings::PutAffiliateId(const std::string& aVal)
{
	SCOPE_LOG();

	PutVal("affiliate_id", aVal);
}
