// Copyright (C) 1998, Takashi Ogisawa - magical design co., ltd.
// 連絡先：荻沢 隆
// E-Mail Address: aau42320@par.odn.ne.jp
//    非営利目的に限り、このソースを自由に複写、改変、修正する事を
//    承諾いたします。その際にはヘッダー部分は残して下さい。
//    但し、このプログラムについて何ら保証致しません。
//    尚、商利用においても同様に使用する事ができますが、
//    若干のご承諾事項があります。メールでお問い合せ下さい。

/////////////////////////////////////////////////////
//機　能　概　要
// Unkanjは現在以下の機能・特徴を有しています
//	1.漢字コードの相互変換機能
//		1) JIS  -  7bitJIS
//			ISO-2022-JP
//			JIS X 0208-1983(90)
//			JIS X 0208-1978
//		2) EUC - AT&T JIS
//			115～119区（IBM 拡張文字）については OSF/JVC（Open Software 
//			Foundation 日本ベンダ協議会）が96年に定めた変換テーブルによる
//		3) Shift-Jis - MS Kanj
//         JIS X 0208-1990 をベースに、NEC 特殊文字（13区）、
//			NEC 選定 IBM 拡張文字（89～92区）、IBM 拡張文字（115～119区）
//     ※EUC<->SJISとの外字(IBM 拡張文字(394文字))相互変換は、
//                     ENABLE_IBM_EXPAND_CODESデファイン時のみ
//	2.漢字コードの自動認識機能
//　3.UNIX, DOS/WINDOWS, MAC　改行コードの相互変換機能
//  4.超高速コード判定＆変換
//　5.マルチタスク対応
//  6.ユーザープログラムに組み込み使用できるインタフェース機能
//		ANSI標準Cファイルリード関数及び統合アーカイバＡＰＩ仕様に準ずる
//　7.内部的には、ANSI標準C++に準拠したソースコードです
#include "swenv.h"
#include "unkanj.h"
#include "pp_unkanj.h"
//using namespace std;

///////////////////////////////////////
////// Display usage.
char svzHelp[] = 
"unakanj ver 0.05 - kanj filter utility.\n"\
" Copyright (C) 1998.10, Takashi Ogisawa - aau42320@par.odn.ne.jp\n"\
"usage: unkanj [<command>] [<-switch> ...] <souce_file_name> [<dest_file_name>]\n"\
"<<command>>     t :convert kanji code\n"\
"                c :check the srouce file kanji code\n"\
"                ? :give this help\n"\
"<<switch>>\n"\
"  output code  -j :convert to jis code\n"\
"               -e :convert to euc code\n"\
"               -s :convert to sift-jis code\n"\
"               -u :convert to utf-8 code\n"\
"  input code   -J :convert from jis code\n"\
"               -E :convert from euc code\n"\
"               -S :convert from sift-jis code\n"\
"               -U :convert from utf-8 code\n"\
"  line end    -cr :convert to CR code at the line end.(mac file)\n"\
"              -lf :convert to LF code at the line end.(unix file)\n"\
"            -crlf :convert to CRLF code at the line end.(dos/win)\n"\
"  JIS KI/KO -kib -kob :KI=[ESC]$B KO=[ESC](B (default)\n"\
"            -ki@ -koj :KI=[ESC]$@ KO=[ESC](J \n"\
"            -kib -koj :KI=[ESC]$B KO=[ESC](J \n"\
"            -kik -koh :KI=[ESC]K  KO=[ESC]H  \n"\
"  read buffer size  -r<nnn> :nnn kbytes \n"\
"  GAIJI euc<=>sjis  -g<0/1> :0=off(default) / 1=on"\
;

char *svzKanjType[] = {
"check ok -> may be <<binary>> file.",
"check ok -> may be <<ascii>> file.",
"check ok -> may be <<jis>> file.",
"check ok -> may be <<euc>> file.",
"check ok -> may be <<shift-jis>> file.",
"check ok -> may be <<utf-8>> file.",
};

///////////////////////////////////////
////// global data define
HWND vhOwnerHwnd=null;
bool vbRunning = false;
bool vbBackGroundMode = false;
///////////////////////////////////////
////// global function defines
unsigned int jis2sj(unsigned int c);
unsigned int sj2jis(unsigned int c);
unsigned int euc2jis(unsigned int c);
unsigned int jis2euc(unsigned int c);
unsigned int euc2sj(unsigned int c);
unsigned int sj2euc(unsigned int c);
unsigned int jis2jis(unsigned int c);
unsigned int sj2utf8(unsigned int c);
unsigned int utf82sj(unsigned int c);

string GetPartStringOfPath(string &strSrc);
string GetPartString(string &strSrc, char ch);
bool FCreateDirectory(const char * lpszPath);
string GetCurrentPath(void);
//////////////////////////////////////////
//static data
#define         NL      0x0a
#define         ESC     0x1b
#define         SP      0x20
#define         AT      0x40
#define         SSP     0xa0
#define         DEL     0x7f
#define         SI      0x0e
#define         SO      0x0f
#define         SS2     0x8e
#define         SS3     0x8f
#define INV_MAKEWORD(a, b)      ((unsigned short)(((BYTE)(b)) | ((unsigned short)((BYTE)(a))) << 8))
//#define MAKEWORD(a, b)      ((unsigned short)(((BYTE)(a)) | ((unsigned short)((BYTE)(b))) << 8))

#define ANS			1
#define JIS			2
#define EUC         4      /* Extended UNIX en-Coding of KANJI */
#define SJ1			8      /* The first byte of shift-JIS KANJI    */
#define SJ2			0x10   /* The second byte of shift-JIS KANJI   */
#define HANKATA     0x20   /* "half-width" KATAKANA            */
#define CJ			0x40
#define	CE			0x80
#define UEF2		0x100	/* utf-8 2bytes word first character */
#define UEF3		0x200	/* utf-8 3bytes word first character */
#define UEC			0x400	/* utf-8 2,3～bytes word not first character */
#define ANJ			(ANS | JIS)
#define AJSJ2		(ANS | JIS  | SJ2)
#define SJ12        (SJ1 | SJ2)
#define SJ12UEC     (SJ12 | UEC)

#define ES2H        (EUC | SJ2 | HANKATA)
#define ES2HUEC     (ES2H | UEC)
#define ES2HUEF2	(ES2H | UEF2)
#define ESJ12       (EUC | SJ1 | SJ2)
#define ESJ12UEF3   (ESJ12 | UEF3)
#define ANS_JIS		(ANS | JIS | CJ | HANKATA)	//ans or jis
#define ANS_EUC		(ANS | EUC | CE | HANKATA)	//ans or euc
#define ANS_SJIS	(ANS | SJ12 | HANKATA)		//ans or sjis
#define GetType(ch)		vchTypeTbl[0xffff & (ch)]
#define IsValidCode(p)	(p!=0)
/*
 * Character classifier for KANJI.
 */
const unsigned int   vchTypeTbl[0x100] = {
//   0,     1,     2,     3,     4,     5,     6,     7,
//   8,     9,     A,     B,     C,     D,     E,     F,
   /*000 0x00をANSとしておく*/
   ANS,   000,   000,   000,   000,   000,   000,   000,    /* 00           */
   000,   ANS,   ANS,   000,   000,   ANS,   CJ,    CJ,     /* 08           */
   000,   000,   000,   000,   000,   000,   000,   000,    /* 10           */
   000,   000,   000,   CJ,    000,   000,   000,   000,    /* 18           */
   ANS,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,    /* 20  !"#$%&'  */
   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,    /* 28 ()*+,-./  */
   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,    /* 30 01234567  */
   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,   ANJ,    /* 38 89:;<=>?  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 40 @ABCDEFG  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 48 HIJKLMNO  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 50 PQRSTUVW  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 58 XYZ[\]^_  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 60 `abcdefg  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 68 hijklmno  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2,  /* 70 pqrstuvw  */
   AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, AJSJ2, 000,    /* 78 xyz{|}~   */

   SJ2|UEC,   SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,   /*   80 .. 87   */
   SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12|CE|UEC,  SJ12|CE|UEC,   /*   88 .. 8F   */
   SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,   /*   90 .. 97   */
   SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,  SJ12UEC,   /*   98 .. 9F   */
   SJ2|UEC,   ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,   /*   A0 .. A7   */
   ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,   /*   A8 .. AF   */
   ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,   /*   B0 .. B7   */
   ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,  ES2HUEC,   /*   B8 .. BF   */

   ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,   /*   C0 .. C7   */
   ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,   /*   C8 .. CF   */
   ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,   /*   D0 .. D7   */
   ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,  ES2HUEF2,   /*   D8 .. DF   */

   ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3,  /*   E0 .. E7   */
   ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3, ESJ12UEF3,  /*   E8 .. EF   */

   ESJ12, ESJ12, ESJ12, ESJ12, ESJ12, ESJ12, ESJ12, ESJ12,  /*   F0 .. F7   */
   ESJ12, ESJ12, ESJ12, ESJ12, ESJ12, EUC,   EUC,   000,    /*   F8 .. FF   */
};

//////////////////////////////////////////////////////////////
//Gaiji tables of 115～119区（IBM 拡張文字）
//     ※EUC<->SJISとの外字(IBM 拡張文字(394文字))相互変換は、
//                     ENABLE_IBM_EXPAND_CODESデファイン時のみ
//TODO:if you don't want to support ibm_expand_codes, so you can comment out this define.
#define ENABLE_IBM_EXPAND_CODES

