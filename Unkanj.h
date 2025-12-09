#if !defined(UNKANJ_H)
#define UNKANJ_H
/*=======================================================================
                UNKANJ.h Ver 0.05  ヘッダーファイル
				unkanji.exe , unkanji.dll API 説明ファイル
  =======================================================================*/
/*========================================================================
  コマンド 説明
 =========================================================================
--------------------------------------------------------------------------
	1.  コマンドラインの様式
	2.  命令の詳細
	3.  スイッチオプションの詳細
--------------------------------------------------------------------------

 1.  コマンドラインの様式

    [<command>] [-<switch>...] <souce_file_name> [<destination_file_name>]

  　命令は、大文字でも小文字でも構いません。
    スイッチは、大文字でも小文字の別があります。スイッチの位置についても問いません。
  　スイッチを表す文字としては '-' と '/' が使えます。

		例： unkanj t -s src.txt dest.txt
             　　src.txt を dest.txt へJISコード変換します。
　　　　　　unkanj t -s src.txt
             　　src.txt を JISコード変換して、標準出力へ表示します。

    現バージョンではファイルの指定にワイルドカードは使用できません。
	また、ディレクトリの指定もできません。ソースファイルとディスティネーションファイル
	は、１対１に対応します。
	*Future> ディレクトリ指定等による、複数ファイルのサポート

	Open() 関数使用時には、つづく Read() 関数でメモリへ変換する為
	ディスティネーションファイルの指定は不要です。
		例： t -s c:\src\src.txt


 2.  命令の詳細

	t: ファイルをコード変換します。

		例： t -s c:\src\src.txt c:\dest\dest.txt
		  ソースファイル c:\src\src.txt を、
		c:\dest\dest.txt shift-jisのファイルに変換します。

		例： t -s "c:\src file\src.txt" "c:\dest file\dest.txt"
		  ソースファイル c:\src file\src.txt を、
		c:\dest file\dest.txt shift-jisのファイルに変換します。
		ディレクトリやファイル名にスペースを使用している場合は、
		"(file)" と言うように"(ダブルコウティション)で括ります。
		最近は、スペースを使用したディレクトリが多いのでスペースの
		あるなしに関らずこの様に指定しておいたほうが無難です。

	c: ファイルの漢字コードをチェックします。

		ソースファイルの漢字コードをチェックし、判定した漢字コードを返します。

		例： c c:\src\src.txt

 3.  スイッチオプションの詳細

	-<switch>:
		※　通常特に記述無き場合は、大文字小文字を区別しません

	1) 漢字コードスイッチ群

		s: shift-jis へ変換します。
		j: jis へ変換します。
		e: euc へ変換します。
		u: utf-8へ変換します。

		S: ソースファイルを、shift-jis と仮定します。
		J: ソースファイルを、jis と仮定します。
		E: ソースファイルを、euc と仮定します。
		U: ソースファイルを、utf-8 と仮定します。

		-S,-J,-E,-U の何れかがない場合は、ソースファイルの漢字コードは、
		未定として自動判別します。
		※　尚、このスイッチ群では、大文字小文字を区別します

	2) 行末コードスイッチ群

		CRLF: CR+LF へ変換します。
		CR: CR へ変換します。
		LF: LF へ変換します。

	3) リードバッファサイズ
		R<nn>: (1～nn)キロバイト以下のリードバッファを設定します。 
		指定なき場合は、64 kbytes となります。
		　ソースファイルの漢字コード自動判定時には、
		ファイルの一番先頭からのリードバッファサイズ迄で判定しています。
		従って、ソースファイルサイズに比べてこの値を極端に小さくすると
		判定精度が低下する事があります。メモリが極端に大きいまたは小さい
		場合で且つ、大規模サイズを扱う場合以外は、特に指定する必要は
		ないと思います。

	4) JISコードへ変換する場合において、KI、KOコードを指定します
		KI<?>:続くアルファベットで、KIコードを指定します
		KO<?>:続くアルファベットで、KOコードを指定します
		
		漢字シフトとスイッチオプションの対応
		スイッチオプション　　　KI　　　　　　　KO          
		-KIB -KOB　<default>  [ESC] $ B     [ESC] ( B
		-KI@ -KOJ　　　　　　　[ESC] $ @     [ESC] ( J
		-KIB -KOJ　　　　　　　[ESC] $ B     [ESC] ( J
		-KIK -KOH　　　　　　　[ESC] K       [ESC] H

		特に指定無き場合は、-KIB -KOBになっています。
		尚、ソースファイルがJISの場合、何れの形式でも対応されています。

	5) 外字　115～119区（IBM 拡張文字）を、変換対象にするか否か
		G<0/1>: 0=変換しない <default>　1=変換する
 =========================================================================*/
