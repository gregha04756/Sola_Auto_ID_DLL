// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment (lib,"iphlpapi.lib")
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib,"strsafe.lib")
#pragma comment (lib,"comctl32.lib")
#pragma comment (lib,"Report_Error_DLL.lib")
#pragma comment (lib,"libmodbus-2.0.3_DLL.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <CommCtrl.h>



// TODO: reference additional headers your program requires here
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

/* TCP */
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <sys/types.h>
#include <new>
#include <list>
#include <string>
#include "libmodbus-2.0.3_DLL.h"
#include "Report_Error_DLL.h"
