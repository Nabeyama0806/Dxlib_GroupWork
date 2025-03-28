#include "CreateDXFontData.h"
#include "FileLib.h"
#include "DXArchive.h"
#include "CharCode.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

// キャラセットテーブル
DWORD CharSetTable[ 8 ] =
{
	DEFAULT_CHARSET,
	SHIFTJIS_CHARSET,
	HANGEUL_CHARSET,
	CHINESEBIG5_CHARSET,
	GB2312_CHARSET,
	ANSI_CHARSET,
	ANSI_CHARSET,
	DEFAULT_CHARSET,
} ;

DWORD CharCodeFormatTable[ 8 ] =
{
	CHARCODEFORMAT_SHIFTJIS,
	CHARCODEFORMAT_SHIFTJIS,
	CHARCODEFORMAT_UHC,
	CHARCODEFORMAT_BIG5,
	CHARCODEFORMAT_GB2312,
	CHARCODEFORMAT_WINDOWS_1252,
	CHARCODEFORMAT_ISO_IEC_8859_15,
	CHARCODEFORMAT_UTF8,
} ;

FONTSYSTEM_WIN FontSystem_Win ;
FONTSYSTEM FontSystem ;

// フォント処理の初期化
extern int InitializeFont( void )
{
	int i ;
	int j ;
	int k ;

	// テーブルを作成する
	for( i = 0 ; i < 256 ; i ++ )
	{
		j = i ;
		for( k = 0 ; j != 0 ; k ++, j &= j - 1 ){}
		FontSystem.BitCountTable[ i ] = ( BYTE )k ;
	}

	return 0 ;
}

// フォント列挙用コールバック関数
int CALLBACK EnumFontFamExProcEx( const ENUMLOGFONTEXW *lpelf, const NEWTEXTMETRICEXW * /*lpntm*/, DWORD /*nFontType*/, LPARAM lParam )
{
	ENUMFONTDATA *EnumFontData = ( ENUMFONTDATA * )lParam ;

	// 横向きフォント(@付)はいずれもはじく
	if( lpelf->elfFullName[0] != L'@' )
	{
		int i ;

		// 同じフォント名が以前にもあった場合は弾く
		for( i = 0 ; i < EnumFontData->FontNum ; i ++ )
		{
			if( CL_strcmp( CHARCODEFORMAT_UTF16LE, ( char * )&lpelf->elfFullName[0], ( char * )&EnumFontData->FontBuffer[64 * i] ) == 0 )
			{
				return TRUE ;
			}
		}

		// ネームを保存する
		CL_strcpy( CHARCODEFORMAT_UTF16LE, ( char * )&EnumFontData->FontBuffer[ 64 * EnumFontData->FontNum ], ( char * )&lpelf->elfFullName[0] ) ;

		// フォントの数を増やす
		EnumFontData->FontNum ++ ;

		// もしバッファの数が限界に来ていたら列挙終了
		if( EnumFontData->BufferNum != 0 && EnumFontData->BufferNum == EnumFontData->FontNum )
		{
			return FALSE ;
		}
	}

	// 終了
	return TRUE ;
}

// EnumFontName の Windows 環境依存処理を行う関数
extern int EnumFontName_Win( ENUMFONTDATA *EnumFontData, int CharSet )
{
	HDC			hdc ;
	LOGFONTW	LogFont ;

	// デバイスコンテキストを取得
	hdc = GetDC( NULL );

	// 列挙開始
	memset( &LogFont, 0, sizeof( LOGFONTW ) ) ;
	LogFont.lfCharSet = ( BYTE )( CharSet < 0 ? DEFAULT_CHARSET : CharSetTable[ CharSet ] ) ;
	if( EnumFontData->EnumFontName != NULL )
	{
		CL_strncpy( CHARCODEFORMAT_UTF16LE, ( char * )LogFont.lfFaceName, ( char * )EnumFontData->EnumFontName, 31 ) ;
	}
	else
	{
		LogFont.lfFaceName[0] = L'\0' ;
	}
	LogFont.lfPitchAndFamily	= 0 ;
	EnumFontFamiliesExW( hdc, &LogFont, ( FONTENUMPROCW )EnumFontFamExProcEx, ( LPARAM )EnumFontData, 0  ) ;

	// デバイスコンテキストの解放
	ReleaseDC( NULL, hdc ) ;

	// 正常終了
	return 0 ;
}

// フォントを列挙する
static int EnumFontName( wchar_t *NameBuffer, int NameBufferNum, int CharSet, const wchar_t *EnumFontName )
{
	ENUMFONTDATA	EnumFontData ;
	wchar_t			*DestBuffer;
	int				Result ;

	EnumFontData.FontNum		= 0 ;
	EnumFontData.EnumFontName	= EnumFontName ;

	if( NameBuffer == NULL )
	{
		DestBuffer = ( wchar_t * )malloc( 1024 * 256 ) ;
	}
	else
	{
		DestBuffer = NameBuffer ;
	}

	EnumFontData.FontBuffer		= DestBuffer ;
	EnumFontData.BufferNum		= NameBufferNum ;

	// 環境依存処理
	Result = EnumFontName_Win( &EnumFontData, CharSet ) ;

	// メモリの解放
	if( NameBuffer == NULL )
	{
		free( DestBuffer ) ;
		DestBuffer = NULL ;
	}

	// エラーチェック
	if( Result < 0 )
	{
		return -1 ;
	}

	// フォントが存在したか、若しくはフォントデータ領域数を返す
	return EnumFontData.FontNum ;
}

