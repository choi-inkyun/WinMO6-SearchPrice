#pragma once


class OperaBrowserFinder
{
public:
	static void GetRealPathName( const TCHAR *path, TCHAR *exePath )
	{
		int len = lstrlen( path );
		int start = 0;

		if( '\"' == path[0] )
			++start;

		TCHAR *exePtr = exePath;

		BOOL bCopy = FALSE;
		for( int i = start; i < len; i++ )
		{
			if( '\"' == path[i] )
			{
				if( bCopy )
				{
					*exePtr = 0;
					break;
				}
				else
				{
					bCopy = TRUE;
				}
			}
			else
			{
				if( ' ' == path[i] )
				{
					*exePtr = 0;
					break;
				}

				*exePtr = path[i];
				exePtr++;
			}
		}
	}

	static BOOL CheckOperaRegistry(TCHAR *path)
	{
		TCHAR tmpPath[MAX_PATH] = {0,};
		HKEY regKey;
		if( ERROR_SUCCESS != ::RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("_htmlfile_Opera\\Shell\\Open\\Command"), 
			0, KEY_ALL_ACCESS, &regKey ) )
		{
			return FALSE;
		}

		DWORD size = MAX_PATH * 2;
		DWORD lpType = REG_MULTI_SZ;
		int ret = RegQueryValueEx( regKey, NULL, NULL, &lpType, (BYTE *)tmpPath, &size);
		::RegCloseKey(regKey);

		GetRealPathName( tmpPath, path );
		return ret == ERROR_SUCCESS;
	}

	static BOOL CheckOperaFilePath(TCHAR *path)
	{
		const TCHAR *operaPath = _T("\\Windows\\OperaL.exe");
		lstrcpy( path, operaPath );
		return TRUE == CUtil::IsFileExist( operaPath );
	}

	static BOOL GetDefaultBrower(TCHAR *path)
	{
		TCHAR tmpPath[MAX_PATH] = {0,};
		HKEY regKey;
		if( ERROR_SUCCESS != ::RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("http\\Shell\\Open\\Command"), 
			0, KEY_ALL_ACCESS, &regKey ) )
		{
			return FALSE;
		}

		DWORD size = MAX_PATH * 2;
		DWORD lpType = REG_MULTI_SZ;
		int ret = RegQueryValueEx( regKey, NULL, NULL, &lpType, (BYTE *)tmpPath, &size);
		::RegCloseKey(regKey);

		GetRealPathName( tmpPath, path );
		return ret == ERROR_SUCCESS;		
	}
};
