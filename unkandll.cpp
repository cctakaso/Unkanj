// Copyright (C) 1998, Takashi Ogisawa - magical design co., ltd.
// 連絡先：荻沢 隆
// E-Mail Address: aau42320@par.odn.ne.jp
//    非営利目的に限り、このソースを自由に複写、改変、修正する事を
//    承諾いたします。その際にはヘッダー部分は残して下さい。
//    但し、このプログラムについて何ら保証致しません。
//    尚、商利用においても同様に使用する事ができますが、
//    若干のご承諾事項があります。メールでお問い合せ下さい。

///////////////////////////////////////////////////////////////////
// unkanjdll.cpp : DLL interface file
#include "swenv.h"
#include "unkanj.h"
#include "pp_unkanj.h"
#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////
////// Global data
HINSTANCE vhInst = null;
extern HWND vhOwnerHwnd;
extern bool vbRunning;
extern bool vbBackGroundMode;

#if defined(_MSC_VER) && defined(_DEBUG)
inline void CheckMfcDebugHeap()
{
	int tmpDbgFlag;
	/* デバック確認の為：
	 * デバッグ ヒープのフラグを設定し、ヒープのリンクリストに解放
     * したブロックを保持し、解放したメモリの不正なアクセスを検出する
	 * 様に設定しておく->これによりヒープメモリのチェックは完全になる  */
	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	//tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF; 	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF);
	//tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
}
#endif
/////////////////////////////////////////////////////////////////////////////
// DLL関連関数

BOOL WINAPI DllMain(HINSTANCE hInst, unsigned long dwReason, void far * lpvReserved)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	switch( dwReason) {
		case DLL_PROCESS_ATTACH:
			if(vhInst == NULL)
				vhInst = hInst;
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

/////////////////
static inline bool IsValidHanle(const HANDLE handle)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	return (bool)(AfxIsValidAddress((void*)handle, sizeof(CUnKanj))==TRUE); 
#else
	return handle!=null;
#endif
}

unsigned short WINAPI ccGetVersion(void)
{
	return UNKANJ_VERSION;
}

int WINAPI ccCommand(const HWND _hwnd, LPCSTR szCmdLine, LPSTR szOutput, const unsigned long wSize)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	return _command(_hwnd, szCmdLine, szOutput, wSize);
}


BOOL WINAPI ccSetOwnerWindow(const HWND _hwnd)
{
	if (!::IsWindow(_hwnd)) {
		vhOwnerHwnd = null;
		return FALSE;
	}
//	if (IsWindow(_hwnd)) {
//		vhOwnerHwnd = _hwnd;
//		return true;
//	}
//	return false;
	vhOwnerHwnd = _hwnd;
	return true;
}
/*
bool _ClearOwnerWindow(void)
{
	vhOwnerHwnd = null;
	return true;
}
*/

HANDLE WINAPI ccOpen(const HWND _hwnd, LPCSTR szCmdLine, LPSTR szOutput, const unsigned long wSize, int *_prtn)
{
	int rtn;
	CUnKanj *pkft=null;

#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	rtn=_command(_hwnd, szCmdLine, szOutput, wSize, &pkft);
	if (_prtn)
		*_prtn = rtn;
	
	if (rtn == UNKANJI_OK)
		return (HANDLE)pkft;
	return null;
}

unsigned int WINAPI ccRead(const HANDLE handle, void* lpBuf, unsigned int nCount, int *pRtn)
{
	if (!IsValidHanle(handle)) {
		return 0;
	}
#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	return ((CUnKanj *)handle)->read(lpBuf, nCount, pRtn);
}

void WINAPI ccClose(const HANDLE handle)
{
	if (!IsValidHanle(handle)) {
		return;
	}
#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	delete ((CUnKanj *)handle);
}
//bool WINAPI ccClearOwnerWindow(void)
//{
//	return _ClearOwnerWindow();
//}
bool WINAPI ccGetRunning(void)
{
	return 	vbRunning;
}
bool WINAPI ccSetBackGroundMode(const bool BackGroundMode)
{
	vbBackGroundMode = BackGroundMode;
	return true;
}

int WINAPI ccGetType(const HANDLE handle)
{
	if (!IsValidHanle(handle)) {
		return 0;
	}
#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	return ((CUnKanj *)handle)->IsCheckedCode() ? 
		((CUnKanj *)handle)->GetKanjCode():((CUnKanj *)handle)->CheckKanjCode();
}
/*
void WINAPI ccGetInfo(const HANDLE handle, )
{
	if (!IsValidHanle(handle)) {
		return 0;
	}
	return ((CUnKanj *)handle)->GetKanjCode();
}
*/
bool WINAPI ccIsEOF(const HANDLE handle)
{
	if (!IsValidHanle(handle)) {
		return 0;
	}
#if defined(_MSC_VER) && defined(_DEBUG)
	CheckMfcDebugHeap();
#endif
	return ((CUnKanj *)handle)->IsEOF();
}
/////////////////////////////////////////////////////////////////////////
#ifdef ARC_API_SUPPORT	//ARC - API

HARC _OpenArchive(const HWND _hwnd, const char * _szFileName, const unsigned long _dwMode);
char *szExtType_4Arc[MAX_SUPPORT_KANJCODES] = {
	".jis",
	".euc",
	".sjis",
	".utf8",
 };