/*========================================================================
	UnKanj.dll の使用例
 =========================================================================
	１．DLL初期設定手法について
		付属の typedef 群は、DLL関数へのポインタ宣言に使用できます。
		以下に、コーディング例を示します。
	
	例）ソースファイル
		HINSTANCE		hDll = null;		//UNKANJ.DLLインスタンスハンドル
		DGetVersion		pGetVersion = null;	//GetVersion()関数へのポインタ
		//------ ライブラリロード＆関数ポインタへの取得
		if (hDll = LoadLibrary () {
			pGetVersion = (DGetVersion)GetProcAddress(hDll, "ccGetVersion");
			// please  codeing for get other functions address.
		}
		//------ DLL関数の呼び出し
		unsigned long version = (*pGetVersion)();
		// please call other functions.

		//------ ライブラリの開放
		if (hDll)
			FreeLibrary (hDll);
	from og, thunk you.
 =========================================================================*/
/*========================================================================
  API 説明
 =========================================================================
////////////// スタンダード系API ////////////////////////////////////////
// スタンダード系APIとは変換DLLに必要なAPIで、次の圧縮格納系DLLとの整合性の
// 為のAPIとはその性質上一線を画すものです。
-----------------------------------------------------------------------
	unsigned short WINAPI ccGetVersion(void)
-----------------------------------------------------------------------
機能
	UNKANJ.DLL の現在のバージョンを返します。

戻り値
	現在のバージョン
	5 -> Version 0.05

その他
	AP側での動作確認された、UNKANJ.DLLを特定する場合には
	本バージョンのチェックを推奨します。

-----------------------------------------------------------------------
	int WINAPI ccCommand(const HWND _hwnd, const char * _szCmdLine,
		char * _szOutput, const unsigned long _dwSize)
-----------------------------------------------------------------------
機能
	コマンド文字列を与えて、各種の変換操作を行います。

引数
	_hwnd	UNKANJ.DLL を呼び出すアプリに所属するウィンドウのハンドル。
		    ウィンドウが存在しないコンソールアプリの場合は null を渡します。
	_szCmdLine  UNKANJ.DLL に渡すコマンドの文字列。
	_szOutput   UNKANJ.DLL が結果を返すためのバッファへのポインタ。
			コンソールアプリの場合は、ここに変換結果が出力されます。
		    ソールに表示するのと同様のものが、このバッファに格納さ
		    れます。
	_dwSize	    バッファのサイズ。UNKANJ.DLL が返す結果のサイズより指
			定されたサイズが小さい場合は指定サイズまで出力されます。

戻り値
	UnKanj_Rtn の何れかをセットします。
	正常終了の時			UNKANJI_OK(=0) を返します。
	エラーが発生した場合	UNKANJI_OK(=0) 以外の数。
							エラーコードについては UNKANJ.Hを参照

-----------------------------------------------------------------------
	bool WINAPI ccGetRunning(void)
-----------------------------------------------------------------------
機能
	現在 UNKANJ.DLL が動作中か否かを返します。
	*Future> Open() 後に、Close() されていないタスクが１つでも
	存在する場合は、TRUE を返します。

戻り値
	実行中であれば TRUE
	実行中でなければ FALSE

-----------------------------------------------------------------------
	bool WINAPI ccSetBackGroundMode(const bool _bBackGroundMode)
-----------------------------------------------------------------------
機能
	UNKANJ.DLL のバックグラウンドモードを設定します。

引数
	バックグラウンドモードの場合は TRUE を渡します。
	非バックグラウンドモードの場合は FALSE を渡します。

戻り値
	正常終了の場合 TRUE を返します。
	動作中で変更できなかった場合や、引数が間違いの場合には FALSE を返
	します。

注意
	ここでの「バックグラウンドモード」とは、
	「UNKANJ.DLL が動作中にメッセージを、他タスク処理に明け渡します。」

	この API で FALSE を指定した場合でには、UNKANJ.DLL は、完全に処理時
	間を占有しますので、小さなファイルを変換する事が分かっている場合以外
	は、TRUE に設定することを推奨いたします。
-----------------------------------------------------------------------
	bool WINAPI ccSetOwnerWindow(const HWND _hwnd)
-----------------------------------------------------------------------
機能
	オーナーウィンドウを設定します。

引数
	_hwnd	UNKANJ.DLL を呼び出すアプリのオーナーウィンドウのハンドル。

戻り値
	正常終了の時			TRUE
	エラーが発生した場合	FALSE

-----------------------------------------------------------------------
	HANDLE WINAPI ccOpen(const HWND _hwnd, const char * _szCmdLine,
		char * _szOutput, const unsigned long _dwSize, int *_prtn)
-----------------------------------------------------------------------
機能
	コマンド文字列を与えて、各種の変換操作の指定をします。
	引数の説明は Command() とほぼ同じです。
	通常は、Read() に取得したコード変換ハンドルを引数を渡して
	コールし実際の変換を行います。

引数
	_hwnd	UNKANJ.DLL を呼び出すアプリに所属するウィンドウのハンドル。
		    ウィンドウが存在しないコンソールアプリの場合は null を渡します。
	_szCmdLine  UNKANJ.DLL に渡すコマンドの文字列。
	_szOutput   UNKANJ.DLL が結果を返すためのバッファへのポインタ。
			コンソールアプリの場合は、ここに変換結果が出力されます。
		    ソールに表示するのと同様のものが、このバッファに格納さ
		    れます。
	_dwSize	    バッファのサイズ。UNKANJ.DLL が返す結果のサイズより指
			定されたサイズが小さい場合は指定サイズまで出力されます。
	_prtn	null 以外を指定した場合：
			*_prtn には、UnKanj_Rtn の何れかをセットします。
			正常終了の時	UNKANJI_OK(=0) を返します。
			エラーが発生した場合	UNKANJI_OK(=0) 以外の値
							エラーコードについては UNKANJ.Hを参照
戻り値
	正常終了の時			コード変換ハンドル
	エラーが発生した場合	null

-----------------------------------------------------------------------
	unsigned int WINAPI ccRead(const HANDLE _handle, void* _lpBuf,
		unsigned int _nCount, int *_prtn)
-----------------------------------------------------------------------
機能
	Open() で取得したコード変換ハンドルに基づいて、
	特定ファイルを変換しメモリへロードします。
	１回のコールですべてを変換する必要はありません。

引数
	_handle	Open() で返されたハンドル。
	_lpBuf	変換したデータを格納するバッファへのポインタ
	_nCount	最大格納サイズ
	_prtn	null 以外を指定した場合：
			*_prtn には、UnKanj_Rtn の何れかをセットします。
			正常終了の時	UNKANJI_OK(=0) を返します。
			エラーが発生した場合	UNKANJI_OK(=0) 以外の値
						エラーコードについては UNKANJ.Hを参照
戻り値
	変換したデータサイズ

注意点
	ファイルのリード動作と同様、通常は本関数を複数回コールにより全ての
	変換済みコードを取得できます。つまりコード自動判定するように指定す
	ると特定漢字コードでのファイルリード処理の代わりになります。
	但し変換途中でも _nCount に満たないサイズで戻る場合があるので、
	すべてを変換したか否かは、本関数の戻り値 =0 かもしくは IsEOF()
	関数をコールすることでチェックして下さい。

-----------------------------------------------------------------------
	void WINAPI ccClose(const HANDLE _handle)
-----------------------------------------------------------------------
機能
	Open() で取得したコード変換ハンドルを解放する。

引数
	_handle		Open() で返されたハンドル。

戻り値
	なし

-----------------------------------------------------------------------
	bool WINAPI ccIsEOF(const HANDLE _handle)
-----------------------------------------------------------------------
機能
	Read() 実行後、すべてのコード変換が終了したか否かを返す。

引数
	_handle		Open() で返されたハンドル。

戻り値
	コード変換終了	TRUE
	コード変換途中	FALSE

-----------------------------------------------------------------------
	int WINAPI ccGetType(const HANDLE _handle)
-----------------------------------------------------------------------
機能
	Read() 実行後、ソースファイルの漢字コードタイプを返す。

引数
	_handle		Open() で返されたハンドル。

戻り値
	KanjType	漢字コード種類

////////////// 圧縮格納系API ////////////////////////////////////////
// 圧縮格納系のDLLをサポートするアプリケーションが、
// 本DLLも簡単に使える目的で、存在するAPI群です。
//	＜...＞....UnKanjでの意味になります。
// 尚、スタンダード系APIにも使うであろうAPIが幾つか存在していますので
// そちらも合わせてご参考下さい。
//#ifdef ARC_API_SUPPORT	//ARC系APIサポートするか否か
-----------------------------------------------------------------------
	bool WINAPI ccCheckArchive(const char * _szFileName,
		const int _iMode)
-----------------------------------------------------------------------
機能
	指定ファイルが書庫として正しいかどうかを返します。
	＜現在実質処理は何もしていません、整合性の為だけにあります。＞

引数
	_szFileName	チェックすべき書庫のファイル名。
				＜ソースファイル名＞
				
	_iMode		チェックモード。
				＜どの様な値を指定しても、TRUEを返します＞
戻り値
	＜どの様なファイルを指定しても、現在はTRUEを返します＞

その他
　　チェックするとしたら、テキストファイルか否かでしょうが、何らかの
　　漢字コードをもとにしたものかどうかをみるのは、次のFind系APIと処理が
　　重複して時間が余分にかかるので、敢えてしてません。

-----------------------------------------------------------------------
	int WINAPI ccGetFileCount(const char * _szArcFile)
-----------------------------------------------------------------------
機能
	指定された書庫ファイルに格納されているファイル数を得ます。
	＜現在実質処理は何もしていません、常に３つの漢字コードファイル
	　（jis, euc, s-jis）の３種類という意味で３を返します。＞

引数
	_szArcFile	格納ファイル数を得たい書庫ファイル名。
				＜ソースファイル名＞

戻り値
	格納ファイル数を得る。エラーのときは -1 を返す。
	＜常に３つの漢字コードファイルjis, euc, s-jis）の３種類という
	　意味で３を返します。＞

-----------------------------------------------------------------------
	HARC WINAPI ccOpenArchive(const HWND _hwnd, const char * _szFileName,
				const unsigned long _dwMode)
-----------------------------------------------------------------------
機能
	ハンドルと書庫ファイルを結び付けます。

引数
	_hwnd		呼び出し側のアプリケーションのウィンドウのハンドル。
	_szFileName	書庫ファイル名。
				＜ソースファイル名＞
	_dwMode		FindFirst() 等の処理のモード。
				＜現在、指定すべきものは何もありません。
				　0を指定しておいて下さい。＞

戻り値
	指定の書庫ファイルに対応したハンドル。エラー時は null を返します。

注意
	＜現バージョンでは，同時に複数のハンドルを利用できます。＞

-----------------------------------------------------------------------
	int WINAPI ccCloseArchive(HARC _harc)
-----------------------------------------------------------------------
機能
	OpenArchive() で割り付けたハンドルを解放する。

引数
	_harc		OpenArchive() で返されたハンドル。

戻り値
	正常終了時に０が返されます。

-----------------------------------------------------------------------
	int WINAPI ccFindFirst(HARC _harc, const char * _szWildName,
				INDIVIDUALINFO   *lpSubInfo)
-----------------------------------------------------------------------
機能
	最初の格納ファイルの情報を得ます。

引数
	_harc		OpenArchive() で返されたハンドル。
	_szWildName	検索するファイル名。
				＜現在："*"を指定して下さい。
				（"*":全漢字コードファイルへの変換検索の意味です）＞
	_lpSubInfo	結果を返すための INDIVIDUALINFO 型の構造体へのポイ
			ンタ。結果を必要としない場合は null を指定できます。

戻り値
	0		正常終了。_lpSubInfo  で指定された変数に格納ファイ
			ルの情報を格納する。
	-1		検索終了。

注意
	＜FindFirst() と次の FindNext() では，API の処理の終了時に
	　HARC で関連付けられている書庫は開いたままです。＞

-----------------------------------------------------------------------
	int WINAPI ccFindNext(HARC _harc, INDIVIDUALINFO   *_lpSubInfo)
-----------------------------------------------------------------------
機能
	２番目以降の格納ファイルの情報を得ます。

引数
	_harc		OpenArchive() で返されたハンドル。
	_lpSubInfo	結果を返すための INDIVIDUALINFO 型の構造体へのポイ
			ンタ。結果を必要としない場合は null を指定できます。

戻り値
	0		正常終了。_lpSubInfo  で指定された変数に格納ファイ
			ルの情報を格納する。
	-1		検索終了。
-----------------------------------------------------------------------
%2. INDIVIDUALINFO の構造
-----------------------------------------------------------------------
構造体定義
		typedef struct {
			unsigned long 		dwOriginalSize;
			unsigned long 		dwCompressedSize;
			unsigned long		dwCRC;
			unsigned int		uFlag;
			unsigned int		uOSType;
			unsigned short		wRatio;
			unsigned short		wDate;
			unsigned short 		wTime;
			char		szFileName[FNAME_MAX32 + 1];
			char		dummy1[3];
			char		szAttribute[8];
			char		szMode[8];
		}　INDIVIDUALINFO;

メンバの説明
  　dwOriginalSize		ファイルのサイズ。
						＜ソースファイルサイズになります＞
	dwCompressedSize	圧縮後のサイズ。
						＜この変換ファイルのサイズになります＞
	dwCRC			格納ファイルのチェックサム用 CRC。ただし，
				上位 16 bit は常に０。
						＜常に０になります＞
	uFlag			展開やテストを行った場合，格納ファイル毎の
				処理結果が返されます。 コードは Command() が
				返すものと同じ。その他の場合は０。
	uOSType			このファイルの作成に使われたＯＳ。
					＜常に０になります＞
	wRatio			圧縮率。ただし，パーミルで表してあります。
					＜この漢字コード変換をした場合の確からしさになります。
						１００％に近いほどそれです。＞
	wDate			日付。上位から７ビットが１９８０年からの年
				数，４ビットが月，残りの５ビットが日を
				表す。
				＜ソースファイル日付になります＞
	wTime			時刻。上位から５ビットが時間，６ビットが分，
				残りの５ビットが秒を２で割ったもの。
				＜ソースファイル時間になります＞
	szFileName		アーカイブファイル名。
					＜ソースファイル名になります＞
	szAttribute		格納ファイルの属性。'l' コマンド等でのリス
				ト出力と同じ。
					＜ソースファイル属性になります＞
	szMode			ダミー。
					＜未使用です＞

 =========================================================================*/