#ifdef ENABLE_IBM_EXPAND_CODES
//SJIS:fa40->fc4b IBM 拡張文字
const unsigned short _IBM_sjis2euc[] = {
0xf3f3,0xf3f4,0xf3f5,0xf3f6,0xf3f7,0xf3f8,0xf3f9,0xf3fa,0xf3fb,0xf3fc,0xf3fd,0xf3fe,
0xf4a1,0xf4a2,0xf4a3,0xf4a4,0xf4a5,0xf4a6,0xf4a7,0xf4a8,0xa2cc,0xa2c3,0xf4a9,0xf4aa,
0xf4ab,0xf4ac,0xf4ad,0xa2e8,0xd4e3,0xdcdf,0xe4e9,0xe3f8,0xd9a1,0xb1bb,0xf4ae,0xc2ad,
0xc3fc,0xe4d0,0xc2bf,0xbcf4,0xb0a9,0xb0c8,0xf4af,0xb0d2,0xb0d4,0xb0e3,0xb0ee,0xb1a7,
0xb1a3,0xb1ac,0xb1a9,0xb1be,0xb1df,0xb1d8,0xb1c8,0xb1d7,0xb1e3,0xb1f4,0xb1e1,0xb2a3,
0xf4b0,0xb2bb,0xb2e6,0x0000,0xb2ed,0xb2f5,0xb2fc,0xf4b1,0xb3b5,0xb3d8,0xb3db,0xb3e5,
0xb3ee,0xb3fb,0xf4b2,0xf4b3,0xb4c0,0xb4c7,0xb4d0,0xb4de,0xf4b4,0xb5aa,0xf4b5,0xb5af,
0xb5c4,0xb5e8,0xf4b6,0xb7c2,0xb7e4,0xb7e8,0xb7e7,0xf4b7,0xf4b8,0xf4b9,0xb8ce,0xb8e1,
0xb8f5,0xb8f7,0xb8f8,0xb8fc,0xb9af,0xb9b7,0xbabe,0xbadb,0xcdaa,0xbae1,0xf4ba,0xbaeb,
0xbbb3,0xbbb8,0xf4bb,0xbbca,0xf4bc,0xf4bd,0xbbd0,0xbbde,0xbbf4,0xbbf5,0xbbf9,0xbce4,
0xbced,0xbcfe,0xf4be,0xbdc2,0xbde7,0xf4bf,0xbdf0,0xbeb0,0xbeac,0xf4c0,0xbeb3,0xbebd,
0xbecd,0xbec9,0xbee4,0xbfa8,0xbfc9,0xc0c4,0xc0e4,0xc0f4,0xc1a6,0xf4c1,0xc1f5,0xc1fc,
0xf4c2,0xc1f8,0xc2ab,0xc2a1,0xc2a5,0xf4c3,0xc2b8,0xc2ba,0xf4c4,0xc2c4,0xc2d2,0xc2d7,
0xc2db,0xc2de,0xc2ed,0xc2f0,0xf4c5,0xc3a1,0xc3b5,0xc3c9,0xc3b9,0xf4c6,0xc3d8,0xc3fe,
0xf4c7,0xc4cc,0xf4c8,0xc4d9,0xc4ea,0xc4fd,0xf4c9,0xc5a7,0xc5b5,0xc5b6,0xf4ca,0xc5d5,
0xc6b8,0xc6d7,0xc6e0,0xc6ea,0xc6e3,0xc7a1,0xc7ab,0xc7c7,0xc7c3,0xc7cb,0xc7cf,0xc7d9,
0xf4cb,0xf4cc,0xc7e6,0xc7ee,0xc7fc,0xc7eb,0xc7f0,0xc8b1,0xc8e5,0xc8f8,0xc9a6,0xc9ab,
0xc9ad,0xf4cd,0xc9ca,0xc9d3,0xc9e9,0xc9e3,0xc9fc,0xc9f4,0xc9f5,0xf4ce,0xcab3,0xcabd,
0xcaef,0xcaf1,0xcbae,0xf4cf,0xcbca,0xcbe6,0xcbea,0xcbf0,0xcbf4,0xcbee,0xcca5,0xcbf9,
0xccab,0xccae,0xccad,0xccb2,0xccc2,0xccd0,0xccd9,0xf4d0,0xcdbb,0xf4d1,0xcebb,0xf4d2,
0xceba,0xcec3,0xf4d3,0xcef2,0xb3dd,0xcfd5,0xcfe2,0xcfe9,0xcfed,0xf4d4,0xf4d5,0xf4d6,
0x0000,0xf4d7,0xd0e5,0xf4d8,0xd0e9,0xd1e8,0xf4d9,0xf4da,0xd1ec,0xd2bb,0xf4db,0xd3e1,
0xd3e8,0xd4a7,0xf4dc,0xf4dd,0xd4d4,0xd4f2,0xd5ae,0xf4de,0xd7de,0xf4df,0xd8a2,0xd8b7,
0xd8c1,0xd8d1,0xd8f4,0xd9c6,0xd9c8,0xd9d1,0xf4e0,0xf4e1,0xf4e2,0xf4e3,0xf4e4,0xdcd3,
0xddc8,0xddd4,0xddea,0xddfa,0xdea4,0xdeb0,0xf4e5,0xdeb5,0xdecb,0xf4e6,0xdfb9,0xf4e7,
0xdfc3,0xf4e8,0xf4e9,0xe0d9,0xf4ea,0xf4eb,0xe1e2,0xf4ec,0xf4ed,0xf4ee,0xe2c7,0xe3a8,
0xe3a6,0xe3a9,0xe3af,0xe3b0,0xe3aa,0xe3ab,0xe3bc,0xe3c1,0xe3bf,0xe3d5,0xe3d8,0xe3d6,
0xe3df,0xe3e3,0xe3e1,0xe3d4,0xe3e9,0xe4a6,0xe3f1,0xe3f2,0xe4cb,0xe4c1,0xe4c3,0xe4be,
0xf4ef,0xe4c0,0xe4c7,0xe4bf,0xe4e0,0xe4de,0xe4d1,0xf4f0,0xe4dc,0xe4d2,0xe4db,0xe4d4,
0xe4fa,0xe4ef,0xe5b3,0xe5bf,0xe5c9,0xe5d0,0xe5e2,0xe5ea,0xe5eb,0xf4f1,0xf4f2,0xf4f3,
0xe6e8,0xe6ef,0xe7ac,0xf4f4,0xe7ae,0xf4f5,0xe7b1,0xf4f6,0xe7b2,0xe8b1,0xe8b6,0xf4f7,
0xf4f8,0xe8dd,0xf4f9,0xf4fa,0xe9d1,0xf4fb,0xe9ed,0xeacd,0xf4fc,0xeadb,0xeae6,0xeaea,
0xeba5,0xebfb,0xebfa,0xf4fd,0xecd6,0xf4fe,
};
//euc:a2c3->f4fe IBM expand character
const unsigned short _IBM_euc2sjis[][2] = {
0xfa55,0xa2c3,/*0xfa54,0xa2cc,0xfa5b,0xa2e8,*/0xfa68,0xb0a9,0xfa69,0xb0c8,0xfa6b,0xb0d2
,0xfa6c,0xb0d4,0xfa6d,0xb0e3,0xfa6e,0xb0ee,0xfa70,0xb1a3,0xfa6f,0xb1a7,0xfa72,0xb1a9
,0xfa71,0xb1ac,0xfa61,0xb1bb,0xfa73,0xb1be,0xfa76,0xb1c8,0xfa77,0xb1d7,0xfa75,0xb1d8
,0xfa74,0xb1df,0xfa7a,0xb1e1,0xfa78,0xb1e3,0xfa79,0xb1f4,0xfa7b,0xb2a3,0xfa7d,0xb2bb
,0xfa7e,0xb2e6,0xfa80,0xb2ed,0xfa81,0xb2f5,0xfa82,0xb2fc,0xfa84,0xb3b5,0xfa85,0xb3d8
,0xfa86,0xb3db,0xfb77,0xb3dd,0xfa87,0xb3e5,0xfa88,0xb3ee,0xfa89,0xb3fb,0xfa8c,0xb4c0
,0xfa8d,0xb4c7,0xfa8e,0xb4d0,0xfa8f,0xb4de,0xfa91,0xb5aa,0xfa93,0xb5af,0xfa94,0xb5c4
,0xfa95,0xb5e8,0xfa97,0xb7c2,0xfa98,0xb7e4,0xfa9a,0xb7e7,0xfa99,0xb7e8,0xfa9e,0xb8ce
,0xfa9f,0xb8e1,0xfaa0,0xb8f5,0xfaa1,0xb8f7,0xfaa2,0xb8f8,0xfaa3,0xb8fc,0xfaa4,0xb9af
,0xfaa5,0xb9b7,0xfaa6,0xbabe,0xfaa7,0xbadb,0xfaa9,0xbae1,0xfaab,0xbaeb,0xfaac,0xbbb3
,0xfaad,0xbbb8,0xfaaf,0xbbca,0xfab2,0xbbd0,0xfab3,0xbbde,0xfab4,0xbbf4,0xfab5,0xbbf5
,0xfab6,0xbbf9,0xfab7,0xbce4,0xfab8,0xbced,0xfa67,0xbcf4,0xfab9,0xbcfe,0xfabb,0xbdc2
,0xfabc,0xbde7,0xfabe,0xbdf0,0xfac0,0xbeac,0xfabf,0xbeb0,0xfac2,0xbeb3,0xfac3,0xbebd
,0xfac5,0xbec9,0xfac4,0xbecd,0xfac6,0xbee4,0xfac7,0xbfa8,0xfac8,0xbfc9,0xfac9,0xc0c4
,0xfaca,0xc0e4,0xfacb,0xc0f4,0xfacc,0xc1a6,0xface,0xc1f5,0xfad1,0xc1f8,0xfacf,0xc1fc
,0xfad3,0xc2a1,0xfad4,0xc2a5,0xfad2,0xc2ab,0xfa63,0xc2ad,0xfad6,0xc2b8,0xfad7,0xc2ba
,0xfa66,0xc2bf,0xfad9,0xc2c4,0xfada,0xc2d2,0xfadb,0xc2d7,0xfadc,0xc2db,0xfadd,0xc2de
,0xfade,0xc2ed,0xfadf,0xc2f0,0xfae1,0xc3a1,0xfae2,0xc3b5,0xfae4,0xc3b9,0xfae3,0xc3c9
,0xfae6,0xc3d8,0xfa64,0xc3fc,0xfae7,0xc3fe,0xfae9,0xc4cc,0xfaeb,0xc4d9,0xfaec,0xc4ea
,0xfaed,0xc4fd,0xfaef,0xc5a7,0xfaf0,0xc5b5,0xfaf1,0xc5b6,0xfaf3,0xc5d5,0xfaf4,0xc6b8
,0xfaf5,0xc6d7,0xfaf6,0xc6e0,0xfaf8,0xc6e3,0xfaf7,0xc6ea,0xfaf9,0xc7a1,0xfafa,0xc7ab
,0xfafc,0xc7c3,0xfafb,0xc7c7,0xfb40,0xc7cb,0xfb41,0xc7cf,0xfb42,0xc7d9,0xfb45,0xc7e6
,0xfb48,0xc7eb,0xfb46,0xc7ee,0xfb49,0xc7f0,0xfb47,0xc7fc,0xfb4a,0xc8b1,0xfb4b,0xc8e5
,0xfb4c,0xc8f8,0xfb4d,0xc9a6,0xfb4e,0xc9ab,0xfb4f,0xc9ad,0xfb51,0xc9ca,0xfb52,0xc9d3
,0xfb54,0xc9e3,0xfb53,0xc9e9,0xfb56,0xc9f4,0xfb57,0xc9f5,0xfb55,0xc9fc,0xfb59,0xcab3
,0xfb5a,0xcabd,0xfb5b,0xcaef,0xfb5c,0xcaf1,0xfb5d,0xcbae,0xfb5f,0xcbca,0xfb60,0xcbe6
,0xfb61,0xcbea,0xfb64,0xcbee,0xfb62,0xcbf0,0xfb63,0xcbf4,0xfb66,0xcbf9,0xfb65,0xcca5
,0xfb67,0xccab,0xfb69,0xccad,0xfb68,0xccae,0xfb6a,0xccb2,0xfb6b,0xccc2,0xfb6c,0xccd0
,0xfb6d,0xccd9,0xfaa8,0xcdaa,0xfb6f,0xcdbb,0xfb73,0xceba,0xfb71,0xcebb,0xfb74,0xcec3
,0xfb76,0xcef2,0xfb78,0xcfd5,0xfb79,0xcfe2,0xfb7a,0xcfe9,0xfb7b,0xcfed,0xfb81,0xd0e5
,0xfb83,0xd0e9,0xfb84,0xd1e8,0xfb87,0xd1ec,0xfb88,0xd2bb,0xfb8a,0xd3e1,0xfb8b,0xd3e8
,0xfb8c,0xd4a7,0xfb8f,0xd4d4,0xfa5c,0xd4e3,0xfb90,0xd4f2,0xfb91,0xd5ae,0xfb93,0xd7de
,0xfb95,0xd8a2,0xfb96,0xd8b7,0xfb97,0xd8c1,0xfb98,0xd8d1,0xfb99,0xd8f4,0xfa60,0xd9a1
,0xfb9a,0xd9c6,0xfb9b,0xd9c8,0xfb9c,0xd9d1,0xfba2,0xdcd3,0xfa5d,0xdcdf,0xfba3,0xddc8
,0xfba4,0xddd4,0xfba5,0xddea,0xfba6,0xddfa,0xfba7,0xdea4,0xfba8,0xdeb0,0xfbaa,0xdeb5
,0xfbab,0xdecb,0xfbad,0xdfb9,0xfbaf,0xdfc3,0xfbb2,0xe0d9,0xfbb5,0xe1e2,0xfbb9,0xe2c7
,0xfbbb,0xe3a6,0xfbba,0xe3a8,0xfbbc,0xe3a9,0xfbbf,0xe3aa,0xfbc0,0xe3ab,0xfbbd,0xe3af
,0xfbbe,0xe3b0,0xfbc1,0xe3bc,0xfbc3,0xe3bf,0xfbc2,0xe3c1,0xfbca,0xe3d4,0xfbc4,0xe3d5
,0xfbc6,0xe3d6,0xfbc5,0xe3d8,0xfbc7,0xe3df,0xfbc9,0xe3e1,0xfbc8,0xe3e3,0xfbcb,0xe3e9
,0xfbcd,0xe3f1,0xfbce,0xe3f2,0xfa5f,0xe3f8,0xfbcc,0xe4a6,0xfbd2,0xe4be,0xfbd6,0xe4bf
,0xfbd4,0xe4c0,0xfbd0,0xe4c1,0xfbd1,0xe4c3,0xfbd5,0xe4c7,0xfbcf,0xe4cb,0xfa65,0xe4d0
,0xfbd9,0xe4d1,0xfbdc,0xe4d2,0xfbde,0xe4d4,0xfbdd,0xe4db,0xfbdb,0xe4dc,0xfbd8,0xe4de
,0xfbd7,0xe4e0,0xfa5e,0xe4e9,0xfbe0,0xe4ef,0xfbdf,0xe4fa,0xfbe1,0xe5b3,0xfbe2,0xe5bf
,0xfbe3,0xe5c9,0xfbe4,0xe5d0,0xfbe5,0xe5e2,0xfbe6,0xe5ea,0xfbe7,0xe5eb,0xfbeb,0xe6e8
,0xfbec,0xe6ef,0xfbed,0xe7ac,0xfbef,0xe7ae,0xfbf1,0xe7b1,0xfbf3,0xe7b2,0xfbf4,0xe8b1
,0xfbf5,0xe8b6,0xfbf8,0xe8dd,0xfbfb,0xe9d1,0xfc40,0xe9ed,0xfc41,0xeacd,0xfc43,0xeadb
,0xfc44,0xeae6,0xfc45,0xeaea,0xfc46,0xeba5,0xfc48,0xebfa,0xfc47,0xebfb,0xfc4a,0xecd6
,0xfa40,0xf3f3,0xfa41,0xf3f4,0xfa42,0xf3f5,0xfa43,0xf3f6,0xfa44,0xf3f7,0xfa45,0xf3f8
,0xfa46,0xf3f9,0xfa47,0xf3fa,0xfa48,0xf3fb,0xfa49,0xf3fc,0xfa4a,0xf3fd,0xfa4b,0xf3fe
,0xfa4c,0xf4a1,0xfa4d,0xf4a2,0xfa4e,0xf4a3,0xfa4f,0xf4a4,0xfa50,0xf4a5,0xfa51,0xf4a6
,0xfa52,0xf4a7,0xfa53,0xf4a8,0xfa56,0xf4a9,0xfa57,0xf4aa,0xfa58,0xf4ab,0xfa59,0xf4ac
,0xfa5a,0xf4ad,0xfa62,0xf4ae,0xfa6a,0xf4af,0xfa7c,0xf4b0,0xfa83,0xf4b1,0xfa8a,0xf4b2
,0xfa8b,0xf4b3,0xfa90,0xf4b4,0xfa92,0xf4b5,0xfa96,0xf4b6,0xfa9b,0xf4b7,0xfa9c,0xf4b8
,0xfa9d,0xf4b9,0xfaaa,0xf4ba,0xfaae,0xf4bb,0xfab0,0xf4bc,0xfab1,0xf4bd,0xfaba,0xf4be
,0xfabd,0xf4bf,0xfac1,0xf4c0,0xfacd,0xf4c1,0xfad0,0xf4c2,0xfad5,0xf4c3,0xfad8,0xf4c4
,0xfae0,0xf4c5,0xfae5,0xf4c6,0xfae8,0xf4c7,0xfaea,0xf4c8,0xfaee,0xf4c9,0xfaf2,0xf4ca
,0xfb43,0xf4cb,0xfb44,0xf4cc,0xfb50,0xf4cd,0xfb58,0xf4ce,0xfb5e,0xf4cf,0xfb6e,0xf4d0
,0xfb70,0xf4d1,0xfb72,0xf4d2,0xfb75,0xf4d3,0xfb7c,0xf4d4,0xfb7d,0xf4d5,0xfb7e,0xf4d6
,0xfb80,0xf4d7,0xfb82,0xf4d8,0xfb85,0xf4d9,0xfb86,0xf4da,0xfb89,0xf4db,0xfb8d,0xf4dc
,0xfb8e,0xf4dd,0xfb92,0xf4de,0xfb94,0xf4df,0xfb9d,0xf4e0,0xfb9e,0xf4e1,0xfb9f,0xf4e2
,0xfba0,0xf4e3,0xfba1,0xf4e4,0xfba9,0xf4e5,0xfbac,0xf4e6,0xfbae,0xf4e7,0xfbb0,0xf4e8
,0xfbb1,0xf4e9,0xfbb3,0xf4ea,0xfbb4,0xf4eb,0xfbb6,0xf4ec,0xfbb7,0xf4ed,0xfbb8,0xf4ee
,0xfbd3,0xf4ef,0xfbda,0xf4f0,0xfbe8,0xf4f1,0xfbe9,0xf4f2,0xfbea,0xf4f3,0xfbee,0xf4f4
,0xfbf0,0xf4f5,0xfbf2,0xf4f6,0xfbf6,0xf4f7,0xfbf7,0xf4f8,0xfbf9,0xf4f9,0xfbfa,0xf4fa
,0xfbfc,0xf4fb,0xfc42,0xf4fc,0xfc49,0xf4fd,0xfc4b,0xf4fe
};
#endif //#ifdef ENABLE_IBM_EXPAND_CODES

