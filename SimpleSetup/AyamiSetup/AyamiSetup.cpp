// AyamiSetup.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Resource.h"
#include <iostream>
#include <string>
#include <shellapi.h>
#include <mmsystem.h>
#include <Digitalv.h>

#pragma comment(lib,"Winmm.lib")

const WCHAR* TopTitle = L"【靴下汉化组】放学后的灰姑娘 简体中文化补丁v1.0 安装程序";

const int FileCount = 5;
struct FileListStruct {
	std::wstring fnm;
	DWORD ID;
};
FileListStruct FileList[FileCount] = {
	{L"Cinderella_cn.cpk", IDR_CPK},
	{L"Cinderella_cn.dll", IDR_DLL}, 
	{L"Cinderella_cn.exe", IDR_EXE}, 
	{L"Cinderella_cn.key", IDR_KEY}, 
	{L"汉化说明.txt", IDR_TXT}
};

HINSTANCE g_hInstance = NULL;

BOOL InstallCheck()
{
	FILE* fp = nullptr;
	_wfopen_s(&fp, L"BGI.hvl", L"rb");
	if (!fp)
	{
		MessageBox(0, L"请将安装程序放到游戏目录下运行。", L"安装检查", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

void Install(HWND hw)
{
	if (!InstallCheck())
		return;
	FILE* fp = nullptr;
	HRSRC hResSrc = NULL;
	HGLOBAL hRes = NULL;
	BYTE* ResBuffer = NULL;
	DWORD ResSize = 0;

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	WCHAR szApp[] = L"notepad.exe 汉化说明.txt";

	for (int i = 0; i < FileCount; i++)
	{
		std::wstring FileName = FileList[i].fnm;
		std::wstring TitleName = L"[" + std::to_wstring(i + 1) + L"/" + std::to_wstring(FileCount) + L"] " + FileName + L"...";
		SetWindowTextW(hw, TitleName.c_str());

		// TODO: WriteFile
		hResSrc = FindResource(g_hInstance, MAKEINTRESOURCEW(FileList[i].ID), L"RES");
		if (!hResSrc)
		{
			WCHAR err_msg[MAX_PATH] = { 0 };
			wsprintfW(err_msg, L"内部错误。\n错误码：%d", GetLastError());
			MessageBox(0, err_msg, L"安装错误", MB_OK | MB_ICONERROR);
			goto back;
		}
		hRes = LoadResource(NULL, hResSrc);
		if (!hRes)
		{
			WCHAR err_msg[MAX_PATH] = { 0 };
			wsprintfW(err_msg, L"内部错误。\n错误码：%d", GetLastError());
			MessageBox(0, err_msg, L"安装错误", MB_OK | MB_ICONERROR);
			goto back;
		}
		ResBuffer = (BYTE*)LockResource(hRes);
		ResSize = SizeofResource(NULL, hResSrc);

		_wfopen_s(&fp, FileName.c_str(), L"wb");
		if (!fp)
		{
			MessageBox(0, L"写入文件失败。", L"安装错误", MB_OK | MB_ICONERROR);
			goto back;
		}
		fwrite(ResBuffer, ResSize, 1, fp);
		fclose(fp);

		FreeResource(hRes);
	}
	MessageBoxW(hw, L"安装完成", L"提示", MB_ICONINFORMATION);


	if (CreateProcess(NULL, szApp, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

back:
	SetWindowTextW(hw, TopTitle);
}

BOOL OnDlgCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	switch (LOWORD(wParam))
	{
	case IDC_BUTTON1:
		Install(hWnd);
		break;
	default:
		break;
	}
	bRet = TRUE;
	return bRet;
}

HRSRC hBgmSrc = NULL;
HGLOBAL hBgm = NULL;
void PlayBGM()
{
	BYTE* ResBuffer = NULL;
	DWORD ResSize = 0;

	hBgmSrc = FindResource(g_hInstance, MAKEINTRESOURCEW(IDR_BGM), L"WAVE");
	if (!hBgmSrc)
	{
		return;
	}
	hBgm = LoadResource(NULL, hBgmSrc);
	if (!hBgm)
	{
		return;
	}
	ResBuffer = (BYTE*)LockResource(hBgm);

	PlaySound((LPCWSTR)ResBuffer, NULL, SND_MEMORY | SND_ASYNC | SND_LOOP);
}

void StopBGM()
{
	PlaySoundA(0, 0, SND_FILENAME);

	if(hBgm)
		FreeResource(hBgm);
}

VOID OnInitDialog(HWND hWnd)
{
	SetWindowTextW(hWnd, TopTitle);

	HICON hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1));
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	PlayBGM();
}

void OnCloseDialog(HWND hw)
{
	StopBGM();

	EndDialog(hw, 0);
}

LRESULT CALLBACK DialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	switch (nMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		break;
	case WM_COMMAND:
		bRet = OnDlgCommand(hWnd, wParam, lParam);
		break;
	case WM_CLOSE:
		OnCloseDialog(hWnd);
		break;
	default:
		break;
	}
	return bRet;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	g_hInstance = hInstance;
    DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_PROPPAGE_MEDIUM), NULL, (DLGPROC)DialogProc, 0);

    return 0;
}