//#endif //ARC_API_SUPPORT	//ARC系APIサポートするか否か

/*=======================================================================
    1998.10.20
    UNKANJ.DLL Version 0.05
                   Copyright (c) 1997-98 by og.  All rights reserved.
  =======================================================================*/
/*========================================================================
  ヘッダー
 =========================================================================*/
// ★==================USER-TODO======================★
//   WINDOWS VERSION　であれば、下記をコメントにして下さい。
//#undef WIN32
// ★==================USER-TODO======================★

#ifdef WIN32
	#if !defined(__BORLANDC__)
	#define	_export
	#endif
	typedef	HGLOBAL	HARC;
	#define bool BOOL
#else
	#undef WINAPI
	#define WINAPI
	#define	_export
	typedef void *  HANDLE;
	typedef void *  HWND;
	typedef	void *	HARC;
#endif

/////////////////////////////////////////////////////////////////////////////
// DLL バージョン デファイン
#define UNKANJ_VERSION	((unsigned short)(0006))	//version 0.06

/////////////////////////////////////////////////////////////////////////////
// API デファイン
#define ARC_API_SUPPORT	//ARC系APIサポートするか否か(Default = ON )

#ifndef DATACONVINFO_DEFINES
	#define DATACONVINFO_DEFINES
	#define DATA_TYPE_MAX		4
	#define DATA_TYPE_SUB_MAX	16
	#define DATA_CMD_PARAM_MAX	64
	#define DATA_TITLE_MAX	128
	#define DATA_DESCRIPTION_MAX	512
	typedef struct {
		bool	bSrcFile;						/* ソースファイルに対する情報か否か */
		char	szType[DATA_TYPE_MAX];			/* データタイプ=拡張子 */
		char	szTypeSub[DATA_TYPE_SUB_MAX];	/* サブ・データタイプ=拡張子 */
		unsigned int	majorVersion;			/* データフォーマットメジャーバージョン番号（0～255） */
		unsigned int	minerVersion;			/* データフォーマットマイナーバージョン番号（0～255） */
		char	szTitle[DATA_TITLE_MAX];		/* フォーマット名称（例：MS-Word V8.0） */
		char	szDescription[DATA_DESCRIPTION_MAX];	/* 詳細用説明 */
		char	szCmdParam[DATA_CMD_PARAM_MAX];			/* 変換先・変換元データ種類指定コマンドパラメータ */
		unsigned short	wRatio;							/* データタイプの確からしさの度合い */
														/* ソースファイルである場合のみ */
	}	DATACONVINFO, *LPDATACONVINFO;
