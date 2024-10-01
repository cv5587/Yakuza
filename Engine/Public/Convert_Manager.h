#pragma once
#include "Base.h"

BEGIN(Engine)
class CConvert_Manager :
    public CBase
{
private:
    CConvert_Manager();
    virtual ~CConvert_Manager() = default;

public:
    HRESULT Initialize();

public:
	void Get_FileNames(const wstring& strPath, vector<wstring>& vecOut);
	void Get_FileNames(const string& strPath, vector<string>& vecOut);
	wstring Get_FileName(const wstring& strPath);
	string Get_FileName(const string& strPath);

	void Get_FileExts(const wstring& strPath, vector<wstring>& vecOut);
	void Get_FileExts(const string& strPath, vector<string>& vecOut);
	wstring Get_FileExt(const wstring& strPath);
	string Get_FileExt(const string& strPath);

	void Get_DirectoryName(const wstring& strPath, vector<wstring>& vecOut);
	void Get_DirectoryName(const string& strPath, vector<string>& vecOut);

	wstring Get_Directory(const wstring& strPath);
	string Get_Directory(const string& strPath);

	_uint Get_NumFolders(const wstring& strPath);		//폴더경로입력
	_uint Get_NumFolders(const string& strPath);		//폴더경로입력
	_uint Get_NumFiles(const wstring& strPath);			//폴더경로입력
	_uint Get_NumFiles(const string& strPath);			//폴더경로입력

	_bool IsExists_Path(const wstring& strPath);		//폴더경로입력
	_bool IsExists_Path(const string& strPath);			//폴더경로입력

	wstring StringToWstring(const string& str);
	string WstringToString(const wstring& str);

	string Extract_String(const string& str, char cHead, char cTail);

public:
    static CConvert_Manager* Create();
    virtual void Free() override;
};
END
