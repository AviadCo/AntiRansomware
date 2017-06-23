// AntiRansomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment(lib, "comctl32.lib")

/* includes */
#include <fstream>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#include "HoneypotsManager.h"
#include "HoneypotNameGenerator.h"
#include "Honeypot.h"
#include "FileSystemHelper.h"
#include "ProcessesMonitor.h"
#include "FunctionCalledHandlerWrapper.h"
#include "ProcessHookMonitorWrapper.h"

#include "resource\resource.h"

/* using namespaces */
using std::wstring;
using std::list;
using std::wcout;
using std::cin;
using std::endl;

#define PROCESS_ID 0
#define PROCESS_NAME 1
#define PROCESS_SUSPICOUS 2
#define PROCESS_IS_SUSPICOUS 3

/* Global variables for GUI */
static HWND hList = NULL;  // List View identifier
HWND hEdit;

LVCOLUMN LvCol; // Make Coluom struct for ListView
LVITEM LvItem;  // ListView Item struct
LV_DISPINFO lvd;

int iSelect = 0;
int index = 0;
int flag = 0;

bool escKey = 0;

wchar_t tempstr[100]  = L"";
wchar_t tempstr2[100] = L"";

TCHAR tchar;

MSG msg;

/* init ProcessesMonitor */
//TODO remove pid after debug
unsigned int pid = 5300;
HoneypotsManager honeypotsManager;
//TODO use 
ProcessesMonitor processesMonitor = ProcessesMonitor(&honeypotsManager);
//ProcessesMonitor processesMonitor = ProcessesMonitor(&honeypotsManager, pid);

/* Handles */
HINSTANCE hInst;			// main function handler
#define WIN32_LEAN_AND_MEAN // this will assume smaller exe

/* Setting colors of the list */
LRESULT ProcessCustomDraw(LPARAM lParam)
{
	return CDRF_DODEFAULT;
}

