// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SOLA_AUTO_ID_DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SOLA_AUTO_ID_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SOLA_AUTO_ID_DLL_EXPORTS
#define SOLA_AUTO_ID_DLL_API __declspec(dllexport)
#else
#define SOLA_AUTO_ID_DLL_API __declspec(dllimport)
#endif

using namespace std;

#include <string>
#include <new>
#include <list>

#define WM_APPENDDLGTHREAD WM_APP
#define WM_APPUPDDLGTEXT WM_APP + 1
#define WM_APPAIDSCANUPDCALLBACK WM_APP + 2
#define WM_APPAIDCALLBACK WM_APP + 3
#define WM_APPAIDSCANDONECALLBACK WM_APP + 4
#define WM_APPDLGENDING WM_APP + 5
#define IDD_POLLINGDLG			214
#define ID_INIT   150
#define ID_TEXT   200

typedef void (*func_ptr)(void *pto,DWORD code);
typedef void (*scan_upd_callback_func_ptr)(void *pto,std::wstring *str_upd);
typedef void (*scan_done_callback_func_ptr)(void *pto);

uint16_t const ui16_Residential_Sola_Type_Code = 0x0100;
uint16_t const ui16_Commercial_Sola_Type_Code = 0x0101;
uint16_t const PRODUCT_TYPE_REGISTER_ADDR = 0x00b0;

int const MAX_IP_LENGTH = 20;
int const STD_MODBUS_TCP_PORT = 502;
std::string const str_COM_Prefix = "COM";
std::wstring const wstr_COM_Prefix = _T("COM");
int const i_Max_COM_Port = 10;

// This class is exported from the Sola_Auto_ID_DLL.dll
class SOLA_AUTO_ID_DLL_API CSola_Auto_ID_DLL {
public:
	enum conn_type {RTU,TCP};
	typedef struct tag_Sola_Device_Coords {
		enum conn_type ct;
		std::wstring Interface_Name;
		uint8_t ui8_addr;
	} SOLADEVICECOORDS,*LPSOLADEVICECOORDS;

	CSola_Auto_ID_DLL(uint8_t const la,uint8_t const ha);
	CSola_Auto_ID_DLL(uint8_t const la,uint8_t const ha,std::list<wstring> const *lip,int const port);
	~CSola_Auto_ID_DLL();
	inline std::list<wstring> *Get_IP_List(){return m_p_lipddn;};
	inline uint8_t Get_Low_Address(){return m_ui8_Low_Range_MB_Addr;};
	inline uint8_t Get_High_Address(){return m_ui8_High_Range_MB_Addr;};
	inline int Get_Server_Port(){return m_int_Port;};
	HANDLE Start_Auto_Discovery(void);
	inline std::list<uint8_t> *Get_Addr_List(){return m_pList_Addr_Solas;};
	inline std::list<CSola_Auto_ID_DLL::SOLADEVICECOORDS> *Get_SDC_List(){return m_p_SDCL;};
	inline func_ptr Get_Callback_Func_Ptr(){return m_pcf;};
	inline void* Get_Pointer_To_Owner() { return m_p_to_owner; };
	inline scan_upd_callback_func_ptr Get_Scan_Upd_Callback_Func_Ptr(){return m_sucfp;};
	inline scan_done_callback_func_ptr Get_Scan_Done_Callback_Func_Ptr(){return m_sdcfp;};
	inline enum conn_type Get_Conn_Type(){return m_connection;};
	inline int Get_TCP_Port(){return m_int_Port;};
	void Set_Callback_Func_Ptr(void *p);
	void Set_Scan_Upd_Callback_Func_Ptr(void *p);
	void Set_Scan_Done_Callback_Func_Ptr(void *p);
	void Set_Pointer_To_Owner(void *p);
	// TODO: add your methods here.
private:
	size_t m_st_ip_len;
	std::list<uint8_t> *m_pList_Addr_Solas;
	std::string *m_pstr_IP;
	std::wstring *m_pwstr_IP;
	std::list<wstring> *m_p_lipddn;
	int m_int_Port;
	uint8_t m_ui8_Low_Range_MB_Addr;
	uint8_t m_ui8_High_Range_MB_Addr;
	static DWORD WINAPI Work_Thread(LPARAM l_p);
	HANDLE m_h_Work_Thread;
	HANDLE m_hdup_Work_Thread;
	DWORD m_dw_Work_Thread_ID;
	func_ptr m_pcf;
	scan_upd_callback_func_ptr m_sucfp;
	scan_done_callback_func_ptr m_sdcfp;
	HANDLE m_h_Cancel_Event;
	std::list<CSola_Auto_ID_DLL::SOLADEVICECOORDS> *m_p_SDCL;
	conn_type m_connection;
	void *m_p_to_owner;
};