// CreateFontManageData のWindows環境依存処理を行う関数
static int CreateFontManageData_Win( FONTMANAGE *ManageData, int DefaultCharSet )
{
	int CreateFontSize ;
	int SampleScale ;
	int EnableAddHeight	= FALSE ;
	int	AddHeight		= 0 ;
	int	OrigHeight		= 0 ;

	switch( ManageData->ImageBitDepth )
	{
	default:
	case DX_FONTIMAGE_BIT_1:
		SampleScale = 1 ;
		break ;

	case DX_FONTIMAGE_BIT_4:
		SampleScale = 4 ;
		break ;

	case DX_FONTIMAGE_BIT_8:
		SampleScale = 16 ;
		break ;
	}
	CreateFontSize = ManageData->BaseInfo.FontSize * SampleScale ;

CREATEFONTLABEL:

	// 既にフォントが作成されていたら削除
	if( ManageData->FontObj != NULL )
	{
		DeleteObject( ManageData->FontObj ) ;
	}

	if( ManageData->FontName[0] != L'\0' )
	{
		// 特に文字セットの指定が無い場合で、且つ指定のフォント名の指定の文字セットが無い場合は文字セットを DEFAULT_CHARSET にする
		if( DefaultCharSet == TRUE )
		{
			wchar_t	TempNameBuffer[ 16 ][ 64 ] ;
			wchar_t	*TempNameBufferP ;
			int		TempNameNum ;
			int		TempNameBufferAlloc ;
			int		i ;

			TempNameNum = EnumFontName( TempNameBuffer[ 0 ], 16, ManageData->BaseInfo.CharSet, ManageData->FontName ) ;
			if( TempNameNum >= 16 )
			{
				TempNameNum			= EnumFontName( NULL,            0,           ManageData->BaseInfo.CharSet, ManageData->FontName ) ;
				TempNameBufferP		= ( wchar_t * )malloc( TempNameNum * 64 * sizeof( wchar_t ) ) ;
				TempNameNum			= EnumFontName( TempNameBufferP, TempNameNum, ManageData->BaseInfo.CharSet, ManageData->FontName ) ;
				TempNameBufferAlloc = TRUE ;
			}
			else
			{
				TempNameBufferAlloc = FALSE ;
				TempNameBufferP		= TempNameBuffer[ 0 ] ;
			}

			for( i = 0 ; i < TempNameNum && CL_strcmp( CHARCODEFORMAT_UTF16LE, ( char * )( TempNameBufferP + i * 64 ), ( char * )ManageData->FontName ) != 0 ; i ++ ){}
			if( i == TempNameNum )
			{
				ManageData->BaseInfo.CharSet = DX_CHARSET_DEFAULT ;
			}

			if( TempNameBufferAlloc )
			{
				free( TempNameBufferP ) ;
				TempNameBufferP = NULL ;
			}
		}

		ManageData->FontObj = CreateFontW(
			CreateFontSize + AddHeight, 0, 0, 0,
			ManageData->BaseInfo.FontThickness * 100,
			ManageData->BaseInfo.Italic, FALSE, FALSE,
			CharSetTable[ ManageData->BaseInfo.CharSet ],
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			NONANTIALIASED_QUALITY, FIXED_PITCH,
			ManageData->FontName
		) ;

		if( ManageData->FontObj == NULL )
		{
			ManageData->FontObj = CreateFontW(
				CreateFontSize + AddHeight, 0, 0, 0,
				ManageData->BaseInfo.FontThickness * 100,
				ManageData->BaseInfo.Italic, FALSE, FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				NONANTIALIASED_QUALITY, FIXED_PITCH,
				ManageData->FontName
			) ;

			if( ManageData->FontObj == NULL )
			{
				ManageData->FontObj = CreateFontW(
					CreateFontSize + AddHeight, 0, 0, 0,
					ManageData->BaseInfo.FontThickness * 100,
					ManageData->BaseInfo.Italic, FALSE, FALSE,
					SHIFTJIS_CHARSET,
					OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					NONANTIALIASED_QUALITY, FIXED_PITCH,
					ManageData->FontName
				) ;

				if( ManageData->FontObj == NULL )
				{
					wprintf( L"指定のフォントの作成に失敗しました\n" ) ;
					goto ERR ;
				}
			}
		}
	}
	else
	{
		ManageData->FontObj = CreateFontW(
			CreateFontSize + AddHeight, 0, 0, 0,
			ManageData->BaseInfo.FontThickness * 100,
			ManageData->BaseInfo.Italic, FALSE, FALSE,
			CharSetTable[ ManageData->BaseInfo.CharSet ],
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			NONANTIALIASED_QUALITY, FIXED_PITCH,
			L"ＭＳ ゴシック"
		) ;
		ManageData->FontName[0] = L'\0' ;
	}

	if( ManageData->FontObj == NULL )
	{
		ManageData->FontObj = CreateFontW(
			CreateFontSize + AddHeight, 0, 0, 0,
			ManageData->BaseInfo.FontThickness * 100,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			NONANTIALIASED_QUALITY, FIXED_PITCH,
			L"ＭＳ ゴシック"
		) ;

		if( ManageData->FontObj == NULL )
		{
			ManageData->FontObj = CreateFontW(
				CreateFontSize + AddHeight, 0, 0, 0,
				ManageData->BaseInfo.FontThickness * 100,
				FALSE, FALSE, FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				NONANTIALIASED_QUALITY, FIXED_PITCH,
				NULL
			) ;
		}
		ManageData->FontName[0] = L'\0' ;

		if( ManageData->FontObj == NULL )
		{
			wprintf( L"フォントの作成に失敗しました\n" ) ;
			goto ERR ;
		}
	}

	// 文字のサイズを取得する
	{
		HDC			DC ;
		HFONT		OldFont ;
		TEXTMETRIC	TextInfo ;

		// デバイスコンテキストを取得
		DC = CreateCompatibleDC( NULL ) ;
		if( DC == NULL )
		{
			wprintf( L"デバイスコンテキストの取得に失敗しました\n" ) ;
			goto ERR ;
		}

		// フォントのセット
		OldFont = ( HFONT )SelectObject( DC, ManageData->FontObj ) ;

		// フォントのステータスを取得
		GetTextMetrics( DC, &TextInfo ) ;

		// もし TextInfo.tmInternalLeading + TextInfo.tmExternalLeading が 0 ではなかったらその高さを追加してフォントを作成しなおす
		if( EnableAddHeight == FALSE )
		{
			if( TextInfo.tmInternalLeading + TextInfo.tmExternalLeading > 0 )
			{
				OrigHeight		= TextInfo.tmHeight ;
				AddHeight		= ( int )( CreateFontSize / ( 1.0f - ( float )( TextInfo.tmInternalLeading + TextInfo.tmExternalLeading ) / TextInfo.tmHeight ) ) - CreateFontSize ;
				EnableAddHeight	= TRUE ;

				SelectObject( DC, OldFont ) ;
				DeleteDC( DC ) ;

				DeleteObject( ManageData->FontObj ) ;
				ManageData->FontObj = NULL ;

				goto CREATEFONTLABEL ;
			}
			
			ManageData->BaseInfo.FontAddHeight = 0 ;
		}
		else
		{
			ManageData->BaseInfo.FontAddHeight = ( WORD )( ( TextInfo.tmHeight - OrigHeight ) / SampleScale ) ;
		}

		// フォントの最大サイズを取得
		if( ManageData->BaseInfo.Italic )
		{
			// イタリック体の場合は最大幅が 1.35倍になる
			ManageData->BaseInfo.MaxWidth = ( WORD )( ( TextInfo.tmMaxCharWidth * 135 / SampleScale + 4 * 135 ) / 100 ) ;
		}
		else
		{
			ManageData->BaseInfo.MaxWidth = ( WORD )( TextInfo.tmMaxCharWidth / SampleScale + 4 ) ;
		}

		// フォントの高さを保存
		ManageData->BaseInfo.FontHeight = ( WORD )( TextInfo.tmHeight / SampleScale + 1 ) ;

		// ベースラインから一番上までの高さを保存
		ManageData->BaseInfo.Ascent = ( WORD )( TextInfo.tmAscent / SampleScale ) ;
		if( TextInfo.tmAscent % SampleScale >= SampleScale / 2 )
		{
			ManageData->BaseInfo.Ascent ++ ;
		}

		// GetGlyphOutline が使用できるかどうかを調べる
		{
			GLYPHMETRICS	gm ;
			MAT2			mt = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } } ;
			unsigned int	Code ;
			DWORD			DataSize ;

			memset( &gm, 0, sizeof( GLYPHMETRICS ) ) ;
			Code		= ' ' ;
			DataSize	= GetGlyphOutline( DC, Code, GGO_BITMAP, &gm, 0, NULL, &mt ) ;

			// 失敗した場合は TextOut 方式を使用する
			if( DataSize == GDI_ERROR )
			{
				ManageData->UseTextOut = TRUE ;
			}
		}

		// TextOut 方式を使用する場合は DIB を作成しておく
		ManageData->CacheBitmap			= NULL ;
		ManageData->CacheBitmapMem		= NULL ;
		ManageData->CacheBitmapMemPitch	= 0 ;
		if( ManageData->UseTextOut )
		{
			BITMAPINFO	*BmpInfoPlus ;
			BITMAP		BmpData ;

			// イメージビット深度も DX_FONTIMAGE_BIT_1 のみ
			ManageData->ImageBitDepth = DX_FONTIMAGE_BIT_1 ;

			// キャッシュ領域のステータスの初期化
			BmpInfoPlus = ( BITMAPINFO * )malloc( sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * 256 ) ;
			if( BmpInfoPlus == NULL )
			{
				wprintf( L"メモリの確保に失敗しました\n" ) ;
				return -1 ;
			}
			BmpInfoPlus->bmiHeader.biSize			= sizeof( BITMAPINFOHEADER ) ;
			BmpInfoPlus->bmiHeader.biWidth			=  ManageData->BaseInfo.MaxWidth ;
			BmpInfoPlus->bmiHeader.biHeight			= -ManageData->BaseInfo.MaxWidth ;
			BmpInfoPlus->bmiHeader.biPlanes			= 1 ;
			BmpInfoPlus->bmiHeader.biBitCount		= 8 ;
			BmpInfoPlus->bmiHeader.biCompression	= BI_RGB ;
			BmpInfoPlus->bmiHeader.biSizeImage		= ( DWORD )( ManageData->BaseInfo.MaxWidth * ManageData->BaseInfo.MaxWidth ) ;

			// カラーパレットのセット
			{
				RGBQUAD *Color ;
				int		i ;

				Color = &BmpInfoPlus->bmiColors[0] ;
				for( i = 0 ; i < 256 ; i ++ )
				{
					Color->rgbBlue     = ( BYTE )i ;
					Color->rgbRed      = ( BYTE )i ;
					Color->rgbBlue     = ( BYTE )i ;
					Color->rgbReserved = 0 ;

					Color ++ ;
				}
			}

			// ＤＩＢデータを作成する
			ManageData->CacheBitmapMem	= NULL ; 
			ManageData->CacheBitmap		= CreateDIBSection( DC, BmpInfoPlus, DIB_PAL_COLORS, ( void ** )&ManageData->CacheBitmapMem, NULL, 0 ) ;

			// ピッチを得る
			GetObject( ManageData->CacheBitmap, sizeof( BITMAP ), &BmpData ) ;
			ManageData->CacheBitmapMemPitch = BmpData.bmWidthBytes ;

			// メモリの解放
			free( BmpInfoPlus ) ;
		}

		// カーニングペア情報を取得する
		ManageData->BaseInfo.KerningPairNum = 0 ;
		ManageData->BaseInfo.KerningPairNum = GetKerningPairs( DC, 0, NULL ) ;
		if( ManageData->BaseInfo.KerningPairNum > 0 )
		{
			int i, j ;
			KERNINGPAIR *KerningPair = NULL ;

			KerningPair = ( KERNINGPAIR * )malloc( sizeof( KERNINGPAIR ) * ManageData->BaseInfo.KerningPairNum ) ;
			if( KerningPair == NULL )
			{
				wprintf( L"カーニングペア情報を一時的に格納するメモリの確保に失敗しました\n" ) ;
				goto ERR ;
			}
			ManageData->KerningPairData = ( FONTDATAFILEKERNINGPAIRDATA * )malloc( sizeof( FONTDATAFILEKERNINGPAIRDATA ) * ManageData->BaseInfo.KerningPairNum ) ;
			if( ManageData->KerningPairData == NULL )
			{
				wprintf( L"カーニングペア情報を格納するメモリの確保に失敗しました\n" ) ;
				goto ERR ;
			}
			memset( ManageData->KerningPairData, 0, sizeof( FONTDATAFILEKERNINGPAIRDATA ) * ManageData->BaseInfo.KerningPairNum ) ;

			GetKerningPairs( DC, ManageData->BaseInfo.KerningPairNum, KerningPair ) ;

			// カーニングペア情報をソートしながら保存
			for( i = 0 ; ( DWORD )i < ManageData->BaseInfo.KerningPairNum ; i ++ )
			{
				for( j = 0 ; j < i ; j ++ )
				{
					if( ( ( ( DWORD )( ManageData->KerningPairData[ j ].Second       ) ) | 
						  ( ( DWORD )( ManageData->KerningPairData[ j ].First  << 16 ) ) ) >=
						( ( ( DWORD )( KerningPair[ i ].wSecond       ) ) |
						  ( ( DWORD )( KerningPair[ i ].wFirst  << 16 ) ) ) )
					{
						break ;
					}
				}

				if( j != i )
				{
					memmove( &ManageData->KerningPairData[ j + 1 ], &ManageData->KerningPairData[ j ], sizeof( FONTDATAFILEKERNINGPAIRDATA ) * ( i - j ) ) ;
				}

				ManageData->KerningPairData[ j ].First = KerningPair[ i ].wFirst ;
				ManageData->KerningPairData[ j ].Second = KerningPair[ i ].wSecond ;
				ManageData->KerningPairData[ j ].KernAmount = ( KerningPair[ i ].iKernAmount + ( SampleScale >> 2 ) ) / SampleScale ;
			}

			if( KerningPair != NULL )
			{
				free( KerningPair ) ;
				KerningPair = NULL ;
			}
		}

		// フォントを元に戻す
		SelectObject( DC, OldFont ) ;

		// デバイスコンテキストを削除する
		DeleteDC( DC ) ;
	}

	// 正常終了
	return 0 ;

	// エラー処理