#endif //DATACONVINFO_DEFINES

//LEVEL 0 APIs
unsigned short	WINAPI _export ccGetVersion(void);
int		WINAPI _export ccCommand(const HWND _hwnd, const char * szCmdLine, char * szOutput, const unsigned long wSize);
//LEVEL 1 APIs
bool	WINAPI _export ccGetRunning(void);
bool	WINAPI _export ccSetBackGroundMode(const bool BackGroundMode);
bool	WINAPI _export ccSetOwnerWindow(const HWND _hwnd);
HANDLE	WINAPI _export ccOpen(const HWND _hwnd, const char *szSrcFileName, const char * szCmdLineOption, char * szOutput, const unsigned long wSize, int *_prtn);
bool	WINAPI _export ccResetCmdLineOption(const HANDLE _handle, const char * szCmdLineOption);
unsigned int WINAPI _export ccRead(const HANDLE _handle, void* _lpBuf, unsigned int _nCount, int *_prtn);
void	WINAPI _export ccClose(const HANDLE _handle);
bool	WINAPI _export ccIsEOF(const HANDLE _handle);
int		WINAPI _export ccGetType(const HANDLE _handle);

#ifdef ARC_API_SUPPORT	//ARC系APIサポートするか否か
	#ifndef FNAME_MAX32
		#define FNAME_MAX32		512
	#endif
	#ifndef ARC_DECSTRACT
	typedef struct {
		unsigned long 			dwOriginalSize;		/* ファイルのサイズ */
 		unsigned long 			dwCompressedSize;	/* 圧縮後のサイズ */
		unsigned long			dwCRC;				/* 格納ファイルのチェックサム */
		unsigned int			uFlag;				/* 処理結果 */
		unsigned int			uOSType;			/* 書庫作成に使われたＯＳ */
		unsigned short			wRatio;				/* 圧縮率 */
		unsigned short			wDate;				/* 格納ファイルの日付(DOS 形式) */
		unsigned short 			wTime;				/* 格納ファイルの時刻(〃) */
		char			szFileName[FNAME_MAX32 + 1];	/* 書庫名 */
		char			dummy1[3];
		char			szAttribute[8];		/* 格納ファイルの属性(書庫固有) */
		char			szMode[8];			/* 格納ファイルの格納モード(〃) */
	}	INDIVIDUALINFO, *LPINDIVIDUALINFO;
	#endif //ARC_DECSTRACT
	bool	WINAPI _export ccCheckArchive(const char * _szFileName,	const int _iMode);	//true only in unkanj
