// Copyright (C) 1998, Takashi Ogisawa - magical design co., ltd.
// 連絡先：荻沢 隆
// E-Mail Address: aau42320@par.odn.ne.jp
//    非営利目的に限り、このソースを自由に複写、改変、修正する事を
//    承諾いたします。その際にはヘッダー部分は残して下さい。
//    但し、このプログラムについて何ら保証致しません。
//    尚、商利用においても同様に使用する事ができますが、
//    若干のご承諾事項があります。メールでお問い合せ下さい。
#ifndef __PP_UNKANJ_H__
#define __PP_UNKANJ_H__

#ifdef _MSC_VER
	#define bool BOOL
#endif

/////////////////////////////////////////////////////////////////////////////
typedef unsigned int (*CONVPROC)(unsigned int);
#define DEF_READ_SIZE	(0x10000)	//64Kbyte
enum CommandType {
	CT_TRANS,
	CT_CHECK_SRC_KANJTYPE,
	CT_HELP,
};
enum LineEnd{
	LE_NO,
	LE_CRLF,
	LE_LF,
	LE_CR,
};
/////////////////////////////////////////////////////////////////////////////
class CUnKanj;
bool ParseSwitchs(string &strCmdLine, CUnKanj *pkft, string &strSrcFileName, string &strDesFileName);
int _command(const HWND _hwnd, const char * szCmdLine, char * szOutput=null, const unsigned long wSize=0, CUnKanj **ppkft=null);

class CUnKanj
{
	friend bool ParseSwitchs(string &strCmdLine, CUnKanj *pkft, string &strSrcFileName, string &strDesFileName);
	friend int _command(const HWND _hwnd, const char * szCmdLine, char * szOutput, const unsigned long wSize, CUnKanj **ppkft);
public:
	enum flg_shift{
		fs_no=0,
		fs_kanji,
		fs_hankana,
	};
	enum flg_unknown_save {
		fus_no,
		fus_savestart,
		fus_readstart,
	};
public:
	unsigned int m_uReadSize;	// リードバッファサイズ

protected:

	fstream	m_fileSrc;			// ソースファイル
	BYTE *m_pReadBuff;			// フィードバッファポインタ
	BYTE *m_pWriteBuff;			// フィードバッファポインタ
	unsigned int m_sizeWrite;
	long	m_lSeek;				//File 
	unsigned int	m_sizeRead;
	unsigned int	m_lim1;
	unsigned int	m_lim2;
	bool m_bEOF;

	CommandType m_ct;
	KanjType m_ktSrc;
	KanjType m_ktDes;			//destination
	LineEnd m_le;
	char m_chKI;
	char m_chKO;
	bool m_bGaiji;

	unsigned int m_uWriteInval;
	bool	m_bFstAnsi;
	flg_shift	m_flgShift;
	CONVPROC	m_convProc;
public:
	CUnKanj();

	virtual ~CUnKanj();
public:
	bool open(const char *pSrcFileName,
		//unsigned int nOpenFlags=(CFile::modeRead | CFile::typeBinary));//BYTE *lpOutPut=null, unsigned long wSize=0, unsigned int uReadSize=0x10000/*32Kbytes*/);
		unsigned int nOpenFlags=(ios::in | ios::binary ));//| ios::nocreate
	//BYTE *TransCode(bool bFirst);
	unsigned int read( void* lpBuf, unsigned int nCount, int *pRtn=null );
	int CheckKanjCode(int *pratios=null);
	void close();
	int GetKanjCode(void) {
		return m_ktSrc; }
	bool IsCheckedCode() {
		return m_lSeek!=0; }
	bool IsEOF(void) {
		return m_bEOF; }

protected:
	bool ReadCh(BYTE *&pch, flg_unknown_save fus=fus_no);
	bool WriteCh(BYTE *&pch);
	void CleanUp();
	void SetJisFlgShift(BYTE *&pwh, flg_shift fs=fs_no);
	bool SetKanjType_ReadCh(KanjType ktSrc, BYTE *&pch) {
		ReadCh(pch, fus_readstart);
		return SetKanjType(ktSrc);
	}
	bool SetKanjType(KanjType ktSrc);
	inline bool InvalWrite(BYTE *&pwh, int *&pRtn, bool &bNoConv);
	inline int ConvLineEnd(BYTE *&pwh);
	unsigned int ReadOnly_ConvLineEnd(BYTE* pwh, unsigned int nCount);
	inline unsigned int ReadOnly( void* lpBuf, unsigned int nCount, bool bBegin=true);
private:
	BYTE *pchLim_ReadCh;
	BYTE *pchLimReal_ReadCh;
	BYTE *pchFus_ReadCh;
	bool bEOF_ReadCh;

	BYTE *pwhLim_WriteCh;

//関数別アクセス変数
	BYTE *m_pch;
	BYTE m_ch, m_ch1, m_ch2;
	unsigned int  m_chType, m_chType1, m_chType2;
	bool m_bNoConv;
};


//////////////////////////////////////////
// inline functions
inline void TraceException(const exception &e)
{
	#ifdef _DEBUG
	cout << e.what();
	#endif
}

#endif //__PP_UNKANJ_H__