ERR :

	return -1 ;
}

// フォントデータの作成
extern FONTMANAGE *CreateFontManageData(
	const wchar_t *	FontName,
	int				Size,
	int				ImageBitDepth /* DX_FONTIMAGE_BIT_1等 */ , 
	int				Thick,
	int				Italic,
	int				CharSet,
	int				CharCodeFormat
)
{
	FONTMANAGE *	ManageData		= NULL ;
	int				DefaultCharSet	= FALSE ;

	if( Size == -1 )
	{
		Size = DEFAULT_FONT_SIZE ;
	}

	if( Thick == -1 )
	{
		Thick = DEFAULT_FONT_THICKNESS ;
	}

	if( CharSet == -1 )
	{
		DefaultCharSet = TRUE ;
		CharSet = DX_CHARSET_SHFTJIS ;
	}

	if( CharCodeFormat == -1 )
	{
		if( DefaultCharSet )
		{
			CharCodeFormat = 0xffff ;
		}
		else
		{
			CharCodeFormat = CharCodeFormatTable[ CharSet ] ;
		}
	}

	ManageData = ( FONTMANAGE * )malloc( sizeof( FONTMANAGE ) ) ;
	if( ManageData == NULL )
	{
		return NULL ;
	}
	memset( ManageData, 0, sizeof( FONTMANAGE ) ) ;

	// フォントのパラメータのセット
	ManageData->ImageBitDepth			= ImageBitDepth ;
	ManageData->BaseInfo.FontSize 		= ( WORD )Size ;
	ManageData->BaseInfo.FontThickness 	= ( WORD )Thick ;
	ManageData->BaseInfo.Italic			= ( BYTE )Italic ;
	ManageData->BaseInfo.CharSet		= ( WORD )CharSet ;
	ManageData->BaseInfo.CharCodeFormat		= ( WORD )CharCodeFormat ;

	// フォント名の保存
	if( FontName == NULL )
	{
		ManageData->FontName[0] = L'\0' ;
	}
	else
	{
		CL_strcpy( CHARCODEFORMAT_UTF16LE, ( char * )ManageData->FontName, ( char * )FontName ) ;
	}

	// Windows 環境依存処理
	if( CreateFontManageData_Win( ManageData, DefaultCharSet ) != 0 )
	{
		return NULL ;
	}

	// フォントの最大幅を 8 の倍数にする
	ManageData->BaseInfo.MaxWidth = ( WORD )( ( ManageData->BaseInfo.MaxWidth + 7 ) / 8 * 8 ) ;

	// フォントの高さを２の倍数にする
	ManageData->BaseInfo.FontHeight = ( ManageData->BaseInfo.FontHeight + 1 ) / 2 * 2 ;

	// フォントの高さの方が最大幅より大きかったら最大幅をフォントの高さにする
	if( ManageData->BaseInfo.MaxWidth < ManageData->BaseInfo.FontHeight )
	{
		ManageData->BaseInfo.MaxWidth = ManageData->BaseInfo.FontHeight;
	}

	// 文字キャッシュの作成
	{
		// １ピクセル分のデータを保存するに当たり必要なビット数をセット
		switch( ManageData->ImageBitDepth )
		{
		case DX_FONTIMAGE_BIT_1 :
			ManageData->CacheDataBitNum = 1 ;
			break ;

		case DX_FONTIMAGE_BIT_4 :
		case DX_FONTIMAGE_BIT_8 :
			ManageData->CacheDataBitNum = 8 ;
			break ;
		}

		// キャッシュ用メモリの確保
		ManageData->CachePitch = ( ManageData->CacheDataBitNum * ManageData->BaseInfo.MaxWidth + 7 ) / 8 ;
		ManageData->CacheMem = (unsigned char *)malloc( ( size_t )( ManageData->CachePitch * ManageData->BaseInfo.MaxWidth ) ) ;
		if( ManageData->CacheMem == NULL )
		{
			wprintf( L"フォントのキャッシュ用メモリの確保に失敗しました" ) ;
			return NULL ;
		}
	}

	// 正常終了
	return ManageData ;
}

// DeleteFontManageData の環境依存エラー処理を行う関数
extern int DeleteFontManageData_Win( FONTMANAGE *ManageData )
{
	// フォントイメージ取得用に使用したメモリの解放
	if( ManageData->GetGlyphOutlineBuffer != NULL )
	{
		free( ManageData->GetGlyphOutlineBuffer ) ;
		ManageData->GetGlyphOutlineBuffer = NULL ;
		ManageData->GetGlyphOutlineBufferSize = 0 ;
	}

	// フォントオブジェクトを削除
	if( ManageData->FontObj != NULL )
	{
		DeleteObject( ManageData->FontObj ) ;
		ManageData->FontObj = NULL ;
	}

	// TextOut を使用するフォントで使用するビットマップを解放
	if( ManageData->CacheBitmap != NULL )
	{
		DeleteObject( ManageData->CacheBitmap ) ;
		ManageData->CacheBitmap = NULL ;
		ManageData->CacheBitmapMem = NULL ;
	}

	// 終了
	return 0 ;
}

