#pragma once
#include <string>

class ILocalSettings
{
public:
	virtual bool GetState(const std::string& aKey) = 0;
	virtual void PutState(const std::string& aKey, bool aState) = 0;
	virtual std::string GetVal(const std::string& aKey) = 0;
	virtual void PutVal(const std::string& aKey, const std::string& aVal) = 0;

};