//////////////////////////////////////////
// main
#define SIZE_OUTPUT 0x1000

int main(int argc,char *argv[])
{
	string szCmdLine;
	int r;
	char szOutput[SIZE_OUTPUT];
	szOutput[0] = '\0';

	for(int i=1; i<argc; i++){
		szCmdLine += argv[i];
		szCmdLine += " ";
	}
	r = _command(0, szCmdLine.c_str(), szOutput, sizeof(szOutput));
	if (szOutput[0])
		puts(szOutput);
	return 0;
}

//////////////////////////////////////////
// _command
// interface:
// ppkft != null: set translate-env. only.
// ppkft == null: do translate.

int _command(const HWND _hwnd, const char * szCmdLine, char * szOutput, const unsigned long wSize, CUnKanj **ppkft)
{
	bool bRunningSave = vbRunning;
	vbRunning = true;
	CUnKanj *pkft = null;
	int iRtn = UNKANJI_OK;
	string strCmdLine;
	string strOption;

	string strSrcFileName;
	string strDesFileName;
	bool bMsg = true;

	fstream fout;
	void *pBuff=null;
	unsigned int rsize;
	string strError;
	char szError[256];

	//takaso2010
	setlocale(LC_ALL, "Japanese_Japan.932");

	//step 1. set global window handle
	if (_hwnd!=null)
		vhOwnerHwnd = _hwnd;

	//step 2. make filter object.
	pkft = new CUnKanj();

	//step 3. parse the command line.
	if (szCmdLine && szCmdLine[0])
		strCmdLine = szCmdLine;
	else
	//Initial setting!
		strCmdLine = "?";
	
	//step 3-1. parse the command.
	//step 3-2. parse the options.
	if (!ParseSwitchs(strCmdLine, pkft, strSrcFileName, strDesFileName) ||
		(ppkft && !strDesFileName.empty())) {
		if (pkft->m_ct==CT_HELP)
			strError = svzHelp;
		else {
			sprintf_s(szError, sizeof(szError),"parameter error."); 
			iRtn = UNKANJI_ERROR_PARAM;
			strError = szError;
		}
		goto Err;
	}


	//step 4. pretreatment - open files.
	//step 4-1.open source file
	if (!pkft->open(strSrcFileName.c_str())) {
		iRtn = UNKANJI_ERROR_SOURCE_FILE;
		sprintf_s(szError,sizeof(szError),"error:\"%s\" can not open.",strSrcFileName.c_str());
			strError = szError;
		goto Err;
		}

	//step 4-2.open destination file
	if (ppkft) {
		*ppkft = pkft;
	}else {
		while( !strDesFileName.empty()) {
			fout.clear();
			//fout.open( strDesFileName.c_str(), ios::out | ios::trunc | ios::binary, filebuf::sh_none);
			fout.open( strDesFileName.c_str(), ios::out | ios::trunc | ios::binary);
			if (!fout.is_open()) {
				int lenT = strDesFileName.length()-GetFileName(strDesFileName).size();
				string strT;
				if (lenT>0)
					strT= strDesFileName.substr(0, lenT);
				else
					strT = GetCurrentPath();
				if (FCreateDirectory(strT.c_str()))
					continue;
				#ifdef _DEBUG
				#ifdef WIN32
				cout << _T("could not be creat directory");
				cout << GetLastError();
				cout << endl;
					#else
				cout << _T("could not be creat directory") << endl;
				#endif
				cout << _T("could not be create file ") << endl;
				#endif
				iRtn = UNKANJI_ERROR_DESTINATION_FILE;
				sprintf_s(szError,sizeof(szError),"error:\"%s\" can not open.",strDesFileName.c_str());
				strError = szError;
				goto Err;
			}
			break;
		}
		
		//step 4-3.alloc read buffer
		if ((pBuff=(BYTE *)malloc(pkft->m_uReadSize))==null) {
			iRtn = UNKANJI_ERROR_MEMORY;
			strError = "error:memory error is ocured.\n";
			goto Err;
		}

		//step 5. translate processing. (circulation = taransread -> trans -> write)
		while(!pkft->IsEOF() && (rsize=pkft->read(pBuff, pkft->m_uReadSize))>0) {
			if (pkft->m_ct==CT_CHECK_SRC_KANJTYPE) {
				iRtn = pkft->GetKanjCode();
				if (!strDesFileName.empty()) {
					assert(fout.rdstate()==ios::goodbit);
					rsize = strlen(svzKanjType[iRtn-((int)kt_binary)]);
					fout.write((const char *)svzKanjType[iRtn-((int)kt_binary)], rsize);
				}else{
					puts(svzKanjType[iRtn-((int)kt_binary)]);
				}
				break;
			}else{
				assert(pkft->m_ct==CT_TRANS);
				if (!strDesFileName.empty()) {
					assert(fout.rdstate()==ios::goodbit);
					fout.write((const char *)pBuff, rsize);
				}else{
					fwrite( pBuff, sizeof( char ), rsize, stdout );
				}
			}
		}
	}
	if (fout.is_open()) {
		fout.flush();
	}

	//step 6. terminate
Err:
	if (fout.is_open()) {
		fout.close();
	}


	if (pBuff)
		free(pBuff);
	if (ppkft==null && pkft) {
		delete pkft;
		pkft = null;
	}
	//error ?
	if (!strError.empty()) {
		if (pkft) {
			delete pkft;
			pkft = null;
		}
		#ifdef _DEBUG
		cout << strError.c_str() << endl;
		#endif
		if (szOutput && wSize>0) {
			unsigned long w = wSize<(unsigned long)strError.length() ? wSize:strError.length();
			strncpy_s(szOutput, SIZE_OUTPUT, strError.c_str(), w);
			szOutput[w==wSize ? wSize-1:w] = '\0';
		}else if (!vhOwnerHwnd && !_hwnd)
			fputs( strError.c_str(), stderr );
	}
	vbRunning = bRunningSave;
	return iRtn;
}
bool ParseSwitchs(string &strCmdLine, CUnKanj *pkft, string &strSrcFileName, string &strDesFileName)
{
	string strOption;
	int len;
	bool bSucsess = false;

	strOption = GetPartStringOfPath(strCmdLine);
	len = strOption.length();
	if (len>0) {
		char ch = toupper(strOption[0]);
		switch(ch) {
		case 'T':
			pkft->m_ct = CT_TRANS;
			bSucsess = true;
			break;
		case 'C':
			pkft->m_ct = CT_CHECK_SRC_KANJTYPE;
			bSucsess = true;
			break;
		case '?':
			pkft->m_ct = CT_HELP;
			break;
		default:
			break;
		}
	}
	
	//step 3-2. parse the options.
	while(bSucsess) {
		strOption = GetPartStringOfPath(strCmdLine);
		len = strOption.length();
		if (len==0)
			break;
		if (strOption[0]=='-' || strOption[0]=='/') {
			strOption.erase(0,1);
			len--;
			if (len==1) {
				char ch = strOption[0];
				switch(ch) {
				case 's':
					pkft->m_ktDes = kt_sjis;
					break;
				case 'j':
					pkft->m_ktDes = kt_jis;
					break;
				case 'e':
					pkft->m_ktDes = kt_euc;
					break;
				case 'u':
					pkft->m_ktDes = kt_utf8;
					break;
				//
				case 'S':
					pkft->SetKanjType(kt_sjis);
					break;
				case 'J':
					pkft->SetKanjType(kt_jis);
					break;
				case 'E':
					pkft->SetKanjType(kt_euc);
					break;
				case 'U':
					pkft->SetKanjType(kt_utf8);
					break;
				default:
					bSucsess = false;
					break;
				}
			}else{
				char ch = toupper(strOption[0]);
				char *stopstring=null;
				long ul = strtoul( strOption.data()+1, &stopstring, 10 );
				if (ul>0 && stopstring>(strOption.data()+1)) {
					if (ch=='R') {
						pkft->m_uReadSize = ul*1024;
#ifdef ENABLE_IBM_EXPAND_CODES
					}else if (ch=='G' && ul==0) {
						pkft->m_bGaiji = false;
					}else if (ch=='G' && ul==1) {
						pkft->m_bGaiji = true;
#endif //#ifdef ENABLE_IBM_EXPAND_CODES
					}else
						bSucsess = false;
				}else{
					_strupr_s((char*)strOption.c_str(), strOption.length());
					if (strOption=="CRLF")
						pkft->m_le = LE_CRLF;
					else if (strOption=="CR")
						pkft->m_le = LE_CR;
					else if (strOption=="LF")
						pkft->m_le = LE_LF;
					else if (strOption.length()==3 && strOption.substr(0,2)=="KI")
						pkft->m_chKI = strOption[2];
					else if (strOption.length()==3 && strOption.substr(0,2)=="KO")
						pkft->m_chKO = strOption[2];
					else
						bSucsess = false;
				}
			}
		}else{
			if (strSrcFileName.empty())
				strSrcFileName = strOption;
			else if (strDesFileName.empty()) {
				strDesFileName = strOption;
			} else
				bSucsess = false;
		}
	}

	//takaso2010 bugfix "-sU" -> "-Us" 順番が逆にしてされるとProcが設定できない。
	if (pkft->GetKanjCode() != kt_ascii)
		pkft->SetKanjType((KanjType)pkft->GetKanjCode());

	return bSucsess;
}