// フォントデータの後始末
extern int DeleteFontManageData( FONTMANAGE *ManageData )
{
	// Windows 環境依存処理
	DeleteFontManageData_Win( ManageData ) ;

	// テキストキャッシュ用メモリの解放
	if( ManageData->CacheMem != NULL )
	{
		free( ManageData->CacheMem ) ;
		ManageData->CacheMem = NULL ;
	}

	// カーニングペア情報用に確保していたメモリの解放
	if( ManageData->KerningPairData != NULL )
	{
		free( ManageData->KerningPairData ) ;
		ManageData->KerningPairData = NULL ;
	}

	// 終了
	return 0 ;
}

// 指定のフォントデータに画像を転送する
extern int FontCacheCharImageBlt(
	FONTMANAGE *	ManageData,
	DWORD			CharCode,
	int				Space,
	int				ImageType /* DX_FONT_SRCIMAGETYPE_1BIT 等 */,
	void *			ImageBuffer,
	DWORD			ImageSizeX,
	DWORD			ImageSizeY,
	DWORD			ImagePitch,
	int				ImageDrawX,
	int				ImageDrawY,
	int				ImageAddX
)
{
	BYTE *			ResizeBuffer = NULL ;
	DWORD			SampleScale ;
	DWORD			DrawModDrawY ;
	DWORD			DataHeight ;

	// 画像の倍率をセット
	switch( ImageType )
	{
	case DX_FONT_SRCIMAGETYPE_1BIT_SCALE4 :
		SampleScale = 4 ;
		break ;

	case DX_FONT_SRCIMAGETYPE_1BIT_SCALE8 :
		SampleScale = 8 ;
		break ;

	case DX_FONT_SRCIMAGETYPE_1BIT_SCALE16 :
		SampleScale = 16 ;
		break ;

	default :
		SampleScale = 1 ;
		break ;
	}

	if( Space )
	{
		ManageData->CharaData.DrawX = 0 ;
		ManageData->CharaData.DrawY = 0 ;
		ManageData->CharaData.AddX = ( short )( ( ImageAddX + ( SampleScale >> 1 ) ) / SampleScale ) ;
		ManageData->CharaData.SizeX = 0 ;
		ManageData->CharaData.SizeY = 0 ;
	}
	else
	if( ImageBuffer == NULL )
	{
		ManageData->CharaData.DrawX = 0 ;
		ManageData->CharaData.DrawY = 0 ;
		ManageData->CharaData.AddX = ( short )( ( ImageAddX + ( SampleScale >> 1 ) ) / SampleScale ) ;
		ManageData->CharaData.SizeX = 0 ;
		ManageData->CharaData.SizeY = 0 ;
	}
	else
	{
		ManageData->CharaData.DrawX = ( short )( ( ImageDrawX + ( SampleScale >> 2 ) ) / SampleScale ) ;
		ManageData->CharaData.SizeX = ( WORD  )( ( ImageSizeX +   SampleScale - 1    ) / SampleScale ) ;
		ManageData->CharaData.AddX  = ( short )( ( ImageAddX  + ( SampleScale >> 2 ) ) / SampleScale ) ;

		DrawModDrawY    = ( DWORD )( ImageDrawY % SampleScale ) ;
		DataHeight      = ImageSizeY + DrawModDrawY ;
		ManageData->CharaData.DrawY = ( short )(   ImageDrawY                          / SampleScale ) ;
		ManageData->CharaData.SizeY = ( WORD  )( ( DataHeight +   SampleScale - 1    ) / SampleScale ) ;

		// 文字イメージを一時的に保存するメモリ領域を初期化
		memset(
			ManageData->CacheMem,
			0,
			( size_t )( ManageData->CachePitch * ManageData->BaseInfo.MaxWidth )
		) ;

		// 拡大画像の場合はここで縮小画像を取得する
		if( SampleScale > 1 )
		{
			BYTE *	RDataBuffer ;
			DWORD	RSrcPitch ;
			DWORD	ImageAddPitch ;
			DWORD	ImagePitch2 ;
			DWORD	ImagePitch3 ;
			DWORD	ImagePitch4 ;
			DWORD	ImagePitch5 ;
			DWORD	ImagePitch6 ;
			DWORD	ImagePitch7 ;
			DWORD	ImagePitch8 ;
			DWORD	ImagePitch9 ;
			DWORD	ImagePitch10 ;
			DWORD	ImagePitch11 ;
			DWORD	ImagePitch12 ;
			DWORD	ImagePitch13 ;
			DWORD	ImagePitch14 ;
			DWORD	ImagePitch15 ;
			BYTE *	RSrc ;
			BYTE *	RDest ;
			DWORD	RWidth ;
			DWORD	RHeight ;
			DWORD	MHeight ;
			DWORD	HWidth ;
			DWORD	i ;
			DWORD	j ;

			RWidth	= ( ManageData->CharaData.SizeX + 1 ) / 2 * 2 ;
			HWidth	= RWidth / 2 ;
			RHeight	= DataHeight / SampleScale ;
			MHeight	= DataHeight % SampleScale ;

			RSrcPitch = RWidth + 4 ;

			// 縮小後のデータを格納するメモリを確保
			ResizeBuffer = ( BYTE * )malloc( ( size_t )( RSrcPitch * ( ManageData->CharaData.SizeY + 2 ) ) ) ;
			if( ResizeBuffer == NULL )
			{
				wprintf( L"文字イメージリサンプリング用バッファの確保に失敗しました\n" ) ;
				return -1 ;
			}
			memset( ResizeBuffer, 0, ( size_t )( RSrcPitch * ( ManageData->CharaData.SizeY + 2 ) ) ) ;
			RDataBuffer = ResizeBuffer + RSrcPitch ;

			RSrc			= ( BYTE * )ImageBuffer - DrawModDrawY * ImagePitch ;
			RDest			= RDataBuffer ;
			ImageAddPitch	= ( DWORD )( ImagePitch * SampleScale ) ;

			ImagePitch2		= ( DWORD )( ImagePitch * 2 ) ;
			ImagePitch3		= ( DWORD )( ImagePitch * 3 ) ;
			ImagePitch4		= ( DWORD )( ImagePitch * 4 ) ;
			ImagePitch5		= ( DWORD )( ImagePitch * 5 ) ;
			ImagePitch6		= ( DWORD )( ImagePitch * 6 ) ;
			ImagePitch7		= ( DWORD )( ImagePitch * 7 ) ;
			ImagePitch8		= ( DWORD )( ImagePitch * 8 ) ;
			ImagePitch9		= ( DWORD )( ImagePitch * 9 ) ;
			ImagePitch10	= ( DWORD )( ImagePitch * 10 ) ;
			ImagePitch11	= ( DWORD )( ImagePitch * 11 ) ;
			ImagePitch12	= ( DWORD )( ImagePitch * 12 ) ;
			ImagePitch13	= ( DWORD )( ImagePitch * 13 ) ;
			ImagePitch14	= ( DWORD )( ImagePitch * 14 ) ;
			ImagePitch15	= ( DWORD )( ImagePitch * 15 ) ;

			// リサンプルスケールによって処理を分岐
			switch( SampleScale )
			{
				// ４倍の場合
			case 4 :
				ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX16 ;
				for( i = 0 ; i < RHeight ; i ++ )
				{
					for( j = 0 ; j < HWidth ; j ++ )
					{
						RDest[ j * 2     ] = ( BYTE )( 
							FontSystem.BitCountTable[ RSrc[ j               ] & 0xf0 ] +
							FontSystem.BitCountTable[ RSrc[ j + ImagePitch  ] & 0xf0 ] +
							FontSystem.BitCountTable[ RSrc[ j + ImagePitch2 ] & 0xf0 ] +
							FontSystem.BitCountTable[ RSrc[ j + ImagePitch3 ] & 0xf0 ] ) ;

						RDest[ j * 2 + 1 ] = ( BYTE )( 
							FontSystem.BitCountTable[ RSrc[ j               ] & 0x0f ] +
							FontSystem.BitCountTable[ RSrc[ j + ImagePitch  ] & 0x0f ] +
							FontSystem.BitCountTable[ RSrc[ j + ImagePitch2 ] & 0x0f ] +
							FontSystem.BitCountTable[ RSrc[ j + ImagePitch3 ] & 0x0f ] ) ;
					}

					RSrc  += ImageAddPitch ;
					RDest += RSrcPitch ;
				}

				if( MHeight != 0 )
				{
					for( i = 0 ; i < MHeight ; i ++ )
					{
						for( j = 0 ; j < HWidth ; j ++ )
						{
							RDest[ j * 2     ] += FontSystem.BitCountTable[ RSrc[ j ] & 0xf0 ] ;
							RDest[ j * 2 + 1 ] += FontSystem.BitCountTable[ RSrc[ j ] & 0x0f ] ;
						}

						RSrc += ImagePitch ;
					}
				}
				break;

				// １６倍の場合
			case 16 :
				{
					DWORD MixParam ;

					ImageType = DX_FONT_SRCIMAGETYPE_8BIT_MAX255 ;
					for( i = 0 ; i < RHeight ; i ++ )
					{
						for( j = 0 ; j < RWidth ; j ++ )
						{
							MixParam = 
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0                ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1                ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch   ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch   ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch2  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch2  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch3  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch3  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch4  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch4  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch5  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch5  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch6  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch6  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch7  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch7  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch8  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch8  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch9  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch9  ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch10 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch10 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch11 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch11 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch12 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch12 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch13 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch13 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch14 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch14 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 0 + ImagePitch15 ] ] +
								FontSystem.BitCountTable[ RSrc[ j * 2 + 1 + ImagePitch15 ] ] ;
							RDest[ j ] = ( BYTE )( MixParam == 256 ? 255 : MixParam ) ;
						}

						RSrc  += ImageAddPitch ;
						RDest += RSrcPitch ;
					}

					if( MHeight != 0 )
					{
						for( i = 0 ; i < MHeight ; i ++ )
						{
							for( j = 0 ; j < RWidth ; j ++ )
							{
								int SrcTotal ;

								SrcTotal = FontSystem.BitCountTable[ RSrc[ j * 2 ] ] + FontSystem.BitCountTable[ RSrc[ j * 2 + 1 ] ] ;
								if( RDest[ j ] + SrcTotal >= 256 )
								{
									RDest[ j ] = 255 ;
								}
								else
								{
									RDest[ j ] += SrcTotal ;
								}
							}

							RSrc += ImagePitch ;
						}
					}
				}
				break ;
			}

			ImageBuffer = RDataBuffer ;
			ImagePitch  = RSrcPitch ;
		}

		{
			BYTE	*Src ;
			BYTE	*Dest ;
			BYTE	dat = 0 ;
			DWORD	Height ;
			DWORD	Width ;
			DWORD	i ;
			DWORD	j ;
			DWORD	DestPitch ;

			Width	= ManageData->CharaData.SizeX ;
			Height	= ManageData->CharaData.SizeY ;

			Src		= ( BYTE * )ImageBuffer ;

			DestPitch	= ( DWORD )ManageData->CachePitch ;
			Dest		= ( BYTE * )ManageData->CacheMem ;

			switch( ManageData->ImageBitDepth )
			{
			case DX_FONTIMAGE_BIT_1 :
				switch( ImageType )
				{
				case DX_FONT_SRCIMAGETYPE_1BIT :
					for( i = 0 ; i < Height ; i ++ )
					{
						memcpy( Dest, Src, ( size_t )ImagePitch ) ;

						Src  += ImagePitch ;
						Dest += DestPitch ;
					}
					break ;

				case DX_FONT_SRCIMAGETYPE_8BIT_ON_OFF :
					{
						BYTE *dp ;
						BYTE bit ;

						for( i = 0 ; i < Height ; i ++ )
						{
							dp  = Dest ;
							bit = 0x80 ;
							dat = 0 ;
							for( j = 0 ; j < Width ; j ++, bit >>= 1 )
							{
								if( j != 0 && j % 8 == 0 )
								{
									bit = 0x80 ;
									*dp = dat ;
									dp ++ ;
									dat = 0 ;
								}

								if( Src[ j ] != 0 )
								{
									dat |= bit ;
								}
							}
							*dp = dat ;

							Src  += ImagePitch ;
							Dest += DestPitch ;
						}
					}
					break ;

				default :
					return -1 ;
				}
				break ;

			case DX_FONTIMAGE_BIT_4 : 
				// DX_FONT_SRCIMAGETYPE_8BIT_MAX16 以外はエラー
				if( ImageType != DX_FONT_SRCIMAGETYPE_8BIT_MAX16 )
				{
					return -1 ;
				}

				for( i = 0 ; i < Height ; i ++ )
				{
					for( j = 0 ; j < Width ; j ++ )
					{
						if( Src[j] )
						{
							Dest[j] = ( BYTE )( Src[j] - 1 ) ;
						}
					}

					Src  += ImagePitch ;
					Dest += DestPitch ;
				}
				break ;

			case DX_FONTIMAGE_BIT_8 :
				// DX_FONT_SRCIMAGETYPE_8BIT_MAX255 以外はエラー
				if( ImageType != DX_FONT_SRCIMAGETYPE_8BIT_MAX255 )
				{
					return -1 ;
				}

				for( i = 0 ; i < Height ; i ++ )
				{
					memcpy( Dest, Src, Width ) ;

					Src  += ImagePitch ;
					Dest += DestPitch ;
				}
				break ;
			}
		}
	}

	// リサイズ処理用のメモリを確保していた場合は解放
	if( ResizeBuffer != NULL )
	{
		free( ResizeBuffer ) ;
		ResizeBuffer = NULL ;
	}

	// 終了
	return 0 ;
}