class CUnKanj_Arc
{
public:
	CUnKanj_Arc();
	~CUnKanj_Arc();
	bool openArchive(const char * _szFileName);
	int	 closeArchive();
	int	 findFirst(const char * _szWildName, INDIVIDUALINFO *lpSubInfo);
	int	 findNext(INDIVIDUALINFO *_lpSubInfo);
private:
	int		m_ifind;
	struct _stati64  m_stat;
	string	m_filename;
	int m_ktSrc;
	int	m_ratio[MAX_SUPPORT_KANJCODES];
};

bool WINAPI ccCheckArchive(const char * _szFileName, const int _iMode)
{
	return true;
}

int WINAPI ccGetFileCount(const char * _szArcFile)
{
	return MAX_SUPPORT_KANJCODES;
}

HARC WINAPI ccOpenArchive(const HWND _hwnd, const char * _szFileName,
							  const unsigned long _dwMode)
{
	return _OpenArchive(_hwnd, _szFileName, _dwMode);
}

int WINAPI ccCloseArchive(HARC _harc)
{
	return ((CUnKanj_Arc *)_harc)->closeArchive();
}

int WINAPI ccFindFirst(HARC _harc, const char * _szWildName,
						   INDIVIDUALINFO FAR *_lpSubInfo)
{
	return ((CUnKanj_Arc *)_harc)->findFirst(_szWildName, _lpSubInfo);
}

int WINAPI ccFindNext(HARC _harc, INDIVIDUALINFO FAR *_lpSubInfo)
{
	return ((CUnKanj_Arc *)_harc)->findNext(_lpSubInfo);
}

HARC _OpenArchive(const HWND _hwnd, const char * _szFileName, const unsigned long _dwMode)
{
	CUnKanj_Arc *p= new CUnKanj_Arc;
	if (p) {
		if (!p->openArchive(_szFileName)) {
			delete p;
			p = 0;
		}
	}
	return (HARC) p;
}

CUnKanj_Arc::CUnKanj_Arc()
{
	m_ifind = 0;
	m_ktSrc = kt_ascii;
	m_ratio[0] = 0;
	m_ratio[1] = 0;
	m_ratio[2] = 0;
	m_ratio[3] = 0;
}
CUnKanj_Arc::~CUnKanj_Arc()
{
}

bool CUnKanj_Arc::openArchive(const char * szfilename)
{
/*
	fstream	file;
	file.clear();
	assert(file.rdstate()==ios::goodbit);
	file.open( szfilename, ios::in | ios::binary);
	if (!file.is_open())
		{
		return false;
	}
*/
	int fd;
	errno_t err = _sopen_s( &fd,  szfilename,
             _O_RDONLY,
             _SH_DENYWR ,
             _S_IREAD );
   if( err != 0 )
		return false;

	m_filename = szfilename;
#ifdef WIN32
	if (0!=_fstati64(fd, &m_stat)) {
		_close( fd );
		return false;
	}
	//m_size =  _filelengthi64(file.fd()) + 1; //+1 is for VK_EOF
#else
	assert(false);
	//m_size = filelength(m_file.fd());
#endif
	_close(fd);

	//check kanji code type
	HANDLE handle = ccOpen(null, "t", null, 0, null);
	if (handle) {
		m_ktSrc = (int)(((CUnKanj *)handle)->CheckKanjCode(m_ratio));
		ccClose(handle);
	}
	return true;
}

int	CUnKanj_Arc::closeArchive()
{
	delete this;
	return 0;
}
/*
	typedef struct {
		unsigned long 			dwOriginalSize;		// ファイルのサイズ
 		unsigned long 			dwCompressedSize;	// 圧縮後のサイズ
		unsigned long			dwCRC;				// 格納ファイルのチェックサム
		unsigned int			uFlag;				// 処理結果
		unsigned int			uOSType;			// 書庫作成に使われたＯＳ
		unsigned short			wRatio;				// 圧縮率
		unsigned short			wDate;				// 格納ファイルの日付(DOS 形式)
		unsigned short 			wTime;				// 格納ファイルの時刻(〃)
		char			szFileName[FNAME_MAX32 + 1];	// 書庫名
		char			dummy1[3];
		char			szAttribute[8];		// 格納ファイルの属性(書庫固有)
		char			szMode[8];			// 格納ファイルの格納モード(〃)
	}	INDIVIDUALINFO, *LPINDIVIDUALINFO;
struct _stati64 {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        __int64 st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };
#endif
*/

int	CUnKanj_Arc::findFirst(const char * _szWildName, INDIVIDUALINFO *_lpSubInfo)
{
	m_ifind=0;
	return findNext(_lpSubInfo);
}
int	CUnKanj_Arc::findNext(INDIVIDUALINFO *_lpSubInfo)
{
	memcpy(_lpSubInfo, 0, sizeof(INDIVIDUALINFO));
	if (++m_ifind>MAX_SUPPORT_KANJCODES)
		return -1;
	strncpy_s(_lpSubInfo->szFileName, FNAME_MAX32, m_filename.c_str(), min(FNAME_MAX32, m_filename.size()+1));
	_lpSubInfo->szFileName[FNAME_MAX32] = '\0';
	if (strlen(_lpSubInfo->szFileName)<(FNAME_MAX32+4)) {
		strcat_s(_lpSubInfo->szFileName, FNAME_MAX32, szExtType_4Arc[m_ifind]);
	}
	_lpSubInfo->wRatio = m_ratio[m_ifind];
	_lpSubInfo->dwOriginalSize = m_stat.st_size;
	_lpSubInfo->wTime = HIWORD(m_stat.st_mtime);
	_lpSubInfo->wDate = LOWORD(m_stat.st_mtime);
	return 0;
}

#endif //ARC_API_SUPPORT