//////////////////////////////////////////
// CUnKanj
CUnKanj::CUnKanj()
{
//	vbRunning = true;
	m_ct = CT_TRANS;
	m_ktSrc = kt_ascii;
	m_convProc = null;
	m_bNoConv = false;

	m_ktDes = kt_ascii;
	m_le = LE_NO;

	m_pReadBuff = null;
	m_pWriteBuff = null;
	m_sizeWrite = 0;
	m_uReadSize = DEF_READ_SIZE;

	m_chKI = 'B';
	m_chKO = 'B';

	m_bGaiji = false;
	CleanUp();
}


CUnKanj::~CUnKanj()
{
	close();
//	vbRunning = false;
}

bool CUnKanj::open(const char *pSrcFileName, unsigned int nOpenFlags)
{
	close();

	m_fileSrc.clear();
	assert(m_fileSrc.rdstate()==ios::goodbit);
	m_fileSrc.open( pSrcFileName, nOpenFlags);
	if (!m_fileSrc.is_open())
		{
		#ifdef _DEBUG
			cout << "file could not be opened "  << endl;
		#endif
		return false;
		}
	//m_uReadSize = 0;//DEF_READ_SIZE;
	return true;
}

void CUnKanj::close()
{
	if (m_fileSrc.is_open())
		m_fileSrc.close();
	if (m_pReadBuff!=null) {
		free(m_pReadBuff);
		m_pReadBuff = null;
	}
	//m_ktSrc = kt_ascii;
	//m_convProc = null;
	CleanUp();
}
void CUnKanj::CleanUp()
{
	if (m_fileSrc.is_open()) {
		m_fileSrc.clear();	//state is set ios::failer bit when eof,
//		m_fileSrc.rdbuf()->seekoff(0, ios::beg, ios::in);	//3rd parameter is not default para is bug in win-version.
		m_fileSrc.rdbuf()->pubseekoff(0, ios::beg, ios::in);	//3rd parameter is not default para is bug in win-version.

		assert(m_fileSrc.rdstate()==ios::goodbit);
	}
	m_lSeek = 0;
	m_bEOF=false;

	m_bFstAnsi= true;
	m_flgShift = fs_no;
	m_uWriteInval = 0;

	pchLim_ReadCh = null;
	pchLimReal_ReadCh = null;
	pchFus_ReadCh = null;
	bEOF_ReadCh = false;

	pwhLim_WriteCh = null;

	m_pch = null;
	m_ch=0;
	m_ch1=0;
	m_ch2=0;
	//m_bNoConv = false;
}