// FontCacheCharChangeの環境依存処理を行う関数( 実行箇所区別 0 )
extern int FontCacheCharChange_Timing0_Win( FONTMANAGE *ManageData )
{
	// デバイスコンテキストの作成
	FontSystem_Win.Devicecontext = CreateCompatibleDC( NULL ) ;
	if( FontSystem_Win.Devicecontext == NULL )
	{
		wprintf( L"テキストキャッシュサーフェスのデバイスコンテキストの取得に失敗しました\n" ) ;
		return -1 ;
	}

	// フォントをセット
	FontSystem_Win.OldFont = ( HFONT )SelectObject( FontSystem_Win.Devicecontext, ManageData->FontObj ) ;
	if( FontSystem_Win.OldFont == NULL )
	{
		DeleteDC( FontSystem_Win.Devicecontext ) ;
		wprintf( L"テキストキャッシュサーフェスのデバイスコンテキストの取得に失敗しました" ) ;
		return -1 ;
	}

	// フォントの情報を取得
	GetTextMetrics( FontSystem_Win.Devicecontext, &FontSystem_Win.TextMetric ) ;

	// TextOut を使用するかどうかで処理を分岐
	if( ManageData->UseTextOut )
	{
		// 描画先ビットマップをセット
		FontSystem_Win.OldBitmap = ( HBITMAP )SelectObject( FontSystem_Win.Devicecontext , ManageData->CacheBitmap ) ;

		// 文字の描画設定を行う
		{
			SetTextColor( FontSystem_Win.Devicecontext , RGB( 255 , 255 , 255 ) ) ; 		// 色をセット	

			// 背景色をセット
			SetBkColor( FontSystem_Win.Devicecontext , 0 ) ;
			SetBkMode( FontSystem_Win.Devicecontext , OPAQUE ) ;							// 背景を塗りつぶす指定
		}
	}

	// 正常終了
	return 0 ;
}

