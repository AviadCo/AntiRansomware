// AntiRansomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment(lib, "comctl32.lib")

#include <fstream>
#include <iostream>

#include "HoneypotsManager.h"
#include "HoneypotNameGenerator.h"
#include "Honeypot.h"
#include "FileSystemHelper.h"
#include "ProcessesMonitor.h"
#include <algorithm>
#include <windows.h>
#include "resource\resource.h"
#include <commctrl.h>
#include <stdio.h>

#include "FunctionCalledHandlerWrapper.h"
#include "ProcessHookMonitorWrapper.h"
using std::wstring;
using std::list;
using std::wcout;
using std::cin;
using std::endl;

#define PROCESS_ID 0
#define PROCESS_NAME 1
#define PROCESS_SUSPICOUS 2
#define PROCESS_IS_SUSPICOUS 3

//==============Global Vatriabls===================
static HWND hList = NULL;  // List View identifier
LVCOLUMN LvCol; // Make Coluom struct for ListView
LVITEM LvItem;  // ListView Item struct
LV_DISPINFO lvd;
int iSelect = 0;
int index = 0;
int flag = 0;
HWND hEdit;
bool escKey = 0;
wchar_t tempstr[100] = L"";
wchar_t tempstr2[100] = L"";
TCHAR tchar;
MSG msg;

unsigned int pid = 5080;
HoneypotsManager honeypotsManager;
//TODO use ProcessesMonitor processesMonitor = ProcessesMonitor(&honeypotsManager);
ProcessesMonitor processesMonitor = ProcessesMonitor(&honeypotsManager, pid);
//===================================================

//======================Handles================================================//
HINSTANCE hInst; // main function handler
#define WIN32_LEAN_AND_MEAN // this will assume smaller exe

LRESULT ProcessCustomDraw(LPARAM lParam)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT: //Before the paint cycle begins
						//request notifications for individual listview items
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT: //Before an item is drawn
	{
		return CDRF_NOTIFYSUBITEMDRAW;
	}
	break;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT: //Before a subitem is drawn
	{
		switch (lplvcd->iSubItem)
		{
		case 0:
		{
			lplvcd->clrText = RGB(255, 255, 255);
			lplvcd->clrTextBk = RGB(240, 55, 23);
			return CDRF_NEWFONT;
		}
		break;

		case 1:
		{
			lplvcd->clrText = RGB(255, 255, 0);
			lplvcd->clrTextBk = RGB(0, 0, 0);
			return CDRF_NEWFONT;
		}
		break;

		case 2:
		{
			lplvcd->clrText = RGB(20, 26, 158);
			lplvcd->clrTextBk = RGB(200, 200, 10);
			return CDRF_NEWFONT;
		}
		break;

		case 3:
		{
			lplvcd->clrText = RGB(12, 15, 46);
			lplvcd->clrTextBk = RGB(200, 200, 200);
			return CDRF_NEWFONT;
		}
		break;

		case 4:
		{
			lplvcd->clrText = RGB(120, 0, 128);
			lplvcd->clrTextBk = RGB(20, 200, 200);
			return CDRF_NEWFONT;
		}
		break;

		case 5:
		{
			lplvcd->clrText = RGB(255, 255, 255);
			lplvcd->clrTextBk = RGB(0, 0, 150);
			return CDRF_NEWFONT;
		}
		break;

		}

	}
	}
	return CDRF_DODEFAULT;
}

//================================About dialog window=============================//
static void refreshList()
{
	wchar_t Temp[255] = { 0 };
	unsigned int i = 0;

	for (auto const processAnalyzer : processesMonitor.getAllProcessesAnalyzers()) {

		LvItem.iItem = i;
		SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&LvItem);
		//TODO change this to get all process information

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

		// This Window Message will close the dialog  //
		//============================================//

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

				MessageBox(hWnd, Temp1, L"test", MB_OK);

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

			if (((LPNMHDR)lParam)->code == LVN_BEGINLABELEDIT)
			{
				//Editing=1;
				hEdit = ListView_GetEditControl(hList);
				GetWindowText(hEdit, tempstr, sizeof(tempstr));
			}

			if (((LPNMHDR)lParam)->code == LVN_ENDLABELEDIT)
			{
				int iIndex;
				wchar_t text[255] = L"";

				tchar = (TCHAR)msg.wParam;
				if (tchar == 0x1b)
					escKey = 1;

				iIndex = SendMessage(hList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
				if (iIndex == -1)
					break;

				LvItem.iSubItem = 0;

				if (escKey == 0)
				{
					LvItem.pszText = text;
					GetWindowText(hEdit, text, sizeof(text));
					SendMessage(hList, LVM_SETITEMTEXT, (WPARAM)iIndex, (LPARAM)&LvItem);
				}
				else {
					LvItem.pszText = tempstr;
					SendMessage(hList, LVM_SETITEMTEXT, (WPARAM)iIndex, (LPARAM)&LvItem);
					escKey = 0;
				}
				//Editing=0;
			}
			break;
		}
	}

	case WM_PAINT:
	{
		return 0;
	}
	break;

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
	// add this code if win2000/nt/xp doesn't
	// load ur winmain (experimental only)
	// also add comctl32.lib

	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::initialize();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwICC = ICC_LISTVIEW_CLASSES;
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	BOOL bRet = InitCommonControlsEx(&InitCtrls);

	hInst = hInstance;

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDC_DIALOG), NULL, (DLGPROC)DialogProc, 0);
	return 0;
}
//======================================================================================================//