bool CUnKanj::ReadCh(BYTE *&pch, flg_unknown_save fus)
{

#define READOFFSET	sizeof(int)
#define SAVESTARTOFFSET	(3)


	BYTE *pTemp;
	unsigned int rsize, sizeTemp;
try{
	if (fus != fus_no) {
		if (fus==fus_savestart)
			pchFus_ReadCh = pch - SAVESTARTOFFSET;
		else {//fus_readstart:
			if (pchFus_ReadCh==null) {
				assert(m_pReadBuff);
				pch = m_pReadBuff;
			}else
				pch = pchFus_ReadCh;
		}
		return false;
	}
#if 0
#if defined(_DEBUG) && defined(WIN32) && defined(_TRACE)
		TRACE("[%hd] %p < %p \n",(int)(pch-m_pReadBuff), pch, pchLim_ReadCh);
#endif
#endif
	if (pch!=null && pch < pchLim_ReadCh)
		return false;
	else if (!bEOF_ReadCh || pch==null) {
		if (pch==null) {
			bEOF_ReadCh = false;
			pchFus_ReadCh = null;
		}
		rsize = SAVESTARTOFFSET * 2;
		if ((m_ktSrc == kt_ascii && !m_bFstAnsi) || pch==null) {
			if (m_pReadBuff!=null) {
				//2nd read
				rsize = msize(m_pReadBuff);
				if (m_ktSrc == kt_ascii) {
					//may be binary.
					assert(!m_bNoConv);
					m_bNoConv = true;
					return true;
				}
			}
			assert(m_uReadSize>1);
			if ((pTemp=(BYTE *)realloc(m_pReadBuff, m_uReadSize + rsize))==null) {
				#ifdef _DEBUG
				cout << "memory fail!" << endl;
				assert(false);
				#endif
				throw bad_alloc();
			}
			if (pch==null) {
				pch = pTemp;
				m_pReadBuff = pTemp;
			} else {
				pch = pTemp + (pch - m_pReadBuff);
				rsize = pchLimReal_ReadCh - m_pReadBuff;
				m_pReadBuff = pTemp;
				pTemp += rsize;
			}
		} else {
			// copy surplus characters to first positon in buffer.
			pTemp = m_pReadBuff;
			while(pch<=pchLimReal_ReadCh)
				*pTemp++ = *pch++;
			pch = m_pReadBuff;
		}
		// pTemp <---next file data setting pointer!
		sizeTemp = msize(m_pReadBuff) - (pTemp - m_pReadBuff) - SAVESTARTOFFSET; //full read - SAVESTARTOFFSET
		assert(m_fileSrc.rdstate()==ios::goodbit);
		m_fileSrc.read((char *)pTemp, sizeTemp);
		m_sizeRead = m_fileSrc.gcount();
		bEOF_ReadCh = m_fileSrc.eof();	//if warrning this line at _MSC_VER because of eof() is int-type function. this is bug!
		if (bEOF_ReadCh) {
			pchLimReal_ReadCh = pTemp + m_sizeRead - 1;
			memset((void *)(pchLimReal_ReadCh+1), 0, SAVESTARTOFFSET);
			pchLim_ReadCh = pchLimReal_ReadCh + 1 + SAVESTARTOFFSET;
		} else {
			pchLimReal_ReadCh = pTemp + m_sizeRead - 1;
			pchLim_ReadCh = pchLimReal_ReadCh - SAVESTARTOFFSET;
		}
		if (m_lSeek==0) {
			m_lim1 = m_sizeRead / 10;
			m_lim2 = m_lim1 / 2;
			m_lim2 = m_lim2 < 0x100 ? 0x100:m_lim2;
		}
#if 0
#if defined(_DEBUG) && defined(WIN32) && defined(_TRACE)
		TRACE("[%hd] %p < %p \n",(int)(pch-m_pReadBuff), pch, pchLim_ReadCh);
#endif
#endif
		return false;
	}
	if (m_ktSrc == kt_ascii) {
		//small source file.
		//may be binary.
		assert(!m_bNoConv);
		m_bNoConv = true;
		return true;
	}
	m_bEOF = true;
	return true;

	}catch (exception &e) {
		TraceException(e);
		assert(false);
		throw;
	}
}