// FontCacheCharChangeの環境依存処理を行う関数( 実行箇所区別 1 )
extern int FontCacheCharChange_Timing1_Win( FONTMANAGE *ManageData, DWORD ChangeChar )
{
	int				ImageType = 0 ;
	int				Space ;

	// スペースかどうかを取得しておく
	Space = ( ChangeChar == L' ' || ChangeChar == L'　' ) ? 1 : 0 ;

	// TextOut を使用するかどうかで処理を分岐
	if( ManageData->UseTextOut )
	{
		SIZE	TempSize ;
		wchar_t AddStr[ 4 ] ;
		int     CharNum ;

		CharNum = PutCharCode( ChangeChar, CHARCODEFORMAT_UTF16LE, ( char * )AddStr ) / sizeof( wchar_t ) ;

		// 追加する文字の大きさを取得
		GetTextExtentPoint32W( FontSystem_Win.Devicecontext, AddStr, CharNum, &TempSize );

		// 文字イメージを出力
		TextOutW( FontSystem_Win.Devicecontext, 0, 0, AddStr, CharNum ) ;

		// イメージを転送
		FontCacheCharImageBlt(
			ManageData,
			ChangeChar, 
			FALSE,
			DX_FONT_SRCIMAGETYPE_8BIT_ON_OFF,
			ManageData->CacheBitmapMem,
			TempSize.cx,
			TempSize.cy,
			ManageData->CacheBitmapMemPitch,
			0,
			0,
			TempSize.cx
		) ;
	}
	else
	{
		DWORD			DataSize ;
		GLYPHMETRICS	gm ;
		GCP_RESULTSW	gcp ;
		wchar_t			gcpBuffer[ 2 ] ;
		MAT2			mt = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } } ;
		DWORD			SrcPitch ; 

		// 取得するイメージ形式を決定する
		switch( ManageData->ImageBitDepth )
		{
		case DX_FONTIMAGE_BIT_1 :
			ImageType	= DX_FONT_SRCIMAGETYPE_1BIT ;
			break ;

		case DX_FONTIMAGE_BIT_4 :
			ImageType	= DX_FONT_SRCIMAGETYPE_1BIT_SCALE4 ;
			break ;

		case DX_FONTIMAGE_BIT_8 :
			ImageType	= DX_FONT_SRCIMAGETYPE_1BIT_SCALE16 ;
			break ;
		}

		// サロゲートペアの場合は文字のグリフインデックスを取得する
		if( ChangeChar > 0xffff )
		{
			wchar_t CodeWString[ 4 ] ;
			int Bytes ;
			DWORD Result ;

			memset( &gcp, 0, sizeof( gcp ) ) ;
			gcp.lStructSize = sizeof( gcp ) ;
			gcp.lpGlyphs    = gcpBuffer ;
			gcp.nGlyphs     = 2 ;
			Bytes = PutCharCode( ChangeChar, CHARCODEFORMAT_UTF16LE, ( char * )CodeWString ) ;
			Result = GetCharacterPlacementW( FontSystem_Win.Devicecontext, CodeWString, Bytes / 2, 0, &gcp, GCP_GLYPHSHAPE ) ;
			if( Result == 0 )
			{
				wprintf( L"Win32API GetCharacterPlacement が失敗しました\n" ) ;
				return -1 ;
			}
		}

		// 文字情報の取得
		memset( &gm, 0, sizeof( GLYPHMETRICS ) ) ;
		if( ChangeChar > 0xffff )
		{
			DataSize = GetGlyphOutlineW( FontSystem_Win.Devicecontext, ( UINT )gcp.lpGlyphs[ 0 ], GGO_BITMAP | GGO_GLYPH_INDEX, &gm, 0, NULL, &mt ) ;
		}
		else
		{
			DataSize = GetGlyphOutlineW( FontSystem_Win.Devicecontext, ( UINT )ChangeChar, GGO_BITMAP, &gm, 0, NULL, &mt ) ;
		}
		if( DataSize == GDI_ERROR )
		{
			wprintf( L"Win32API GetGlyphOutline が失敗しました\n" ) ;
			return -1 ;
		}

		// スペース文字だった場合
		if( Space != 0 )
		{
			FontCacheCharImageBlt(
				ManageData,
				ChangeChar, 
				TRUE,
				ImageType,
				NULL,
				0,
				0,
				0,
				0,
				0,
				gm.gmCellIncX
			) ;
		}
		else
		// スペース以外でデータサイズが０の場合
		if( DataSize == 0 )
		{
			FontCacheCharImageBlt(
				ManageData,
				ChangeChar, 
				FALSE,
				DX_FONT_SRCIMAGETYPE_1BIT,
				NULL,
				0,
				0,
				0,
				0,
				0,
				gm.gmCellIncX
			) ;
		}
		else
		{
			BYTE *DataBuffer ;
			DWORD BufferSize ;
			int Height ;

			SrcPitch = ( ( gm.gmBlackBoxX + 7 ) / 8 + 3 ) / 4 * 4 ;
			Height = DataSize / SrcPitch ;
			BufferSize = DataSize + SrcPitch * ( 2 + Height ) ;

			if( ManageData->GetGlyphOutlineBufferSize < BufferSize )
			{
				ManageData->GetGlyphOutlineBufferSize = BufferSize ;

				if( ManageData->GetGlyphOutlineBuffer != NULL )
				{
					free( ManageData->GetGlyphOutlineBuffer ) ;
					ManageData->GetGlyphOutlineBuffer = NULL ;
				}

				ManageData->GetGlyphOutlineBuffer = malloc( ManageData->GetGlyphOutlineBufferSize ) ;
				if( ManageData->GetGlyphOutlineBuffer == NULL )
				{
					wprintf( L"アンチエイリアス文字取得用バッファの確保に失敗しました\n" ) ;
					return -1 ;
				}
			}

			memset( ManageData->GetGlyphOutlineBuffer, 0, BufferSize ) ;

			DataBuffer	= ( BYTE * )ManageData->GetGlyphOutlineBuffer + SrcPitch * ( 1 + Height ) ;
			if( ChangeChar > 0xffff )
			{
				DataSize	= GetGlyphOutlineW( FontSystem_Win.Devicecontext, ( UINT )gcp.lpGlyphs[ 0 ], GGO_BITMAP | GGO_GLYPH_INDEX, &gm, DataSize, ( LPVOID )DataBuffer, &mt ) ;
			}
			else
			{
				DataSize	= GetGlyphOutlineW( FontSystem_Win.Devicecontext, ( UINT )ChangeChar, GGO_BITMAP, &gm, DataSize, ( LPVOID )DataBuffer, &mt ) ;
			}
			if( DataSize == GDI_ERROR )
			{
				wprintf( L"Win32API GetGlyphOutline が失敗しました\n" ) ;
				return -1 ;
			}

			// イメージを転送
			FontCacheCharImageBlt(
				ManageData,
				ChangeChar, 
				FALSE,
				ImageType,
				DataBuffer,
				gm.gmBlackBoxX,
				Height,
				SrcPitch,
				gm.gmptGlyphOrigin.x ,
				FontSystem_Win.TextMetric.tmAscent - gm.gmptGlyphOrigin.y,
				gm.gmCellIncX
			) ;
		}
	}

	// 正常終了
	return 0 ;
}

// FontCacheCharChangeの環境依存処理を行う関数( 実行箇所区別 2 )
extern int FontCacheCharChange_Timing2_Win( FONTMANAGE *ManageData )
{
	// フォントを元に戻す
	SelectObject( FontSystem_Win.Devicecontext, FontSystem_Win.OldFont ) ;

	if( ManageData->UseTextOut )
	{
		// ビットマップを元に戻す
		SelectObject( FontSystem_Win.Devicecontext, FontSystem_Win.OldBitmap ) ;
		FontSystem_Win.OldBitmap = NULL ;
	}

	// デバイスコンテキストの削除
	DeleteDC( FontSystem_Win.Devicecontext ) ;

	// 終了
	return 0 ;
}

// 文字キャッシュの文字を変更する
extern int FontCacheCharChange( FONTMANAGE *ManageData, DWORD CharCode )
{
	// Windows 環境依存処理０
	if( FontCacheCharChange_Timing0_Win( ManageData ) < 0 )
	{
		return -1 ;
	}

	// Windows 環境依存処理１
	if( FontCacheCharChange_Timing1_Win( ManageData, CharCode ) < 0 )
	{
		return -1 ;
	}

	// Windows 環境依存処理２
	if( FontCacheCharChange_Timing2_Win( ManageData ) < 0 )
	{
		return NULL ;
	}

	// 終了
	return 0 ;
}

