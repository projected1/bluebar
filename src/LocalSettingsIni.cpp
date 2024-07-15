#include "StdAfx.h"
#include "LocalSettingsIni.h"

// ----------------------------------------------------------------------------
CLocalSettingsIni::CLocalSettingsIni(const std::string& aSettingsFile)
{
	SCOPE_LOG();

	m_sSettingsFile = aSettingsFile;
}

// ----------------------------------------------------------------------------
CLocalSettingsIni::~CLocalSettingsIni(void)
{
	SCOPE_LOG();
}

// ----------------------------------------------------------------------------
bool CLocalSettingsIni::GetState(const std::string& aKey)
{
	SCOPE_LOG();

	static enum { CFG_STATE_LEN = 2 };
	char szState[CFG_STATE_LEN + sizeof(char)] = {0};
	::GetPrivateProfileStringA("config", aKey.c_str(), NULL, szState, 
		CFG_STATE_LEN, m_sSettingsFile.c_str());
	return (szState[0] == '1');
}

// ----------------------------------------------------------------------------
void CLocalSettingsIni::PutState(const std::string& aKey, bool aState)
{
	SCOPE_LOG();

	BOOL rv = ::WritePrivateProfileStringA("config", aKey.c_str(), 
		(aState ? "1" : NULL), m_sSettingsFile.c_str());
	_ASSERTE(rv);
}

// ----------------------------------------------------------------------------
std::string CLocalSettingsIni::GetVal(const std::string& aKey)
{
	SCOPE_LOG();

	static enum { CFG_VAL_LEN = 1000 };
	char szVal[CFG_VAL_LEN + sizeof(char)] = {0};
	::GetPrivateProfileStringA("config", aKey.c_str(), NULL, szVal, 
		CFG_VAL_LEN, m_sSettingsFile.c_str());
	return szVal;
}

// ----------------------------------------------------------------------------
void CLocalSettingsIni::PutVal(const std::string& aKey, const std::string& aVal)
{
	SCOPE_LOG();

	BOOL rv = ::WritePrivateProfileStringA("config", aKey.c_str(), 
		aVal.c_str(), m_sSettingsFile.c_str());
	_ASSERTE(rv);
}
