#pragma once
#include "LocalSettingsIni.h"

class CBluebarLocalSettings :
	public CLocalSettingsIni
{
public:
	CBluebarLocalSettings(void);
	virtual ~CBluebarLocalSettings(void);

	// Properties (API)
	__declspec(property(get = GetHidden, put = PutHidden)) bool hidden;
	__declspec(property(get = GetAfterInstall, put = PutAfterInstall)) bool after_install;
	__declspec(property(get = GetAfterUpdate, put = PutAfterUpdate)) bool after_update;
	__declspec(property(get = GetAffiliateId, put = PutAffiliateId)) std::string affiliate_id;

	// Getters and setters
	bool GetHidden();
	void PutHidden(bool aState);
	
	bool GetAfterInstall();
	void PutAfterInstall(bool aState);

	bool GetAfterUpdate();
	void PutAfterUpdate(bool aState);

	std::string GetAffiliateId();
	void PutAffiliateId(const std::string& aVal);

private:
	static std::string GetSettingsFileName();
};