// フォントデータファイルの作成
extern int CreateFontDataFile(
	const wchar_t *	SaveFilePath,
	const wchar_t *	FontName,
	int				Size,
	int				ImageBitDepth /* DX_FONTIMAGE_BIT_1等 */ , 
	int				Thick,
	int				Italic,
	int				CharSet,
	int				CharCodeFormat,
	const wchar_t *	SaveCharaList
)
{
	FONTMANAGE				*ManageData ;
	FONTDATAFILEHEADER		*FontFileHeader ;
	FONTDATAFILECHARADATA	*FontCharaData ;
	BYTE					*FileHeaderBuffer = NULL ;
	DWORD					 FileHeaderBufferSize ;
	BYTE					*FilePressHeaderBuffer = NULL ;
	DWORD					 FilePressHeaderSize ;
	BYTE					*FileImageBuffer = NULL ;
	DWORD					 FileImageBufferSize ;
	DWORD					 FileImageBufferAddress ;
	BYTE					*FontTempImageBuffer = NULL ;
	DWORD					 FontTempImageBufferSize = 0 ;
	DWORD					 FontTempImageSize ;
	DWORD					 FontImagePressSize ;
//	static DWORD			 CharaList[ 0x110000 ] ;
	static DWORD			 CharaList[ 0x20000 ] ;
	DWORD					 CharaIndex ;
	DWORD					 CharaNum ;
	DWORD					 CharaImageNum ;
	const int				 CacheCharNum = 16 ;

	// フォントデータを作成
	ManageData = CreateFontManageData( FontName, Size, ImageBitDepth, Thick, Italic, CharSet, CharCodeFormat ) ;
	if( ManageData == NULL )
	{
		wprintf( L"フォントデータの作成に失敗しました\n" ) ;
		return -1 ;
	}
	ImageBitDepth = ManageData->ImageBitDepth ;

	// SaveCharaList が NULL の場合は、全ての文字を変換の対象にする
	CharaNum = 0 ;
	if( SaveCharaList == NULL )
	{
		int i ;

//		for( i = 1 ; i < 0x110000 ; i ++ )
		for( i = 1 ; i < 0x10000 ; i ++ )
		{
			CharaList[ CharaNum ] = i ;
			CharaNum ++ ;
		}
	}
	else
	{
		int Bytes ;
		while( *SaveCharaList != L'\0' )
		{
			int i ;

			int CharaCode = GetCharCode( ( const char * )SaveCharaList, CHARCODEFORMAT_UTF16LE, &Bytes ) ;
			for( i = 0 ; i < CharaNum && CharaList[ i ] != CharaCode ; i ++ ){}
			if( i == CharaNum )
			{
				CharaList[ CharaNum ] = GetCharCode( ( const char * )SaveCharaList, CHARCODEFORMAT_UTF16LE, &Bytes ) ;
				CharaNum ++ ;
			}

			SaveCharaList += Bytes / sizeof( wchar_t ) ;
		}
	}

	// ヘッダバッファを確保
	FileHeaderBufferSize = sizeof( FONTDATAFILEHEADER ) + CharaNum * sizeof( FONTDATAFILECHARADATA ) + ManageData->BaseInfo.KerningPairNum * sizeof( FONTDATAFILEKERNINGPAIRDATA ) ;
	FileHeaderBuffer = ( BYTE * )malloc( FileHeaderBufferSize * 3 ) ;
	if( FileHeaderBuffer == NULL )
	{
		wprintf( L"フォントデータファイルヘッダ情報一時記憶用メモリの確保に失敗しました\n" ) ;
		return -1 ;
	}
	memset( FileHeaderBuffer, 0, FileHeaderBufferSize * 3 ) ;
	FilePressHeaderBuffer = FileHeaderBuffer + FileHeaderBufferSize ;

	// イメージバッファを確保
	FileImageBufferSize = 1024 * 1024 * 256 ; 
	FileImageBuffer = ( BYTE * )malloc( FileImageBufferSize ) ;
	if( FileImageBuffer == NULL )
	{
		wprintf( L"フォントデータファイルヘッダ情報一時記憶用メモリの確保に失敗しました\n" ) ;
		return -1 ;
	}
	memset( FileImageBuffer, 0, FileImageBufferSize ) ;

	FontFileHeader = ( FONTDATAFILEHEADER * )FileHeaderBuffer ;
	FontFileHeader->Magic[ 0 ]           = 'F' ;
	FontFileHeader->Magic[ 1 ]           = 'N' ;
	FontFileHeader->Magic[ 2 ]           = 'T' ;
	FontFileHeader->Magic[ 3 ]           = 'F' ;
	FontFileHeader->Version              = DX_FONTDATAFILE_VERSION ;
	FontFileHeader->Press.BaseInfo      = ManageData->BaseInfo ;
	FontFileHeader->Press.ImageBitDepth = ( BYTE )ImageBitDepth ;

	CL_strcpy( CHARCODEFORMAT_UTF16LE, ( char * )FontFileHeader->Press.FontName, ( char * )ManageData->FontName ) ;

	FontCharaData = ( FONTDATAFILECHARADATA * )( FontFileHeader + 1 ) ;
	FileImageBufferAddress = 0 ;

	// カーニングペア情報を保存
	if( ManageData->BaseInfo.KerningPairNum > 0 )
	{
		memcpy(
			( FONTDATAFILEKERNINGPAIRDATA * )( FontCharaData + CharaNum ),
			ManageData->KerningPairData,
			sizeof( FONTDATAFILEKERNINGPAIRDATA ) * ManageData->BaseInfo.KerningPairNum
		) ;
	}

	// Windows 環境依存処理０
	if( FontCacheCharChange_Timing0_Win( ManageData ) < 0 )
	{
		return -1 ;
	}

	DWORD DispTime ;
	DWORD NowTime ;

	DispTime = timeGetTime() ;
	wprintf( L"\t%7d / %7d   Save Num %7d   Image Num %7d", 0, CharaNum, 0 ) ;

	CharaImageNum = 0 ;
	for( CharaIndex = 0 ; CharaIndex < CharaNum ; CharaIndex ++ )
	{
		if( CharaIndex % 100 == 0 || CharaIndex == CharaNum - 1 )
		{
			NowTime = timeGetTime() ;
			if( CharaIndex % 100 == 0 || CharaIndex == CharaNum - 1 || NowTime - DispTime > 17 )
			{
				DispTime = NowTime ;
				wprintf( L"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%7d / %7d   Save Num %7d   Image Num %7d", CharaIndex + 1, CharaNum, FontFileHeader->CharaNum, CharaImageNum ) ;
			}
		}

		if( FontCacheCharChange_Timing1_Win( ManageData, CharaList[ CharaIndex ] ) < 0 )
		{
			continue ;
		}

		// 画像サイズが０の場合は追加しない
		if( ManageData->CharaData.SizeX == 0 &&
			ManageData->CharaData.SizeY == 0 &&
			ManageData->CharaData.AddX  == 0 )
		{
			continue ;
		}

		// フォントの情報をセット

		FontCharaData->CodeUnicode   = CharaList[ CharaIndex ] ;
		FontCharaData->DrawX         = ManageData->CharaData.DrawX ;
		FontCharaData->DrawY         = ManageData->CharaData.DrawY ;
		FontCharaData->AddX          = ManageData->CharaData.AddX ;
		FontCharaData->SizeX         = ManageData->CharaData.SizeX ;
		FontCharaData->SizeY         = ManageData->CharaData.SizeY ;
		FontCharaData->ImageAddress  = FileImageBufferAddress ;

		// イメージデータの構築
		{
			DWORD		 DestPitch ;
			DWORD		 i ;
			DWORD		 j ;
			BYTE		*Dest ;
			BYTE		*Src ;
			DWORD		 SrcAddPitch ;
			BYTE		 DestData ;

			switch( ManageData->ImageBitDepth )
			{
			case DX_FONTIMAGE_BIT_1 :
				DestPitch   = ( DWORD )( ( ManageData->CharaData.SizeX + 7 ) / 8 ) ;
				SrcAddPitch = ( DWORD )ManageData->CachePitch ;
				break ;

			case DX_FONTIMAGE_BIT_4 :
				DestPitch   = ( DWORD )( ( ManageData->CharaData.SizeX + 1 ) / 2 ) ;
				SrcAddPitch = ( DWORD )( ManageData->CachePitch - ManageData->CharaData.SizeX ) ;
				break ;

			case DX_FONTIMAGE_BIT_8 :
				DestPitch   = ManageData->CharaData.SizeX ;
				SrcAddPitch = ( DWORD )ManageData->CachePitch ;
				break ;
			}

			FontCharaData->ImagePitch = DestPitch ;

			FontTempImageSize = DestPitch * ManageData->CharaData.SizeY ;
			if( FontTempImageBufferSize < FontTempImageSize )
			{
				FontTempImageBufferSize = FontTempImageSize + 1024 * 32 ;
				FontTempImageBuffer = ( BYTE * )realloc( FontTempImageBuffer, FontTempImageBufferSize ) ;
				if( FontTempImageBuffer == NULL )
				{
					wprintf( L"\nフォントデータ画像一時記憶用メモリの確保に失敗しました\n" ) ;
					goto ERR ;
				}
			}

			if( FontTempImageSize > FontFileHeader->MaxImageBytes )
			{
				FontFileHeader->MaxImageBytes = FontTempImageSize ;
			}

			Dest = FontTempImageBuffer ;
			Src  = ( BYTE * )ManageData->CacheMem ;
			switch( ManageData->ImageBitDepth )
			{
			case DX_FONTIMAGE_BIT_1 :
				for( i = 0 ; i < ManageData->CharaData.SizeY ; i ++, Src += SrcAddPitch )
				{
					memcpy( Dest, Src, DestPitch ) ;
					Dest += DestPitch ;
				}
				break ;

			case DX_FONTIMAGE_BIT_4 :
				for( i = 0 ; i < ManageData->CharaData.SizeY ; i ++, Src += SrcAddPitch )
				{
					for( j = 0 ; j < ManageData->CharaData.SizeX ; j += 2 )
					{
						DestData = ( ( BYTE )( *Src * 15 / 16 ) << 4 ) ;
						Src ++ ;
						if( ManageData->CharaData.SizeX == j + 1 )
						{
							*Dest = DestData ;
							Dest ++ ;
							continue ;
						}

						DestData |= ( BYTE )( *Src * 15 / 16 ) ;
						Src ++ ;

						*Dest = DestData ;
						Dest ++ ;
					}
				}
				break ;

			case DX_FONTIMAGE_BIT_8 :
				for( i = 0 ; i < ManageData->CharaData.SizeY ; i ++, Src += SrcAddPitch )
				{
					memcpy( Dest, Src, DestPitch ) ;
					Dest += DestPitch ;
				}
				break ;
			}

			// イメージを圧縮する
			if( FileImageBufferSize < FileImageBufferAddress + FontTempImageSize * 2 )
			{
				FileImageBufferSize += FontTempImageSize * 2 + 1024 * 1024 * 32 ;
				FileImageBuffer = ( BYTE * )realloc( FileImageBuffer, FileImageBufferSize ) ;
				if( FileImageBuffer == NULL )
				{
					wprintf( L"\nフォントデータファイルヘッダ情報一時記憶用メモリの拡張に失敗しました\n" ) ;
					goto ERR ;
				}
			}
			FontImagePressSize = ( DWORD )DXArchive::Encode( FontTempImageBuffer, FontTempImageSize, FileImageBuffer + FileImageBufferAddress, false ) ;

			if( FontImagePressSize >= FontTempImageSize )
			{
				FontCharaData->Press = 0 ;

				memcpy( FileImageBuffer + FileImageBufferAddress, FontTempImageBuffer, FontTempImageSize ) ;
			}
			else
			{
				FontCharaData->Press = 1 ;
			}

			// 既に登録している文字とまったく同じ画像があるか調べる
			{
				FONTDATAFILECHARADATA	*FontCharaDataSub ;

				FontCharaDataSub = ( FONTDATAFILECHARADATA * )( FontFileHeader + 1 ) ;
				for( i = 0 ; i < FontFileHeader->CharaNum ; i ++, FontCharaDataSub ++ )
				{
					if( FontCharaDataSub->Press != FontCharaData->Press ||
						FontCharaDataSub->DrawX != FontCharaData->DrawX ||
						FontCharaDataSub->DrawY != FontCharaData->DrawY ||
						FontCharaDataSub->AddX  != FontCharaData->AddX ||
						FontCharaDataSub->SizeX != FontCharaData->SizeX ||
						FontCharaDataSub->SizeY != FontCharaData->SizeY )
					{
						continue ;
					}

					if( FontCharaData->Press )
					{
						int DecodeSize ;

						DecodeSize = DXArchive::Decode( FileImageBuffer + FontCharaDataSub->ImageAddress, NULL ) ;
						if( DecodeSize != FontImagePressSize )
						{
							continue ;
						}
						if( memcmp( FileImageBuffer + FontCharaDataSub->ImageAddress, FileImageBuffer + FontCharaData->ImageAddress, FontImagePressSize ) == 0 )
						{
							break ;
						}
					}
					else
					{
						if( memcmp( FileImageBuffer + FontCharaDataSub->ImageAddress, FileImageBuffer + FontCharaData->ImageAddress, FontTempImageSize ) == 0 )
						{
							break ;
						}
					}
				}

				// 全く同じ画像があった場合は画像を共有する
				if( i != FontFileHeader->CharaNum )
				{
					FontCharaData->ImageAddress = FontCharaDataSub->ImageAddress ;
				}
				else
				{
					CharaImageNum ++ ;

					// それ以外の場合は画像のアドレスを進める
					if( FontCharaData->Press )
					{
						FileImageBufferAddress += FontImagePressSize ;
					}
					else
					{
						FileImageBufferAddress += FontTempImageSize ;
					}
				}
			}
		}

		FontCharaData ++ ;
		FontFileHeader->CharaNum ++ ;

		if( CharaList[ CharaIndex ] >= 0x10000 )
		{
			FontFileHeader->CharaExNum ++ ;
		}
	}

	// Windows 環境依存処理２
	if( FontCacheCharChange_Timing2_Win( ManageData ) < 0 )
	{
		return -1 ;
	}

	// ファイルヘッダ部分のメンバー変数 FontName 以降の部分の圧縮
	{
		int NotPressSize ;

		NotPressSize = sizeof( FONTDATAFILEHEADER ) - sizeof( FONTDATAFILEPRESSHEADER ) ;

		FilePressHeaderSize = DXArchive::Encode(
			FileHeaderBuffer + NotPressSize,
			( DWORD )( ( BYTE * )FontCharaData - FileHeaderBuffer ) - NotPressSize +
			sizeof( FONTDATAFILEKERNINGPAIRDATA ) * ManageData->BaseInfo.KerningPairNum,
			FilePressHeaderBuffer + NotPressSize, false ) + NotPressSize ;

		// 画像データ開始アドレスをセット
		FontFileHeader->ImageAddress = FilePressHeaderSize ;

		// 圧縮しない部分をコピー
		memcpy( FilePressHeaderBuffer, FileHeaderBuffer, NotPressSize ) ;
	}

	// ファイルに保存
	{
		HANDLE FileHandle ;
		DWORD WriteSize ;

		// 既にファイルがあった場合用に、ファイル削除処理
		DeleteFileW( SaveFilePath ) ;

		// ファイルを開く
		FileHandle = CreateFileW( SaveFilePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;
		if( FileHandle == NULL ) 
		{
			wprintf( L"\nフォントデータファイルのオープンに失敗しました\n" ) ;
			goto ERR ;
		}

		// ファイルヘッダの書き出し
		WriteFile( FileHandle, FilePressHeaderBuffer, FilePressHeaderSize, &WriteSize, NULL ) ;
		
		// イメージの書き出し
		WriteFile( FileHandle, FileImageBuffer, FileImageBufferAddress, &WriteSize, NULL ) ;

		// ファイルを閉じる
		CloseHandle( FileHandle ) ;
	}

	wprintf( L"\n" ) ;
	wprintf( L"ファイル作成完了\n" ) ;
	wprintf( L"保存した文字の数 %d  保存した文字画像の数 %d\n", FontFileHeader->CharaNum, CharaImageNum ) ;

	// 確保していたメモリを解放
	if( FontTempImageBuffer != NULL )
	{
		free( FontTempImageBuffer ) ;
		FontTempImageBuffer = NULL ;
	}

	if( FileHeaderBuffer != NULL )
	{
		free( FileHeaderBuffer ) ;
		FileHeaderBuffer = NULL ;
	}

	if( FileImageBuffer != NULL )
	{
		free( FileImageBuffer ) ;
		FileImageBuffer = NULL ;
	}

	// フォントデータ作成処理用に作成したフォントハンドルを削除
	DeleteFontManageData( ManageData ) ;

	// 正常終了
	return 0 ;

	// エラー処理
ERR :
	if( FontTempImageBuffer != NULL )
	{
		free( FontTempImageBuffer ) ;
		FontTempImageBuffer = NULL ;
	}

	if( FileHeaderBuffer != NULL )
	{
		free( FileHeaderBuffer ) ;
		FileHeaderBuffer = NULL ;
	}

	if( FileImageBuffer != NULL )
	{
		free( FileImageBuffer ) ;
		FileImageBuffer = NULL ;
	}

	if( ManageData != NULL )
	{
		DeleteFontManageData( ManageData ) ;
	}

	return -1 ;
}