class SOLA_AUTO_ID_DLL_API Ctrash81_Modeless_Dlg_DLL {
public:
	Ctrash81_Modeless_Dlg_DLL(void);
	Ctrash81_Modeless_Dlg_DLL(HWND hOwner,HINSTANCE hInst,TCHAR* szTitle);
	~Ctrash81_Modeless_Dlg_DLL(void);
	// TODO: add your methods here.

	typedef struct _tagDlgThreadParms {
		std::wstring const *p_ip;} DLGTHREADPARMS,*LPDLGTHREADPARMS;
	typedef struct tag_Conn_Type_Parms {
		CSola_Auto_ID_DLL::conn_type ct;
		std::list<wstring> *p_lipddn;
		std::list<wstring> *p_port;
	} CONNTYPEPARMS,*LPCONNTYPEPARMS;

	typedef void (*pf_callback)(void *p_v);
	typedef void (*pf_upd_callback)(std::wstring *p_upd_str);
	typedef void (*pf_scan_done_handler)(void *p_v);

	BOOL Run(LPCONNTYPEPARMS lp_ctp);
	BOOL Stop();
	inline HWND Get_Caller_HWND() { return m_hw_caller; };
	inline pf_callback Get_Callback(void){return m_p_callback_func;};
	inline pf_upd_callback Get_Upd_Callback(void){return m_p_upd_callback_func;};
	inline CSola_Auto_ID_DLL *Get_SID(){return m_p_sid;};
	inline int Get_TCP_Port(){return m_i_Port;};
	void Set_TCP_Port(int p);
	void Set_Callback(pf_callback p);
	void Set_Scan_Done_Handler(pf_scan_done_handler p_f);
	void Set_Caller_HWND(HWND &hw);
	HGLOBAL Make_Dlg_Template(LPSTR lpszMessage);

private:
	static DWORD WINAPI DlgThreadProc(LPVOID lParam);
	static INT_PTR CALLBACK DlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
	static void Upd_Callback_Func(std::wstring *p_str);
	static void Auto_ID_Callback(void *pto,DWORD dw_code);
	static void Auto_ID_Scan_Upd_Callback(void *pto,std::wstring *p);
	static void Auto_ID_Scan_Done_Callback(void *pto);
	void Scan_Done_Handler(void);
	int m_i_Port;
	HGLOBAL m_h_Dlg_Glbl;
	LPDLGTEMPLATE m_p_Dlg_Tmpl;
	LPDLGITEMTEMPLATE m_p_Dlg_Item_Tmpl;
	HWND m_hwndOwner;
	HWND m_hw_caller;
	HANDLE m_hDlgQuitEvent;
	TCHAR* m_szDlgQuitEvent;
	TCHAR* m_szDlgRdyEvent;
	HWND m_hwndDlg;
	HANDLE m_hDlgRdyEvent;
	HANDLE m_hDlgThread;
	HANDLE m_hdupDlgThread;
	DWORD m_dwDlgThreadID;
	HINSTANCE m_hInst;
	TCHAR* m_szTitle;
	pf_callback m_p_callback_func;
	pf_upd_callback m_p_upd_callback_func;
	pf_scan_done_handler m_pf_scan_done_handler;
	LPWORD lpwAlign(LPWORD lpIn);
	std::list<wstring> *m_p_lipddn;
	CSola_Auto_ID_DLL::conn_type m_ct;
	CSola_Auto_ID_DLL *m_p_sid;
};
