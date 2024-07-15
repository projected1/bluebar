// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Deprecate warnings
#pragma warning(disable : 4996) 				// Unsafe function
#pragma warning(disable : 4244) 				// Conversion truncating

// Globals
#include "targetver.h"
#include "resource.h"

// Windows
#define WIN32_LEAN_AND_MEAN						// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <minmax.h>

// CRT
#include <io.h>
#include <tchar.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// ATL
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS		// some CString constructors will be explicit
#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atltypes.h>

// CEF
#include "include/cef_app.h"
#include "include/cef_url.h"
#include "include/cef_frame.h"
#include "include/cef_stream.h"
#include "include/cef_cookie.h"
#include "include/cef_browser.h"
#include "include/cef_nplugin.h"
#include "include/cef_runnable.h"
#include "include/cef_command_line.h"
#include "include/internal/cef_ptr.h"
#include "include/internal/cef_time.h"
#include "include/internal/cef_build.h"
#include "include/wrapper/cef_byte_read_handler.h"

// CEF Client
#include "cefclient/util.h"
#include "cefclient/cefclient.h"
#include "cefclient/string_util.h"
#include "cefclient/resource_util.h"
#include "cefclient/client_handler.h"
#include "cefclient/cefclient_switches.h"

// CEF handlers
#include "include/cef_client.h"
#include "include/cef_load_handler.h"
#include "include/cef_menu_handler.h"
#include "include/cef_display_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_geolocation_handler.h"

// STL
#include <map>
#include <string>
#include <vector>
#include <strstream>
#include <algorithm>

// WTL
#include <atlapp.h>
#include <atlgdi.h>
#include <atltheme.h>
#include <atlctrls.h>

// BOOST
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/win32/mutex.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// cURL
#include "curl/curl.h"
#pragma comment(lib, "libcurl.lib")

// Common controls
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// Utils
#include "Ping.h"
#include "EventLogger.h"
#include "Installer.h"
#include "StringUtils.h"
#include "tstring.h"
#include "ModuleInfo.h"

// Common
#include "Logger.h"
#include "ScopeLogger.h"