#define MAX_SUPPORT_KANJCODES	(4)
	int		WINAPI _export ccGetFileCount(const char * _szArcFile);//this function is simply about return MAX_SUPPORT_KANJCODES numbers.
	HARC	WINAPI _export ccOpenArchive(const HWND _hwnd, const char * _szFileName, const unsigned long _dwMode);
	int		WINAPI _export ccCloseArchive(HARC _harc);
	int		WINAPI _export ccFindFirst(HARC _harc, const char * _szWildName, INDIVIDUALINFO *lpSubInfo);
	int		WINAPI _export ccFindNext(HARC _harc, INDIVIDUALINFO   *_lpSubInfo);
#endif //ARC_API_SUPPORT	//ARC系APIサポートするか否か

/////////////////////////////////////////////////////////////////////////////
// エラー＆ワーニングコード・デファイル
// 関数でのpRtnへのリターンコード
enum UnKanj_Rtn{
	UNKANJI_ERROR_MEMORY			= -7,
	UNKANJI_ERROR_DESTINATION_FILE	= -6,
	UNKANJI_ERROR_SOURCE_FILE		= -5,
	UNKANJI_ERROR_PARAM				= -4,
	UNKANJI_SAME_CONVERT			= -3,
	UNKANJI_INVAL_TEXT				= -2,
	UNKANJI_OK						= 0,
	UNKANJI_EOF						= 1,
};

