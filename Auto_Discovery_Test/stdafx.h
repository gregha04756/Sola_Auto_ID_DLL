// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment (lib,"Report_Error_DLL.lib")
#pragma comment (lib,"Sola_Auto_ID_DLL.lib")
#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"IPHLPAPI.LIB")
#pragma comment (lib,"comctl32.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdint.h>
#include <IPHlpApi.h>
#include <WinSock2.h>
#include <CommCtrl.h>
#include <new>
#include <list>
#include <string>

// TODO: reference additional headers your program requires here
#include "Report_Error_DLL.h"
#include "Sola_Auto_ID_DLL.h"
