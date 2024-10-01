#include "Convert_Manager.h"



CConvert_Manager::CConvert_Manager()
{
}

HRESULT CConvert_Manager::Initialize()
{

	return S_OK;
}

void CConvert_Manager::Get_FileNames(const wstring& strPath, vector<wstring>& vecOut)
{
	vecOut.clear();

	vector<wstring> vecDirectoryNames;

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		wstring strFileName = entry.path().filename();

		vecDirectoryNames.emplace_back(strFileName);
	}

	vecOut = vecDirectoryNames;
}

void CConvert_Manager::Get_FileNames(const string& strPath, vector<string>& vecOut)
{
	vecOut.clear();

	vector<string> vecDirectoryNames;

	for (const fs::directory_entry& entry :
			fs::directory_iterator(strPath))
	{
		string strFileName = entry.path().filename().string();

		vecDirectoryNames.emplace_back(strFileName);
	}

	vecOut = vecDirectoryNames;
}



wstring CConvert_Manager::Get_FileName(const wstring& strPath)
{
	fs::path p(strPath);
	return p.stem();
}

string CConvert_Manager::Get_FileName(const string& strPath)
{
	fs::path p(strPath);
	return p.stem().string();
}

void CConvert_Manager::Get_FileExts(const wstring& strPath, vector<wstring>& vecOut)
{
	vecOut.clear();

	vector<wstring> vecDirectoryNames;

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		wstring strFileName = entry.path().extension();

		vecDirectoryNames.emplace_back(strFileName);
	}

	vecOut = vecDirectoryNames;
}

void CConvert_Manager::Get_FileExts(const string& strPath, vector<string>& vecOut)
{
	vecOut.clear();

	vector<string> vecDirectoryNames;

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		string strFileName = entry.path().extension().string();

		vecDirectoryNames.emplace_back(strFileName);
	}

	vecOut = vecDirectoryNames;
}

wstring CConvert_Manager::Get_FileExt(const wstring& strPath)
{
	fs::path p(strPath);
	return p.extension();
}

string CConvert_Manager::Get_FileExt(const string& strPath)
{
	fs::path p(strPath);
	return p.extension().string();
}

void CConvert_Manager::Get_DirectoryName(const wstring& strPath, vector<wstring>& vecOut)
{
	vecOut.clear();

	vector<wstring> vecDirectoryNames;

	//_tchar			szTextureFilePath[MAX_PATH] = TEXT("");
	//_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		wstring strDirPath = entry.path().wstring();

		strDirPath.erase(0, strPath.size());

		vecDirectoryNames.emplace_back(strDirPath);
	}

	vecOut = vecDirectoryNames;
}

void CConvert_Manager::Get_DirectoryName(const string& strPath, vector<string>& vecOut)
{
	vecOut.clear();

	vector<string> vecDirectoryNames;

	//_tchar			szTextureFilePath[MAX_PATH] = TEXT("");
	//_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		string strDirPath = entry.path().string();

		strDirPath.erase(0, strPath.size());

		vecDirectoryNames.emplace_back(strDirPath);
	}

	vecOut = vecDirectoryNames;
}

wstring CConvert_Manager::Get_Directory(const wstring& strPath)
{
	fs::path p(strPath);
	return p.parent_path();
}

string CConvert_Manager::Get_Directory(const string& strPath)
{
	fs::path p(strPath);
	return p.parent_path().string();
}

_uint CConvert_Manager::Get_NumFolders(const wstring& strPath)
{
	_uint iNum = { 0 };

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		//디렉토리일 때에만 증가
		if (fs::is_directory(entry))
			iNum++;
	}

	return iNum;
}

_uint CConvert_Manager::Get_NumFolders(const string& strPath)
{
	_uint iNum = { 0 };

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		//디렉토리일 때에만 증가
		if (fs::is_directory(entry))
			iNum++;
	}

	return iNum;
}

_uint CConvert_Manager::Get_NumFiles(const wstring& strPath)
{
	_uint iNum = { 0 };

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		//파일일 때에만 증가
		if (fs::is_regular_file(entry))
			iNum++;
	}

	return iNum;
}

_uint CConvert_Manager::Get_NumFiles(const string& strPath)
{
	_uint iNum = { 0 };

	for (const fs::directory_entry& entry :
		fs::directory_iterator(strPath))
	{
		//파일일 때에만 증가
		if (fs::is_regular_file(entry))
			iNum++;
	}

	return iNum;
}

_bool CConvert_Manager::IsExists_Path(const wstring& strPath)
{
	fs::path p(strPath);
	return exists(p);
}

_bool CConvert_Manager::IsExists_Path(const string& strPath)
{
	fs::path p(strPath);
	return exists(p);
}

wstring CConvert_Manager::StringToWstring(const string& str)
{
	//wstring strWidePath;
	//strWidePath.assign(str.begin(), str.end());

	//return strWidePath;

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
	std::wstring wstr(size_needed, 0);

	// 실제 변환
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

string CConvert_Manager::WstringToString(const wstring& str)
{
	string narrow_text;

	// Convert to UTF-8 encoding (common choice)
	int num_chars_converted = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	narrow_text.assign(num_chars_converted - 1, '\0'); // Allocate enough space and null terminate
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &narrow_text[0], num_chars_converted, nullptr, nullptr);

	return narrow_text;
}

string CConvert_Manager::Extract_String(const string& str, char cHead, char cTail)
{
	string extracted_str;
	bool extracting = false;

	for (char c : str) {
		if (c == cHead) {
			extracting = true;
		}
		else if (c == cTail && extracting) {
			break;
		}
		else if (extracting) {
			extracted_str += c;
		}
	}

	return extracted_str;
}

CConvert_Manager* CConvert_Manager::Create()
{
	CConvert_Manager* pInstance = new CConvert_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CConvert_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConvert_Manager::Free()
{
}