bool CUnKanj::WriteCh(BYTE *&pwh)
{
#define WRITEOFFSET	sizeof(int)

try{
	//assert(pwh!=null);
	if (pwh == null){
		pwh = m_pWriteBuff;
		pwhLim_WriteCh = pwh + m_sizeWrite -1 - WRITEOFFSET;
		//if ((m_pWriteBuff=(BYTE *)realloc(m_pWriteBuff, m_uReadSize))==null) {
		//	AfxThrowMemoryException();
		//}
		//	pwh = m_pWriteBuff;
		//	pwhLim_WriteCh = pwh +  msize(m_pWriteBuff) - 1 - WRITEOFFSET;
		return false;
	}else if (pwh < pwhLim_WriteCh) {
		return false;
	}else {
		//first read
		if (m_ktSrc == kt_ascii) {
			assert(!m_bNoConv);
			m_bNoConv = true;
			return true;
		}
	}
	return true;

	}catch (exception &e) {
		TraceException(e);
		assert(false);
		throw;
	}
}
inline bool CUnKanj::InvalWrite(BYTE *&pwh, int *&pRtn, bool &bNoConv)
{
#define INVALLIMIT	(10)
	*pwh++ = '*';
	m_uWriteInval++;
	//check first read & upper than INVALLIMIT & upper 20%
	if (m_lSeek==0  && (m_uWriteInval>INVALLIMIT && m_uWriteInval>(m_sizeRead/5))) {
		if (pRtn) {
			*pRtn = UNKANJI_INVAL_TEXT;
		} else {
			m_ktSrc = kt_binary;	//source code type = binary
			bNoConv = true;
		}
		return true;
	}
	return  false;
}
inline int CUnKanj::ConvLineEnd(BYTE *&pwh)
{
	if (m_le != LE_NO && m_chType & ANS) {
		int eate = 0;
		if (m_ch=='\r' && m_ch1=='\n') {
			eate = 2;
			m_ch1 = 0;	//use 2bytes
		}else if (m_ch=='\r')
			eate = 1;
		else if (m_ch=='\n')
			eate = 1;
		if (eate>0) {
			if (m_le == LE_CR) {
				*pwh++ = '\r';
			}else if (m_le == LE_LF) {
				*pwh++ = '\n';
			}else{
				*pwh++ = '\r';
				*pwh++ = '\n';
			}
			return eate;
		}
	}
	return 0;
}
unsigned int CUnKanj::ReadOnly_ConvLineEnd(BYTE* pwh, unsigned int nCount)
{
	try {
	assert(nCount>0);
	BYTE *pch_first = new BYTE[nCount+sizeof(int)];
	BYTE *pch = pch_first;

	m_fileSrc.clear();	//state is set ios::failer bit when eof,
	m_fileSrc.rdbuf()->pubseekoff(m_lSeek, ios::beg, ios::in);	//3rd parameter is not default para is bug in win-version.
	m_fileSrc.read((char *)pch_first, nCount+sizeof(int));

	unsigned int fsize = m_fileSrc.gcount();
	BYTE *pch_last = pch + (fsize<nCount ? fsize:nCount),
		*pwh_last = pwh + nCount -1,
		*pwh_first = pwh;
	*pch_last++ = 0;
	while(pch<pch_last) {
		if (m_ch1) {
			m_ch = m_ch1; m_chType = m_chType1;
		}else {
			m_ch = *pch++; m_chType = GetType(m_ch);
		}
		m_ch1 = *pch++; m_chType1 = GetType(m_ch1);
		if (ConvLineEnd(pwh)==0) {
			*pwh++ = m_ch;
		}
		if (pwh>=pwh_last) {
			fsize =  pch - pch_first - (m_ch1==0 ? 0:1);
			m_ch1 = 0;
			m_lSeek += fsize;
			delete [] pch_first;
			return pwh - pwh_first;
		}
	}

	m_bEOF = true;
	delete [] pch_first;
	return pwh - pwh_first;
	}catch (exception &e) {
		TraceException(e);
		assert(false);
		throw;
	}
}
inline unsigned int CUnKanj::ReadOnly( void* lpBuf, unsigned int nCount, bool bBegin)
{
	unsigned int fsize;
	try {
		if (bBegin) {
			assert(m_fileSrc.rdstate()==ios::goodbit || m_fileSrc.rdstate()&ios::eofbit);
			m_fileSrc.clear();	//state is set ios::failer bit when eof,
			m_fileSrc.rdbuf()->pubseekoff(0, ios::beg, ios::in);	//3rd parameter is not default para is bug in win-version.
			m_bEOF = false;
			m_lSeek = 0;
			m_ch = m_ch1 = m_ch2 = 0;
		}
		if (m_le == LE_NO) {
			assert(m_fileSrc.rdstate()==ios::goodbit);
			m_fileSrc.read((char *)lpBuf, nCount);
			fsize = m_fileSrc.gcount();
			m_bEOF = m_fileSrc.eof();//if warrning this line at _MSC_VER because of eof() is int-type function. this is bug!
			m_lSeek += fsize;
			return fsize;
		}else
			return ReadOnly_ConvLineEnd((BYTE *)lpBuf, nCount);
	}catch (exception &e) {
		TraceException(e);
		assert(false);
		throw;
	}
}
int CUnKanj::CheckKanjCode(int *pratios)
{
	int iRtn = kt_ascii;
	m_ct = CT_CHECK_SRC_KANJTYPE;
	BYTE *pBuff;
	if ((pBuff=(BYTE *)malloc(m_uReadSize))==null) {
		m_ct = CT_TRANS;
		return UNKANJI_ERROR_MEMORY;
	}
	if (read(pBuff, m_uReadSize, pratios)>0) {
		iRtn = GetKanjCode();
		if (pratios) {
			//count -> ratio
			int c_max = max(*pratios, max(*(pratios+1), max(*(pratios+2), *(pratios+3)) ));
			if (c_max>0) {
				*pratios = (int)(((unsigned int)(*pratios)) * 100 / c_max);
				pratios++;
				*pratios = (int)(((unsigned int)(*pratios)) * 100 / c_max);
				pratios++;
				*pratios = (int)(((unsigned int)(*pratios)) * 100 / c_max);
				pratios++;
				*pratios = (int)(((unsigned int)(*pratios)) * 100 / c_max);
			}
		}
	}
	m_ct = CT_TRANS;
	return iRtn;
}
unsigned int CUnKanj::read( void* lpBuf, unsigned int nCount, int *pRtn)
{
#if defined(_DEBUG) && defined(WIN32) && defined(_TRACE)
	int y=0, x=0;
#endif
	unsigned int ui;
	int bEuc, bSJis, bUtf8;
	BYTE *pwh;
	unsigned c_sjis, c_euc, c_jis, c_utf8;
	bool bfirst = false;

	try {
	if (pRtn)
		*pRtn = UNKANJI_OK;

	if (m_lSeek==0) {
#if defined(_DEBUG) && defined(WIN32) && defined(_TRACE)
		y=0, x=0;
#endif
		m_ch = m_ch1 = m_ch2 = 0;
		c_sjis = c_euc = c_jis = c_utf8 = 0;
		m_pch = null;
		//m_bNoConv = false;
		CleanUp();
		//check a read buffer.
		//if (m_uReadSize<=0)
		m_uReadSize = nCount;
		bfirst = true;
	}

	if (m_bNoConv) {
		//no convert case.
		return ReadOnly(lpBuf, nCount, false);
	}
	
	m_pWriteBuff = (BYTE *)lpBuf;
	m_sizeWrite = nCount;
	pwh = null;

	while(true){
#ifdef WIN32
		if (vbBackGroundMode) {
			MSG msg;
			if (::PeekMessage(&msg, null, null, null, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
#endif
		// write block set more than size(int) bytes.
		if (WriteCh(pwh)) {
			if (m_ct == CT_CHECK_SRC_KANJTYPE) {
				m_lSeek=0;
				if (pRtn) {
					//save counter
					*pRtn++ = c_jis;
					*pRtn++ = c_euc;
					*pRtn++ = c_sjis;
					*pRtn++ = c_utf8;
				}
				return 1;
			}
			if (m_bNoConv) {
				//to binary
				return ReadOnly(lpBuf, nCount);
			}
			unsigned int count = (unsigned int)(pwh - m_pWriteBuff);
			pwh = null;
			m_lSeek += count;
			return count;
		}
		//read block set more than size(int) bytes.
		if (ReadCh(m_pch)) {
			if (m_ct == CT_CHECK_SRC_KANJTYPE) {
				m_lSeek=0;
				if (pRtn) {
					//save counter
					*pRtn++ = c_jis;
					*pRtn++ = c_euc;
					*pRtn++ = c_sjis;
					*pRtn++ = c_utf8;
				}else{
					if (m_ktSrc == kt_ascii) {
						if ((c_sjis+c_jis+c_euc+c_utf8)==0)
							m_ktSrc = kt_ascii;
						else if (c_sjis>=c_euc && c_sjis>=c_jis && c_sjis>=c_utf8)
							m_ktSrc = kt_sjis;
						else if (c_euc>=c_sjis && c_euc>=c_jis && c_euc>=c_utf8)
							m_ktSrc = kt_euc;
						else if (c_jis>=c_sjis && c_jis>=c_euc && c_jis>=c_utf8)
							m_ktSrc = kt_jis;
						else if (c_utf8>=c_sjis && c_utf8>=c_euc && c_utf8>=c_jis)
							m_ktSrc = kt_utf8;
					}
				}
				return 1;
			}
			if (m_bNoConv) {
				//to binary
				if (m_ktSrc == kt_ascii) {
					if ((c_sjis+c_jis+c_euc+c_utf8)==0)
						m_ktSrc = kt_ascii;
					else if (c_sjis>=c_euc && c_sjis>=c_jis && c_sjis>=c_utf8)
						m_ktSrc = kt_sjis;
					else if (c_euc>=c_sjis && c_euc>=c_jis && c_euc>=c_utf8)
						m_ktSrc = kt_euc;
					else if (c_jis>=c_sjis && c_jis>=c_euc && c_jis>=c_utf8)
						m_ktSrc = kt_jis;
					else if (c_utf8>=c_sjis && c_utf8>=c_euc && c_utf8>=c_jis)
						m_ktSrc = kt_utf8;
					if (m_ktSrc != kt_ascii) {
						if (!SetKanjType_ReadCh(m_ktSrc, m_pch)) {
							if (pRtn)
								*pRtn = UNKANJI_SAME_CONVERT;
							else {
								return ReadOnly(lpBuf, nCount);
							}
							return null;
						}
						m_ch1 = m_ch2 = 0;	//read first charcter again.
						m_bNoConv = false;
						continue;
					}
				}
				return ReadOnly(lpBuf, nCount);
			}
			unsigned int count = (unsigned int)(pwh - m_pWriteBuff);
			m_lSeek += count;
			if (pRtn)
				*pRtn = UNKANJI_EOF;
			return count;
		}
		// get 3 characters always.
		// m_ch is always use.
		if (m_ch1) {
			m_ch=m_ch1; m_chType = m_chType1;
			if (m_ch2) {
				m_ch1=m_ch2; m_chType1 = m_chType2;
//takaso 2011.10.2 add
				m_ch2 = *m_pch++; m_chType2 = GetType(m_ch2);
			} else {
				m_ch1 = *m_pch++; m_chType1 = GetType(m_ch1);
//takaso 2011.10.2 add
				m_ch2 = 0; m_chType2 = ANS;
			}
		}else if (m_ch2) {
			m_ch=m_ch2; m_chType = m_chType2;
			m_ch1 = *m_pch++; m_chType1 = GetType(m_ch1);
//takaso 2011.10.2 add
			m_ch2 = *m_pch++; m_chType2 = GetType(m_ch2);
		}else {
			m_ch = *m_pch++; m_chType = GetType(m_ch);
			m_ch1 = *m_pch++; m_chType1 = GetType(m_ch1);
//takaso 2011.10.2 add
			m_ch2 = *m_pch++; m_chType2 = GetType(m_ch2);
		}
//takaso 2011.10.2 remove
		//		m_ch2 = *m_pch++; m_chType2 = GetType(m_ch2);

		if (bEOF_ReadCh && m_pch >= pchLim_ReadCh)
			continue; //

//#if defined(_DEBUG) && defined(WIN32) && defined(_TRACE)
#if 0
		TRACE("[%hd](%d,%d)=%hd(%c),%hd(%c),%hd(%c)\n",(int)(m_pch-m_pReadBuff), y, x, m_ch,m_ch,m_ch1,m_ch1,m_ch2,m_ch2);
		if (m_ch=='\r' && m_ch1=='\n')
			y++;
		else if (m_ch=='\r' || m_ch=='\n')
			x=0;
		else
			x++;
#endif
		//check start!
		if (!IsValidCode(m_chType)) { // invalid character case
			if (InvalWrite(pwh, pRtn, m_bNoConv)) {
				if (m_bNoConv)
					return ReadOnly(lpBuf, nCount);
				return 0;
			}
			continue;
		}else{
		switch (m_ktSrc) {
		case kt_ascii:	// don't know source character code type.
			if (m_bFstAnsi) {
				if (m_chType & ANS) {
					if (ConvLineEnd(pwh)>0)
						break;
					*pwh++ = m_ch;
					break;
				} else {
					if (bfirst) {
						if(m_ch==0xEF  && m_ch1==0xBB  && m_ch2==0xBF) //utf-8 バイト順マーク（略語：BOM）
							m_ktSrc = kt_utf8;
					}
					m_bFstAnsi = false;
					ReadCh(m_pch, fus_savestart);
				}
			}
			if (m_chType & CJ) {
				if (m_ch==ESC) {
					//check KI
					if( (m_ch1=='$' && (m_ch2=='B' || m_ch2=='@')) || m_ch1=='K')
						m_ktSrc = kt_jis;
					//check KO
					else if( ( m_ch1=='(' && (m_ch2=='J' || m_ch2=='B')) || m_ch1=='H' )
						m_ktSrc = kt_jis;
				}else if (m_ch == SI) {
					if (m_chType1 & HANKATA) {
						//if (++c_jis > m_lim2) //upper 5%
						//	m_ktSrc = kt_jis;
						c_jis += 2;
					}
				} //else if (m_ch == SO)
				if (m_ktSrc != kt_jis) {
					if (c_jis > m_lim2 || ((c_jis > (c_sjis+c_euc+c_utf8+1)*100 )))
						m_ktSrc = kt_jis;
					m_ch1 = 0;
				}
				//	m_flgShift = fs_no;
			} else	if (m_ch>=0x80) {
				//try euc or shift-jis.
				bSJis = (m_chType & HANKATA) ? 1:0;
				bSJis += ((m_chType & SJ1) && (m_chType1 & SJ2)) ? 2:0;
				bEuc = ((m_chType & EUC) && (m_chType1 & EUC)) ? 2:0;
				bEuc += ((m_ch==SS2) && (m_chType1 & HANKATA)) ? 1:0;
				bEuc += ((m_ch==SS3) && (m_chType1 & EUC) && (m_chType2 & EUC)) ? 3:0;
				bEuc += ((m_chType == EUC) && (m_chType1 & EUC) && (m_chType2 & ANS_EUC)) ? 4:0;
				bUtf8 = ((m_chType & UEF2) && (m_chType1 & UEC)) ? 2:0;
				bUtf8 += ((m_chType & UEF3) && (m_chType1 & UEC) && (m_chType2 & UEC)) ? 3:0;
				if (/*m_ktDes != kt_sjis &&*/ bSJis /*&& !bEuc && !bUtf8 */) {
					//upper 5%
					c_sjis += bSJis;
					if (c_sjis > m_lim2 || ((c_sjis > (c_jis+c_euc+c_utf8+1)*100 )))
						m_ktSrc = kt_sjis;
					if (bSJis>=2)
						m_ch1 = 0;
				}
				if (/*m_ktDes != kt_utf8 &&*/ bUtf8 /*&& !bSJis && !bEuc*/) {
					//upper 5%
					c_utf8 += bUtf8;
					if (c_utf8 > m_lim2 || ((c_utf8 > (c_jis+c_euc+c_sjis+1)*100 )))
						m_ktSrc = kt_utf8;
					m_ch1 = 0;
					if (bUtf8>=3)
						m_ch2 = 0;
				}
				if (/*m_ktDes != kt_euc &&*/ /*!bSJis && !bUtf8 && */
					(bEuc && (m_chType2 & ANS_EUC))) {
//						if (!(m_chType & HANKATA))
//							m_ktSrc = kt_euc;
//						else {
					c_euc += bEuc;
					if (c_euc > m_lim2 || ((c_euc > (c_jis+c_sjis+c_utf8+1)*100 )))
						m_ktSrc = kt_euc;
//						}
					m_ch1 = 0;
					if (bEuc>=3)
						m_ch2 = 0;
				}
			}
			if (m_ktSrc != kt_ascii) {
				if (m_ct == CT_CHECK_SRC_KANJTYPE) {
					m_lSeek=0;
					if (pRtn) {
						//save counter
						*pRtn++ = c_jis;
						*pRtn++ = c_euc;
						*pRtn++ = c_sjis;
						*pRtn++ = c_utf8;
					}
					return 1;
				}else if (!SetKanjType_ReadCh(m_ktSrc, m_pch)) {
					if (pRtn)
						*pRtn = UNKANJI_SAME_CONVERT;
					else {
						return ReadOnly(lpBuf, nCount);
					}
					return null;
				}
				m_ch1 = m_ch2 = 0;	//read first charcter again.
			}
			bfirst = false;
			break;
		case kt_jis:
			if (ConvLineEnd(pwh)>0)
				break;
			if (m_chType & CJ) {
				if (m_ch==ESC) {
					//check KI
					if( m_ch1=='$' && (m_ch2=='B' || m_ch2=='@') ){
						m_flgShift = fs_kanji;
						m_ch1 = m_ch2 = 0;	//use 3bytes
					} else if( m_ch1=='K') {
						m_flgShift = fs_kanji;
						m_ch1 = 0;	//use 2bytes
					//check KO
					} else if( m_ch1=='(' && (m_ch2=='J' || m_ch2=='B') ){
						m_flgShift = fs_no;
						m_ch1 = m_ch2 = 0;	//use 3bytes
					} else if( m_ch1=='H') {
						m_flgShift = fs_no;
						m_ch1 = 0;	//use 2bytes
					} else {
						if (InvalWrite(pwh, pRtn, m_bNoConv)) {
							if (m_bNoConv)
								return ReadOnly(lpBuf, nCount);
							return 0;
						}
					}
				}else if (m_ch == SI) {
					m_flgShift = fs_hankana;
				}else if (m_ch == SO) {
					m_flgShift = fs_no;
				}
			} else
			switch (m_flgShift) {
			case fs_kanji:
				ui = INV_MAKEWORD(m_ch, m_ch1);
				ui = (*m_convProc)(ui);
				if( ui ){
					*pwh++ = (ui>>8) & 0xff;
					*pwh++ = ui & 0xff;
					m_ch1 = 0;	//use 2bytes
				} else {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				}
				break;
			case fs_hankana:
				if (GetType(m_ch | 0x80) & HANKATA) {
					if (m_ktDes==kt_euc) {
						*pwh++ = SS2;
					}
					*pwh++ = (m_ch |= 0x80);
				}else {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				}
				break;
			default:
				//ansi 
				if (m_ch >= 0x80 && !(m_chType & HANKATA)) {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				} else {
					*pwh++ = m_ch;
				}
				break;
			}
			break;
		case kt_euc:
			if (m_chType & ANS) {
				if (m_ktDes==kt_jis)
					SetJisFlgShift(pwh);
				if (ConvLineEnd(pwh)>0)
					break;
				*pwh++ = m_ch;
			} else if (m_ch==SS2) {
				if (m_chType1 & HANKATA) {
					if (m_ktDes==kt_sjis) {
						*pwh++ = m_ch1;
					} else {//jis
						SetJisFlgShift(pwh, fs_hankana);
						*pwh++ = (m_ch1 & 0x7F);
					}
					m_ch1 = 0;
				} else {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				}
			}else if (m_ch==SS3) {
				//try gaiji.
				//euc:a2c3->f4fe IBM expanded characters.
				//const BYTE _IBM_euc2sjis[][2] = 
#ifdef ENABLE_IBM_EXPAND_CODES
				if (m_bGaiji) {
					ui = INV_MAKEWORD(m_ch1, m_ch2);
					if (ui>=0xa2c3 && ui<=0xf4fe) {
						bool bFind=false;
						//this is slow way.
						for(int i=0, imax=sizeof(_IBM_euc2sjis)/sizeof(short)/2; i<imax; i++) {
							if (_IBM_euc2sjis[i][1]>=ui) {
								if (_IBM_euc2sjis[i][1]==ui) {
									bFind = true;
									ui = _IBM_euc2sjis[i][0];
								}
								break;
							}
						}
						if (bFind) {
							if (m_ktDes == kt_sjis) {
								*pwh++ = (ui>>8) & 0xff;
								*pwh++ = ui & 0xff;
							}else { //jis
								SetJisFlgShift(pwh);
								*pwh++ = '*';
								*pwh++ = '*';
							}
							m_ch2 = 0;
						} else {
							if (InvalWrite(pwh, pRtn, m_bNoConv)) {
								if (m_bNoConv)
									return ReadOnly(lpBuf, nCount);
								return 0;
							}
						}
					}else {
						if (InvalWrite(pwh, pRtn, m_bNoConv)) {
							if (m_bNoConv)
								return ReadOnly(lpBuf, nCount);
							return 0;
						}
					}
				}else
#endif //#ifdef ENABLE_IBM_EXPAND_CODES
				{
					if (m_ktDes==kt_jis)
						SetJisFlgShift(pwh);
					*pwh++ = '*';
					*pwh++ = '*';
					m_ch2 = 0;
				}
			} else if (m_chType & EUC) {
				ui = INV_MAKEWORD(m_ch, m_ch1);
				ui = (*m_convProc)(ui);
				if( ui ){
					if (m_ktDes==kt_jis)
						SetJisFlgShift(pwh, fs_kanji);
					*pwh++ = (ui>>8) & 0xff;
					*pwh++ = ui & 0xff;
					m_ch1 =0;
				}else {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				}
			}
			break;
		case kt_sjis:
			if (m_chType & ANS) {
				if (m_ktDes==kt_jis)
					SetJisFlgShift(pwh);
				if (ConvLineEnd(pwh)>0)
					break;
				*pwh++ = m_ch;
			}else if (m_chType & SJ1) {
				if (m_ktDes==kt_utf8){
					char mb[2];  wchar_t wc;
					mb[0]=m_ch;
					mb[1]=m_ch1;
					if (mbtowc(&wc, mb, 2) == 2) {
						if (wc <= 0x7ff) {
							*pwh++ = wc >> 6   | 0xc0;
							*pwh++ = wc & 0x3f | 0x80;
						}
						else {
							*pwh++ = wc >> 12       | 0xe0;
							*pwh++ = wc >> 6 & 0x3f | 0x80;
							*pwh++ = wc      & 0x3f | 0x80;
						}					
					}
				}else{
					ui = INV_MAKEWORD(m_ch, m_ch1);
					ui = (*m_convProc)(ui);
#ifdef ENABLE_IBM_EXPAND_CODES
					if (m_bGaiji &&  ui == 0  && m_ktDes == kt_euc) {
						//try gaiji.
						ui = INV_MAKEWORD(m_ch, m_ch1);
						if (ui>=0xfa40 && ui<=0xfc4b) {
							//SJIS:fa40->fc4b IBM expand characters
							ui = _IBM_sjis2euc[ui-0xfa40];
							if (ui && (ui!=0xa2cc && ui!=0xa2e8)) {
								*pwh++ = SS3;
							}
						}
					}
#endif //#ifdef ENABLE_IBM_EXPAND_CODES
					if( ui ){
						if (m_ktDes==kt_jis)
							SetJisFlgShift(pwh, fs_kanji);
						*pwh++ =  (ui>>8) & 0xff;
						*pwh++ =  ui & 0xff;
					} else {
						if (m_ktDes==kt_jis)
							SetJisFlgShift(pwh);
						*pwh++ = '*';
						*pwh++ = '*';
					}
				}
				m_ch1 = 0;
			} else if (m_chType & HANKATA) {
				if (m_ktDes==kt_utf8){
					*pwh++ = m_ch >> 6   | 0xc0;
					*pwh++ = m_ch & 0x3f | 0x80;
				}else{
					if (m_ktDes==kt_euc) {
						*pwh++ = SS2;
					}else {//jis
						SetJisFlgShift(pwh, fs_hankana);
						m_ch &= 0x7F;
					}
					*pwh++ = m_ch;
				}
			}else{
				if (InvalWrite(pwh, pRtn, m_bNoConv)) {
					if (m_bNoConv)
						return ReadOnly(lpBuf, nCount);
					return 0;
				}
			}
			break;


		case kt_utf8:
			if (m_chType & ANS) {
				if (m_ktDes==kt_jis)
					SetJisFlgShift(pwh);
				if (ConvLineEnd(pwh)>0)
					break;
				*pwh++ = m_ch;
			}else if (m_chType & UEF2) {
				if (m_chType1 & UEC){
					assert(m_ktDes == kt_sjis);
					//if (m_ktDes == kt_sjis) {
						ui = (m_ch & 0x1f) << 6;
						ui |= m_ch2 & 0x3f;
					//}
					m_ch1 = 0;	//use 2bytes
					assert(m_ktDes == kt_sjis);
					ui = (*m_convProc)(ui);
					*pwh++ =  ui & 0xff;
					if (ui & 0xff00)
						*pwh++ =  (ui>>8) & 0xff;
				} else {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				}
			}else if (m_chType & UEF3) {
				if ((m_chType1 & UEC) && (m_chType2 & UEC)){
					assert(m_ktDes == kt_sjis);
					//if (m_ktDes == kt_sjis) {
						if(m_ch==0xEF  && m_ch1==0xBB  && m_ch2==0xBF)  {//utf-8 バイト順マーク（略語：BOM）
							m_ch1 = m_ch2 = 0;	//use 3bytes
							break; // ignore
						}
						ui = (m_ch & 0x0f) << 12;
						ui |= (m_ch1 & 0x3f) << 6;
						ui |= m_ch2 & 0x3f;
					//}
					m_ch1 = m_ch2 = 0;	//use 3bytes
					assert(m_ktDes == kt_sjis);
					ui = (*m_convProc)(ui);
					*pwh++ =  ui & 0xff;
					if (ui & 0xff00)
						*pwh++ =  (ui>>8) & 0xff;
				} else {
					if (InvalWrite(pwh, pRtn, m_bNoConv)) {
						if (m_bNoConv)
							return ReadOnly(lpBuf, nCount);
						return 0;
					}
				}
			}
			break;
		default:
			break;
		}
		}
	}

	}catch (exception &e) {
		TraceException(e);
		assert(false);
		throw;
	}
}
void CUnKanj::SetJisFlgShift(BYTE *&pwh, flg_shift fs)
{
	assert(m_ktDes==kt_jis);

	if (m_flgShift != fs) {
		if (m_flgShift==fs_hankana)
			*pwh++ =  SO;
		else if (m_flgShift==fs_kanji) {
			*pwh++ = ESC;
			if (m_chKO!='H')
				*pwh++ = '(';
			*pwh++ = m_chKO;
		}
		if (fs==fs_hankana)
			*pwh++ =  SI;
		else if (fs==fs_kanji) {
			*pwh++ = ESC;
			if (m_chKI!='K')
				*pwh++ = '$';
			*pwh++ = m_chKI;
		}
		m_flgShift = fs;
	}
}

bool CUnKanj::SetKanjType(KanjType ktSrc)
{
	assert(ktSrc != kt_ascii);

	m_ktSrc = ktSrc;
	m_convProc = null;
	switch(m_ktSrc) {
	case kt_jis:
		switch(m_ktDes) {
		case kt_euc:
			m_convProc = jis2euc;
			break;
		case kt_sjis:
			m_convProc = jis2sj;
			break;
		//case kt_jis:
		default:
			m_convProc = jis2jis; //this is dumy function.
			break;
		}
		break;
	case kt_euc:
		switch(m_ktDes) {
		case kt_jis:
			m_convProc = euc2jis;
			break;
		case kt_sjis:
			m_convProc = euc2sj;
			break;
		//case kt_euc:
		default:
			break;
		}
		break;
	case kt_sjis:
		switch(m_ktDes) {
		case kt_jis:
			m_convProc = sj2jis;
			break;
		case kt_euc:
			m_convProc = sj2euc;
			break;
		case kt_utf8:
			m_convProc = sj2utf8; //dumy
			break;
		//case kt_sjis:
		default:
			break;
		}
		break;
	case kt_utf8:
		switch(m_ktDes) {
		case kt_sjis:
			m_convProc = utf82sj; //dumy
			break;
		default:
			break;
		}
		break;
	}
	if (m_convProc == null) {
		m_bNoConv = true;
	}else{
		m_bNoConv = false; //takaso2010 bugfix!
	}
	return m_convProc!=null;
}

//////////////////////////////////////////////////////////////////////////////////
// global functions for character type convertion.
//jis -> sift-jis character.
unsigned int jis2sj(unsigned int c)
{
    unsigned int h, l;
    if ( (c>0x7E7E) || (c<0x2121) || ((c&0xFF)>0x7E) || ((c&0xFF)<0x21) )
        return 0;
	c += 0xa17e;
    h = (c >> 8) & 0xff;
    l = c & 0xff;
	if (!(h & 1)) { 
		if (l<0xde)
			l -= 0x5f;
		else
			l -= 0x5e;
	}	
	h >>= 1;
	h ^= 0xe0;
	return (h << 8) + l;
}

//sift-jis -> jis character.
unsigned int sj2jis(unsigned int c)
{
    unsigned int    h, l;

    h = (c >> 8) & 0xff;
    l = c & 0xff;
    h -= (h >= 0xa0) ? 0xc1 : 0x81;
    if(l >= 0x9f) {
        c = (h << 9) + 0x2200;
        c |= l - 0x7e;
    } else {
        c = (h << 9) + 0x2100;
        c |= l - ((l <= 0x7e) ? 0x1f : 0x20);
    }
    if ( (c>0x7E7E) || (c<0x2121) || ((c&0xFF)>0x7E) || ((c&0xFF)<0x21) )
        return 0;
    return c;

#if 0	//this code deppend to the intel-i386 upper only.
	unsigned short s = c;
	_asm{
    mov ax, s
	shl	ah,1
	sub	al,1fh
	js	sj0
	cmp	al,61h
	adc	al,0deh
sj0:
	add	ax,1fa1h
	and	ax,7f7fh
    mov s,ax
	};

	c = s;
    if ( (c>0x7E7E) || (c<0x2121) || ((c&0xFF)>0x7E) || ((c&0xFF)<0x21) )
        return 0;
    return c;
#endif
}
//euc -> jis character.
unsigned int euc2jis(unsigned int cd)
{
	return cd & 0x7F7F;
}
//jis -> euc character.
unsigned int jis2euc(unsigned int cd)
{
	return cd | 0x8080;
}
//euc -> shift-jis character.
unsigned int euc2sj(unsigned int cd)
{
	return jis2sj(cd & 0x7F7F);
}
//sift-jis -> euc character.
unsigned int sj2euc(unsigned int cd)
{
	return sj2jis(cd) | 0x8080;
}
//jis -> jis character.
//this is dumy.
unsigned int jis2jis(unsigned int c)
{
	return c;
}
//sift-jis -> utf-8 character.
//this is dumy.
unsigned int sj2utf8(unsigned int c)
{
	return c;
}
//utf-8 -> sift-ji  character.
unsigned int utf82sj(unsigned int c)
{
	int retvalue;
	unsigned int out=0;
	errno_t rt = wctomb_s(&retvalue, (char *)&out, 2, c);
	if (rt != 0 /*EINVAL*/) {
		out = 0x2a2a; //"**"set
	}else{
		assert(rt==0);
		assert(retvalue==1 || retvalue==2);
	}
	return out;
}

////////////////////////////////
// global utility functions
string GetFileName(const string &strSrc)
{
	string str;
	string::size_type c=0;
	string::size_type  cdir = strSrc.find_last_of(SZ_DIR);
	string::size_type  cdrv = strSrc.find_last_of(SZ_DRV);
	
	if (cdir==string::npos && cdrv==string::npos) {
		str = strSrc;
		return str;
	}
	if (cdir!=string::npos)
		c = cdir;
	if (cdrv!=string::npos && cdrv>c)
		c = cdrv;

	if (++c<strSrc.size())
		str = strSrc.substr(c);
	
	return str;
}

string GetPartStringOfPath(string &strSrc)
{
	bool bDq=false;
	string str,strT;

	try
	{
	while(true)	{
		unsigned long i;
		//trim befor space.
		for(i=0; i<strSrc.length() && *(strSrc.c_str()+i) == ' '; i++);
		if (i<strSrc.length() && *(strSrc.c_str()+i) == '\"') {
			bDq = true;
			i++;
		}
		strT = strSrc.substr(i);
		strSrc = strT;

		int count = strSrc.find(bDq ? '\"':' ');
		if (count==-1) {
			str = strSrc;
			strSrc="";
			break;
		}
		else if (count==0) {
			strT = strSrc.substr(1);
			strSrc = strT;
			continue;
		} else {
			str = strSrc.substr(0, count);
			strT = strSrc.substr(count+1);
			strSrc = strT;
			break;
		}
	}

	}catch (exception &e) {
		TraceException(e);
		assert(false);
	}
	return str;
}

//This codes is slow way , but this code is not so many called at this function.
string GetPartString(string &strSrc, char ch)
{
	string str,strT;
	try	{
		while(true) {
			int count = strSrc.find(ch);
			if (count==-1) {
				str = strSrc;
				strSrc="";
				break;
			} else if (count==0)  {
				strT = strSrc.substr(1);
				strSrc = strT;
				continue;
			}else {
				str = strSrc.substr(0, count);
				strT = strSrc.substr(count+1);
				strSrc = strT;
				break;
			}
		}
	}catch (exception &e) {
		TraceException(e);
		assert(false);
	}
	return str;
}


bool FCreateDirectory(const char * lpszPath)
{
	string strPath;
	bool bCreate=false;
	BYTE ch, chBef;
	while(true) {
		ch=*lpszPath++;
		if ((ch==CH_DIR && chBef!=CH_DRV) || (ch=='\0' && chBef!=CH_DIR)) {
			if (!strPath.empty() && strPath!=SZ_NET_DIR) {
				#ifdef WIN32
					bCreate = (CreateDirectory(strPath.c_str(), null/*LPSECURITY_ATTRIBUTES lpSecurityAttributes*/)==TRUE);
				#else
					bCreate = (mkdir(strPath.c_str())==0);
				#endif
			}
		}
		if (ch=='\0')
			break;
		strPath += ch;
		if ((GetType(ch) & SJ1) && *lpszPath) //kanji file name is may be shift-jis.
			strPath += *lpszPath++;
		else
			chBef = ch;
	}
	return bCreate;
}

string GetCurrentPath(void)
{
	string str;
#ifdef _MSC_VER
	char *pszBuf = (char*)getcwd(null, 0);
	str = pszBuf;
	free(pszBuf);
	return str;
#else
	unsigned long dwRet1=__MAX_PATH,dwRet2;
	char *pszBuf=new char[__MAX_PATH];

	while(true) {
		dwRet2 = getcwd((char *)pszBuf, dwRet1);
		if (dwRet2 >= dwRet1)
			{
			delete [] pszBuf;
			dwRet1=dwRet2*2;
			pszBuf=new char[dwRet1];
			}
		else
			break;
	}
	str = pszBuf;
	delete [] pszBuf;
	if (dwRet2!=0 && !str.empty())
		return str;
	str="";
	return str;
#endif
}

