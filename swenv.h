// Copyright (C) 1998, Takashi Ogisawa - magical design co., ltd.
// 連絡先：荻沢 隆
// E-Mail Address: aau42320@par.odn.ne.jp
//    非営利目的に限り、このソースを自由に複写、改変、修正する事を
//    承諾いたします。その際にはヘッダー部分は残して下さい。
//    但し、このプログラムについて何ら保証致しません。
//    尚、商利用においても同様に使用する事ができますが、
//    若干の承諾条件があります。詳しくはメールにてお問い合わせ下さい。
#ifndef __SWENV_H__
#define __SWENV_H__
#ifdef WIN32
	#pragma message ("<< WINDOWS VERSION >>")
	#if defined(_MSC_VER) && defined(_DEBUG)
		#include <afxwin.h> //for afxDump() only.
		#pragma message ("<< include afxwin.h >>")
		#undef _MFC_VER
	#else
		#include <windows.h>
	#endif
#endif


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <fcntl.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <string>
#include <io.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <exception>
#include <deque>	/* Compile options needed:-GX */

#ifdef _MSC_VER
#pragma warning(disable:4786)
	#pragma message ("<< MSC >>")
	#define  fstream	::fstream	//for error C2872: 'fstream' : symbol error
	#define  endl		::endl

//	#define cout afxDump

	//extern void _assert(char *exp, char *szFile, int line);
//	#define bool BOOL

	#include <direct.h> //for _mkdir
	#define mkdir _mkdir
	#define getcwd	_getcwd
	#ifndef msize
	#define msize	_msize
	#endif
#endif
#include <map>
#include <list>
#include <vector>
using namespace std ;

///////////////////////////////////////
////// Public defines.
#define null (0)
#ifndef _MAX_PATH
	#define _MAX_PATH 512
#endif
#define __MAX_PATH 1024
typedef unsigned char     BYTE;
#ifndef max
	#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
	#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef LOWORD
	#define LOWORD(l)           ((unsigned short)(l))
#endif
#ifndef HIWORD
	#define HIWORD(l)           ((unsigned short)(((unsigned long)(l) >> 16) & 0xFFFF))
#endif
#define CH_DRV	((BYTE)':')
#define SZ_NET_DIR	"\\\\"

/////////////STANDERD C++ LIBRARY VERSION//////////
#ifdef WIN32
	#define CH_DIR	((BYTE)'\\')
	#define SZ_DIR	"\\"
	#define SZ_DRV	":"
#else
	#define CH_DIR	((BYTE)'/')
	#define SZ_DIR	"/"
	#define SZ_DRV	":"
#endif
string GetFileName(const string &strSrc);
/////////////STANDERD C++ LIBRARY VERSION//////////
//<------------------- VERSION CONTROL CODES //////////////////////
#endif //__SWENV_H__
