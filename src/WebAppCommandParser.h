#pragma once

#include "IWebAppCommand.h"


class CWebAppCommandParser
{
public:
	CWebAppCommandParser(const std::string& aUri, IWebAppCommand* aCommandImpl);
	virtual ~CWebAppCommandParser(void);
};
