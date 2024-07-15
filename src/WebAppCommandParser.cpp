#include "stdafx.h"
#include "WebAppCommandParser.h"
#include "FragmentParser.h"
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")


// ----------------------------------------------------------------------------
CWebAppCommandParser::CWebAppCommandParser(const std::string& aUri, 
										   IWebAppCommand* aCommandImpl)
{
	SCOPE_LOG();

	// Get URI fragment
	unsigned int nPos = aUri.find("#");
	if (nPos != aUri.npos)
	{
		CFragmentParser fragment(&aUri[nPos + 1]);

		std::string sCommand;
		if (fragment.GetSection(0, sCommand))
		{
			// Deprecated
			// Message box
			//if (sCommand == "msg")
			//{
			//	std::string sCaption;
			//	std::string sText;
			//	if (fragment.GetSection(1, sCaption) &&
			//		fragment.GetSection(2, sText))
			//	{
			//		aCommandImpl->Message(sCaption, sText);
			//	}
			//}

			// Deprecated
			// Toast
			//if (sCommand == "tst")
			//{
			//	// TODO: Not implemented
			//	// ...
			//}

			// Deprecated
			// Pop-up window
			//if (sCommand == "pop")
			//{
			//	std::string sWindowName;
			//	if (fragment.GetSection(1, sWindowName))
			//	{
			//		// TODO: Implement real login to our app
			//		// ...
			//	}
			//}

			// Log
			if (sCommand == "log")
			{
				// TODO: Not implemented
				// ...
			}
			// Default action
			else if (sCommand == "def")
			{
				std::string sUri;
				if (fragment.GetSection(1, sUri))
				{
					// Open link in default browser
					::ShellExecuteA(NULL, "open", sUri.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
			// Navigate browser
			else if (sCommand == "nav")
			{
				std::string sUri;
				if (fragment.GetSection(1, sUri))
				{
					aCommandImpl->WebCmdNavigate(sUri);
				}
			}
			// Cut window region
			else if (sCommand == "cut")
			{
				// Get region combine mode
				std::string sBuf;
				bool rv = fragment.GetSection(1, sBuf);
				_ASSERTE(rv);
				DWORD nCombineMode = atoi(sBuf.c_str());
				_ASSERTE(nCombineMode >= RGN_MIN && nCombineMode <= RGN_MAX);

				// Get the region rect
				CRect rc(0, 0, 0, 0);
				rv = fragment.GetSection(2, sBuf);
				_ASSERTE(rv);
				rc.left = atoi(sBuf.c_str());

				rv = fragment.GetSection(3, sBuf);
				_ASSERTE(rv);
				rc.top = atoi(sBuf.c_str());

				rv = fragment.GetSection(4, sBuf);
				_ASSERTE(rv);
				rc.right = atoi(sBuf.c_str());

				rv = fragment.GetSection(5, sBuf);
				_ASSERTE(rv);
				rc.bottom = atoi(sBuf.c_str());

				// Handle the region
				aCommandImpl->WebCmdCut(rc, nCombineMode);
			}
			else if (sCommand == "cutpoly")
			{
				// Get region combine mode
				std::string sBuf;
				bool rv = fragment.GetSection(1, sBuf);
				_ASSERTE(rv);
				DWORD nCombineMode = atoi(sBuf.c_str());
				_ASSERTE(nCombineMode >= RGN_MIN && nCombineMode <= RGN_MAX);

				// Parse the vertexes
				std::vector<CPoint> points;
				CPoint pt(0, 0);
				unsigned int nSectionNum = 2;
				while (fragment.GetSection(nSectionNum++, sBuf))
				{
					pt.x = atoi(sBuf.c_str());

					// Expecting even amount of co-ordinates, thus this call should not fail
					rv = fragment.GetSection(nSectionNum++, sBuf);
					_ASSERTE(rv);

					pt.y = atoi(sBuf.c_str());
					points.push_back(pt);
				}

				// Handle the region
				aCommandImpl->WenCmdCutPolygon(points, nCombineMode);
			}
			else if (sCommand == "dock")
			{
				// Get region combine mode
				std::string sBuf;
				bool rv = fragment.GetSection(1, sBuf);
				_ASSERTE(rv);

				if (sBuf == "true")
				{
					aCommandImpl->WebCmdDock(true);
				}
				else if (sBuf == "false")
				{
					aCommandImpl->WebCmdDock(false);
				}
			}
			else if (sCommand == "close")
			{
				aCommandImpl->WebCmdClose();
			}
		} // section 0
	} // fragment
}

// ----------------------------------------------------------------------------
CWebAppCommandParser::~CWebAppCommandParser(void)
{
	SCOPE_LOG();
}
