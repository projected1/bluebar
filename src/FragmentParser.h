#pragma once
#include <string>
#include <vector>

class CFragmentParser
{
public:
	CFragmentParser(const std::string& aFragment)
	{
		std::string::size_type nPos = 0;
		std::string::size_type nPrevPos = 0;
		if (!aFragment.empty())
		{
			while ((nPos = aFragment.find(",", nPrevPos)) != aFragment.npos)
			{
				m_vecCommand.push_back(aFragment.substr(nPrevPos, nPos - nPrevPos));
				nPrevPos = nPos + 1;
			}
			m_vecCommand.push_back(aFragment.substr(nPrevPos, nPos - nPrevPos));
		}
	}

	~CFragmentParser()
	{
	}

	bool GetSection(unsigned int aSectionNumber, std::string& aSection)
	{
		if (aSectionNumber < m_vecCommand.size())
		{
			aSection = m_vecCommand[aSectionNumber];
			return true;
		}
		return false;
	}

protected:
	std::vector<std::string> m_vecCommand;
};