/* refresh list of changes */
static void refreshList()
{
	wchar_t Temp[255] = { 0 };
	unsigned int i = 0;

	ListView_DeleteAllItems(hList);

	for (auto const processAnalyzer : processesMonitor.getAllProcessesAnalyzers()) {

		LvItem.iItem = i;
		LvItem.iSubItem = 0;
		SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&LvItem);

		LvItem.iSubItem = PROCESS_ID;
		swprintf(Temp, 255, L"%d", processAnalyzer.first);
		LvItem.pszText = Temp;
		SendMessage(hList, LVM_SETITEM, PROCESS_ID, (LPARAM)&LvItem);

		LvItem.iSubItem = PROCESS_NAME;
		swprintf(Temp, 255, L"%s", processAnalyzer.second->getProcessName().c_str());
		LvItem.pszText = Temp;
		SendMessage(hList, LVM_SETITEM, PROCESS_NAME, (LPARAM)&LvItem);

		LvItem.iSubItem = PROCESS_SUSPICOUS;
		swprintf(Temp, 255, L"%d", processAnalyzer.second->getCurrentScore());
		LvItem.pszText = Temp;
		SendMessage(hList, LVM_SETITEM, PROCESS_SUSPICOUS, (LPARAM)&LvItem);

		LvItem.iSubItem = PROCESS_IS_SUSPICOUS;
		swprintf(Temp, 255, L"%s", processAnalyzer.second->getCurrentScore() >= 100 ? L"YES" : L"NO");
		LvItem.pszText = Temp;
		SendMessage(hList, LVM_SETITEM, PROCESS_IS_SUSPICOUS, (LPARAM)&LvItem);

		++i;
	}
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		EndDialog(hWnd, 0); // kill dialog
	}
	break;

	case WM_NOTIFY:
	{
		switch (LOWORD(wParam))
		{
		case IDC_LIST:
			LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

			if (pnm->hdr.hwndFrom == hList &&pnm->hdr.code == NM_CUSTOMDRAW)
			{
				SetWindowLong(hWnd, DWL_MSGRESULT, (LONG)ProcessCustomDraw(lParam));
				return TRUE;
			}

			if (((LPNMHDR)lParam)->code == NM_DBLCLK)
			{
				wchar_t Text[255] = { 0 };
				wchar_t Temp[255] = { 0 };
				wchar_t Temp1[255] = { 0 };
				int iSlected = 0;
				int j = 0;

				iSlected = SendMessage(hList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);

				if (iSlected == -1)
				{
					MessageBox(hWnd, L"No Items in ListView", L"Error", MB_OK | MB_ICONINFORMATION);
					break;
				}

				memset(&LvItem, 0, sizeof(LvItem));
				LvItem.mask = LVIF_TEXT;
				LvItem.iSubItem = 0;
				LvItem.pszText = Text;
				LvItem.cchTextMax = 256;
				LvItem.iItem = iSlected;

				SendMessage(hList, LVM_GETITEMTEXT, iSlected, (LPARAM)&LvItem);

				swprintf(Temp1, 255, Text);

				for (j = 1; j <= 5; j++)
				{
					LvItem.iSubItem = j;
					SendMessage(hList, LVM_GETITEMTEXT, iSlected, (LPARAM)&LvItem);
					swprintf(Temp, 255, L" %s", Text);
					lstrcat(Temp1, Temp);
				}

				MessageBox(hWnd, Temp1, L"Process Info", MB_OK);

			}
			if (((LPNMHDR)lParam)->code == NM_CLICK)
			{
				iSelect = SendMessage(hList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);

				if (iSelect == -1)
				{
					break;
				}
				index = iSelect;
				flag = 1;
			}
			break;
		}
	}

	case WM_PAINT:
	{
		return 0;
	}
	break;

	case WM_TIMER:
		switch (wParam)
		{
		case IDT_TIMER:
			/* checking process liveness */
			processesMonitor.checkProcessesLiveness();

			return 0;
		}

	// This Window Message is the heart of the dialog  //
	//================================================//
	case WM_INITDIALOG:
	{
		int i;
		wchar_t Temp[255];
		LVBKIMAGE plvbki = { 0 };
		wchar_t url[] = L"C:\\a.jpg";
		InitCommonControls();
		hList = GetDlgItem(hWnd, IDC_LIST); // get the ID of the ListView				 

		memset(&plvbki, 0, sizeof(plvbki));
		plvbki.ulFlags = LVBKIF_SOURCE_URL;
		plvbki.pszImage = url;
		plvbki.xOffsetPercent = 40;
		plvbki.yOffsetPercent = 15;
		OleInitialize(NULL);

		SendMessage(hList, LVM_SETTEXTBKCOLOR, 0, (LPARAM)CLR_NONE);
		SendMessage(hList, LVM_SETBKIMAGE, 0, (LPARAM)(LPLVBKIMAGE)&plvbki);

		SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT); // Set style

		SendMessageA(hWnd, WM_SETICON, (WPARAM)1, (LPARAM)LoadIconA(hInst, MAKEINTRESOURCEA(IDI_ICON3)));

		// Here we put the info on the Coulom headers
		// this is not data, only name of each header we like
		memset(&LvCol, 0, sizeof(LvCol)); // Reset Coluom
		LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM; // Type of mask
		LvCol.cx = 0x80;

		/* Adding columns names */
		LvCol.pszText = L"Process ID";
		SendMessage(hList, LVM_INSERTCOLUMN, PROCESS_ID, (LPARAM)&LvCol);
		LvCol.pszText = L"Process Name";
		SendMessage(hList, LVM_INSERTCOLUMN, PROCESS_NAME, (LPARAM)&LvCol);
		LvCol.pszText = L"Suspicous Level";
		SendMessage(hList, LVM_INSERTCOLUMN, PROCESS_SUSPICOUS, (LPARAM)&LvCol);
		LvCol.pszText = L"Is suspicous";
		SendMessage(hList, LVM_INSERTCOLUMN, PROCESS_IS_SUSPICOUS, (LPARAM)&LvCol);

		//  Setting properties Of Items:
		memset(&LvItem, 0, sizeof(LvItem)); // Reset Item Struct
		LvItem.mask = LVIF_TEXT;   // Text Style
		LvItem.cchTextMax = 256;   // Max size of text


		//ListView_SetItemState(hList,0,LVIS_SELECTED	,LVIF_STATE);
		ShowWindow(hWnd, SW_NORMAL);
		UpdateWindow(hWnd);

		while (TRUE)
		{
			if (processesMonitor.isUpdateOccured()) {
				refreshList();
			}

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)// killing while looking for a message
				{
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	break;

	// This Window Message will control the dialog  //
	//==============================================//
	case WM_COMMAND:
	{
		iSelect = SendMessage(hList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);

		if (iSelect == -1)
		{
			break;
		}

		memset(&LvItem, 0, sizeof(LvItem));
		LvItem.mask = LVIF_TEXT;
		LvItem.iSubItem = 0;
		LvItem.pszText = tempstr2;
		LvItem.cchTextMax = 256;
		LvItem.iItem = iSelect;

		SendMessage(hList, LVM_GETITEMTEXT, iSelect, (LPARAM)&LvItem);

		switch (LOWORD(wParam)) // what we press on?
		{
		case IDC_KILL:
			processesMonitor.endProcess(std::stoi(LvItem.pszText));

			SendMessage(hList, LVM_DELETEITEM, iSelect, 0);
		break;

		case IDC_SUSPEND:
			processesMonitor.suspendProcess(std::stoi(LvItem.pszText));
			break;

		case IDC_RESUME:
			if (flag) {
				processesMonitor.resumeProcess(std::stoi(LvItem.pszText));
			}

			flag = 0;
			break;
		}
	}
	break;

	default:
	{
		return FALSE;
	}
	}

	return TRUE;
}

//===========================MAIN FUNCTION-WIN32 STARTING POINT========================================//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::initialize();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	BOOL bRet = InitCommonControlsEx(&InitCtrls);

	hInst = hInstance;

	//TODO fix timer
	//SetTimer(hList, IDT_TIMER, 6000, (TIMERPROC)NULL);

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDC_DIALOG), NULL, (DLGPROC)DialogProc, 0);

	KillTimer(hList, IDT_TIMER);

	return 0;
}
//======================================================================================================//