/////////////////////////////////////////////////////////////////////////////
// DLL 漢字コード種類 ccGetType()で返します。
enum KanjType{
	kt_binary=-1,
	kt_ascii,
	kt_jis,
	kt_euc,
	kt_sjis,
	kt_utf8,
};


//関数へのポインタプロトタイプ宣言

typedef	unsigned short (WINAPI _export* DGetVersion)(void);
typedef	int		(WINAPI _export* DCommand)(const HWND _hwnd, const char * szCmdLine, char * szOutput, const unsigned long wSize);

typedef	bool	(WINAPI _export* DSetBackGroundMode)(const bool BackGroundMode);
typedef bool	(WINAPI _export *DGetRunning)(void);
typedef bool	(WINAPI _export *DSetOwnerWindow)(const HWND _hwnd);
typedef HANDLE	(WINAPI _export* DOpen)(const HWND _hwnd, const char * szCmdLine, char * szOutput, const unsigned long wSize, int *_prtn);
typedef unsigned int (WINAPI _export* DRead)(const HANDLE _handle, void* _lpBuf, unsigned int _nCount, int *_prtn);
typedef void	(WINAPI _export *DClose)(HANDLE _handle);
typedef bool	(WINAPI _export *DIsEOF)(const HANDLE _handle);
typedef	int		(WINAPI _export *DGetType)(const HANDLE _handle);

#ifdef ARC_API_SUPPORT	//ARC系APIサポートするか否か
	typedef bool	(WINAPI _export *DCheckArchive)(const char * _szFileName,	const int _iMode);
	typedef int		(WINAPI _export *DGetFileCount)(const char * _szArcFile);
	typedef HARC	(WINAPI _export	*DOpenArchive)(const HWND _hwnd, const char * _szFileName, const unsigned long _dwMode);
	typedef int		(WINAPI _export *DCloseArchive)(HARC _harc);
	typedef int		(WINAPI _export *DFindFirst)(HARC _harc, const char * _szWildName, INDIVIDUALINFO   *lpSubInfo);
	typedef int		(WINAPI _export *DFindNext)(HARC _harc, INDIVIDUALINFO   *_lpSubInfo);
#endif //ARC_API_SUPPORT	//ARC系APIサポートするか否か

#ifdef WIN32
	#undef bool
#endif
#endif	/* UNKANJ_H */
