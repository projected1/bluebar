#pragma once
#include "ILocalSettings.h"

class CLocalSettingsIni :
	public ILocalSettings
{
public:
	CLocalSettingsIni(const std::string& aSettingsFile);
	virtual ~CLocalSettingsIni(void);

	// ILocalSettings
	virtual bool GetState(const std::string& aKey) OVERRIDE;
	virtual void PutState(const std::string& aKey, bool aState) OVERRIDE;
	virtual std::string GetVal(const std::string& aKey) OVERRIDE;
	virtual void PutVal(const std::string& aKey, const std::string& aVal) OVERRIDE;

private:
	std::string m_sSettingsFile;
};
