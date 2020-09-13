// MyExplorer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
#include "MyExplorer.h"
#include "DriveSystem.h"


#define MAX_LOADSTRING 100
#define MAX_LEN_PATH 10240
struct SizeW{
	int width;
	int height;
	SizeW::SizeW(int w, int h){
		width = w;
		height = h;
	}
};
// Function:
HWND createTreeView(HWND hWnd);
HWND createListView(HWND hWnd);
void initListView(bool isDrive);
void addDriveToTreeView(DriveSystem *drive);
void addDriveToListView(DriveSystem *drive);
void loadTreeViewItem(HTREEITEM &hParent, LPCWSTR path);
void loadListViewItem(LPCWSTR path);
void loadChildTree(HTREEITEM hSelected);
void loadSelected();
void loadItemToLisview(LPCWSTR path);
void MakeImageList();
bool CheckNumItemPath(HTREEITEM &hParent, LPCWSTR path);
LPCWSTR getPathTree(HTREEITEM hItem);
LPCWSTR getPathList(int Item);
TCHAR*  getPrevPath(LPCWSTR path);
LPWSTR SYSTEMTIMEtoSTRING(SYSTEMTIME time);
LPWSTR GetFileAttr(DWORD fileAttr);
std::string FillString(std::string str);
bool CheckExtension(LPCTSTR path, LPCTSTR type);
LPWSTR GetType(const WIN32_FIND_DATA &fd);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnPaint(HWND hwnd);
void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void loadItemToLisview(LPCWSTR path, LPCWSTR find);
int dirExists(std::wstring path);
std::string ws2s(const std::wstring& wstr);
LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MiniProc1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MiniProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
SizeW ReadConfig();
void SaveConfig();
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hTreeView = NULL;
HWND hListView = NULL;
HWND hButtonPrev;
HWND hTextBoxPath;
HWND hTextBoxSearch;
HWND hStatusBar;
HWND hWndMain;
DriveSystem* Drive;
TCHAR* pathVisit;
TCHAR* pathPrev;
WNDPROC pEditProc;
WNDPROC pMiniProc1;
WNDPROC pMiniProc2;
int listIcon[14];
int Counter;
int PartXSize;
HCURSOR hcSizeEW;
#define ICON_DESKTOP  0
#define ICON_FOLDER   1
#define ICON_DRIVE    2
#define ICON_COMPUTER 3
#define ICON_EXE	  4
#define ICON_FILE	  5
#define ICON_TXT	  6
#define ICON_ZIP	  7
#define ICON_DOC	  8
#define ICON_PDF	  9
#define ICON_JPG	  10
#define ICON_PNG	  11
#define ICON_MP3	  12
#define ICON_MP4	  13
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MYEXPLORER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYEXPLORER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYEXPLORER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MYEXPLORER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable
	SizeW size = ReadConfig();
	if (size.width != -1){
		hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, size.width, size.height, NULL, NULL, hInstance, NULL);
	}
	else{
		hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	}
	hWndMain = hWnd;
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
BOOL xSizing = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HWND test;
	switch (message)
	{
		//HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
	case WM_NOTIFY:
	{
		int Index;

		NMHDR* notifyMess = (NMHDR*)lParam;
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)notifyMess;
		HTREEITEM hSelected;
		int id_sel = -1;
		switch (notifyMess->code)
		{
		case TVN_ITEMEXPANDING:
			hSelected = lpnmTree->itemNew.hItem;
			loadChildTree(hSelected);
			break;
		case TVN_SELCHANGED:
			hSelected = TreeView_GetSelection(hTreeView);
			//TreeView_Expand(hTreeView, hSelected, TVE_EXPAND);
			ListView_DeleteAllItems(hListView);
			loadListViewItem(getPathTree(hSelected));
			break;
		case NM_CLICK:
			if (notifyMess->hwndFrom == hTreeView){
				hSelected = TreeView_GetSelection(hTreeView);
				if (hSelected != NULL){
					ListView_DeleteAllItems(hListView);
					loadListViewItem(getPathTree(hSelected));
				}
			}
			if (notifyMess->hwndFrom == hListView){
				id_sel = ListView_GetSelectionMark(hListView);
				int number = ListView_GetSelectedCount(hListView);
				if (number == 0){
					SendMessage(hStatusBar, SB_SETTEXT, 0,
						(LPARAM)"");
					break;
				}
				if (id_sel >= 0){
					TCHAR buffer[255];
					int size = 0;
					char HDtext[255];
					HWND hwndHD = ListView_GetHeader(hListView);
					HDITEM hdi = { 0 };
					hdi.mask = HDI_TEXT;
					hdi.pszText = buffer;
					hdi.cchTextMax = 255;
					Header_GetItem(hwndHD, 3, &hdi);

					if (_tcscmp(buffer, _T("Size")) == 0){
						ListView_GetItemText(hListView, id_sel, 2, buffer, 255);
						if (_tcscmp(buffer, _T("File folder")) == 0 || number > 1){
							wsprintf(buffer, L"%d items     %d item selected", Counter, number);
							SendMessage(hStatusBar, SB_SETTEXT, 0,
								(LPARAM)buffer);
						}
						else{
							TCHAR bSize[255];
							ListView_GetItemText(hListView, id_sel, 3, bSize, 255);
							wsprintf(buffer, L"%d items     %d item selected  %s", Counter, number, bSize);
							SendMessage(hStatusBar, SB_SETTEXT, 0,
								(LPARAM)buffer);
						}

					}
					else{
						wsprintf(buffer, L"%d items     %d item selected", Counter, number);
						SendMessage(hStatusBar, SB_SETTEXT, 0,
							(LPARAM)buffer);
					}
				}
				else{
					SendMessage(hStatusBar, SB_SETTEXT, 0,
						(LPARAM)"");
				}
			}
			break;

		case NM_DBLCLK:
			//Get hwndFrom for window handle to the control sending the message
			//To check whether this event fire by Listview
			if (notifyMess->hwndFrom == hListView)
				loadSelected();
			//if (notifyMess->hwndFrom == hTreeView)
			//	loadSelected();
			break;
		}

	}
	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
		//case WM_DESTROY:
	case WM_CLOSE:
		SaveConfig();
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	int wmId, wmEvent;
	wmId = LOWORD(id);
	wmEvent = HIWORD(id);
	// Parse the menu selections:
	switch (wmId)
	{
	case BUTTON_ID:
		//MessageBox(hwnd, 0, L"Notepad", MB_YESNOCANCEL);
		ListView_DeleteAllItems(hListView);
		loadListViewItem(pathPrev);
		break;
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;
	case IDM_EXIT:
		SaveConfig();
		DestroyWindow(hwnd);
		break;
	}
}
//void OnPaint(HWND hwnd);
void OnDestroy(HWND hwnd){
	SaveConfig();
	PostQuitMessage(0);
}

LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int sizeTextBox;
	WCHAR* textTextBox;
	int sizePath;
	WCHAR* textPath;
	TCHAR* Path;
	switch (msg)
	{
	case WM_KEYUP:
		if (hwnd == hTextBoxSearch){
			sizeTextBox = GetWindowTextLength(hTextBoxSearch);
			textTextBox = new WCHAR[sizeTextBox + 1];
			GetWindowText(hTextBoxSearch, textTextBox, sizeTextBox + 1);

			sizePath = GetWindowTextLength(hTextBoxPath);
			textPath = new WCHAR[sizePath + 1];
			GetWindowText(hTextBoxPath, textPath, sizePath + 1);
			ListView_DeleteAllItems(hListView);
			loadItemToLisview(textPath, textTextBox);
		}
		if (hwnd == hTextBoxPath){
			if (13 == wParam){
				sizeTextBox = GetWindowTextLength(hTextBoxPath);
				textTextBox = new WCHAR[sizeTextBox + 1];
				GetWindowText(hTextBoxPath, textTextBox, sizeTextBox + 1);
				int isPath = dirExists(textTextBox);
				if (isPath == 0){
					ShellExecute(NULL, _T("open"), textTextBox, NULL, NULL, SW_SHOWNORMAL);
					SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)pathVisit);

				}
				else if (isPath == 1){
					ListView_DeleteAllItems(hListView);
					loadListViewItem(textTextBox);
				}
				else{
					MessageBox(hwnd, L"Window can't find path. Check the spelling and try again.", L"Explorer", MB_OK);
					SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)pathVisit);
				}

			}
		}
		break;
	default:
		break;
	}
	return CallWindowProc(pEditProc, hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK MiniProc1(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
		RECT rect;
		int width;
		int height;
		HWND hwndTV;
		int xPos;
		int yPos;
		if (GetWindowRect(hWndMain, &rect))
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		// Varible used to get the mouse cursor x and y co-ordinates
		xPos = (int)LOWORD(lParam);
		yPos = (int)HIWORD(lParam);

		xSizing = (xPos >= PartXSize - 5 &&
			xPos <= PartXSize + 5);
		if (xSizing)
		{
			SetCursor(hcSizeEW);
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		int xPos;
		int yPos;

		// Get the x and y co-ordinates of the mouse
		xPos = (int)LOWORD(lParam);
		yPos = (int)HIWORD(lParam);

		// Checks if the left button is pressed during dragging the splitter
		if (wParam == MK_LBUTTON)
		{
			if (xSizing)
			{
				PartXSize = xPos;
				RECT rect;
				GetClientRect(hWndMain, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				if (PartXSize >= width*0.1 && PartXSize <= width*0.9){
					SetWindowPos(hListView, NULL, PartXSize, 32, width - PartXSize, height - 32 - 24, NULL);
					SetWindowPos(hTreeView, NULL, 0, 32, PartXSize, height - 32 - 24, NULL);
				}

			}
		}
		if ((xPos > (PartXSize - 5) && xPos < (PartXSize + 5)))
		{
			SetCursor(hcSizeEW);
		}
		break;
	}
	case WM_LBUTTONUP:{
		xSizing = FALSE;
		break;
	}
	default:
		break;
	}
	return CallWindowProc(pMiniProc1, hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK MiniProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		int xPos;
		int yPos;

		// Get the x and y co-ordinates of the mouse
		xPos = (int)LOWORD(lParam);
		yPos = (int)HIWORD(lParam);

		// Checks if the left button is pressed during dragging the splitter
		if (wParam == MK_LBUTTON)
		{
			if (xSizing)
			{
				PartXSize = PartXSize + xPos;
				RECT rect;
				GetClientRect(hWndMain, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				if (PartXSize >= width*0.1 && PartXSize <= width*0.9){
					SetWindowPos(hListView, NULL, PartXSize, 32, width - PartXSize, height - 32 - 24, NULL);
					SetWindowPos(hTreeView, NULL, 0, 32, PartXSize, height - 32 - 24, NULL);
				}
			}
		}
		/*if ((PartXSize + xPos > (PartXSize - 5) && PartXSize + xPos < (PartXSize + 5)))
		{
			SetCursor(hcSizeEW);
		}*/
		break;
	}
	case WM_LBUTTONUP:{
		xSizing = FALSE;
		break;
	}
	default:
		break;
	}
	return CallWindowProc(pMiniProc2, hwnd, msg, wParam, lParam);
}
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct){
	RECT rect;
	int width;
	int height;
	HWND hwndTV;
	if (GetWindowRect(hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	PartXSize = width*0.2;
	hButtonPrev = CreateWindow(L"button", L"",
		WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_DEFPUSHBUTTON,
		0, 0,
		32, 32,
		hwnd, (HMENU)BUTTON_ID,
		hInst, NULL);
	HBITMAP hbmp = NULL;     // handle to bitmap 
	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP16));
	SendMessage(
		(HWND)hButtonPrev,
		(UINT)BM_SETIMAGE,
		(WPARAM)IMAGE_BITMAP,
		(LPARAM)hbmp);
	hTextBoxPath = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 40, 6, (width - 40)*0.7, 22, hwnd, (HMENU)IDI_TEXTBOX, hInst, NULL);
	hTextBoxSearch = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 40 + (width - 40)*0.7, 6, (width - 40)*0.3, 22, hwnd, (HMENU)IDI_TEXTBOX_2, hInst, NULL);
	if (!(pEditProc = (WNDPROC)SetWindowLong(hTextBoxSearch, GWL_WNDPROC, (LONG)&EditProc)))
	{
		return false;
	}

	if (!(pEditProc = (WNDPROC)SetWindowLong(hTextBoxPath, GWL_WNDPROC, (LONG)&EditProc)))
	{
		return false;
	}
	hStatusBar = CreateWindow(STATUSCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0, 0, 0, 0, hwnd, NULL, hInst, NULL);
	Drive = new DriveSystem();
	Drive->getSystemDrives();
	hTreeView = createTreeView(hwnd);
	hListView = createListView(hwnd);
	MakeImageList();
	addDriveToTreeView(Drive);

	if (!(pMiniProc1 = (WNDPROC)SetWindowLong(hTreeView, GWL_WNDPROC, (LONG)&MiniProc1)))
	{
		return false;
	}

	if (!(pMiniProc2 = (WNDPROC)SetWindowLong(hListView, GWL_WNDPROC, (LONG)&MiniProc2)))
	{
		return false;
	}
	hcSizeEW = LoadCursor(NULL, IDC_SIZEWE);
	return TRUE;
}
void OnSize(HWND hwnd, UINT state, int cx, int cy){
	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	SetWindowPos(hListView, NULL, PartXSize, 32, width - PartXSize, height - 32 - 24, NULL);
	SetWindowPos(hTreeView, NULL, 0, 32, PartXSize, height - 32 - 24, NULL);
	SetWindowPos(hStatusBar, NULL, 0, 0, width, 24, NULL);
}


HWND createListView(HWND hWnd){
	RECT rect;
	int width;
	int height;
	HWND hwndTV;
	if (GetWindowRect(hWnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	hwndTV = CreateWindowEx(WS_EX_CLIENTEDGE,
		WC_LISTVIEW,
		TEXT("List View"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_SHAREIMAGELISTS | WS_TABSTOP | LVS_REPORT | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS,
		width*0.2,
		32,
		width*0.8,
		height*0.89 - 32 - 24,
		hWnd,
		(HMENU)ID_LISTVIEW,
		hInst,
		NULL);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 150;
	lvCol.pszText = _T("Name");
	ListView_InsertColumn(hwndTV, 0, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.pszText = _T("Type");
	lvCol.cx = 125;
	ListView_InsertColumn(hwndTV, 1, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 125;
	lvCol.pszText = _T("Total Size");
	ListView_InsertColumn(hwndTV, 2, &lvCol);


	lvCol.fmt = LVCFMT_LEFT;
	lvCol.pszText = _T("Free Space");
	lvCol.cx = 125;
	ListView_InsertColumn(hwndTV, 3, &lvCol);
	return hwndTV;
}

HWND createTreeView(HWND hWnd){
	RECT rect;
	int width;
	int height;
	HWND hwndTV;
	if (GetWindowRect(hWnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		0,
		32,
		width*0.2,
		height*0.89 - 32 - 24,
		hWnd,
		(HMENU)ID_TREEVIEW,
		hInst,
		NULL);
	return hwndTV;
}

void addDriveToTreeView(DriveSystem *drive){
	TV_INSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	//Desktop
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.iImage = listIcon[ICON_DESKTOP];
	tvInsert.item.iSelectedImage = listIcon[ICON_DESKTOP];
	tvInsert.item.pszText = _T("Desktop");
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	HTREEITEM hDesktop = TreeView_InsertItem(hTreeView, &tvInsert);
	//My Computer
	tvInsert.hParent = hDesktop;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.iImage = listIcon[ICON_COMPUTER];
	tvInsert.item.iSelectedImage = listIcon[ICON_COMPUTER];
	tvInsert.item.pszText = _T("My Computer");
	tvInsert.item.lParam = (LPARAM)_T("MyComputer");
	HTREEITEM hMyComputer = TreeView_InsertItem(hTreeView, &tvInsert);

	for (int i = 0; i < drive->getCount(); ++i)
	{
		tvInsert.hParent = hMyComputer; //Them
		tvInsert.item.iImage = listIcon[ICON_DRIVE];
		tvInsert.item.iSelectedImage = listIcon[ICON_DRIVE];
		tvInsert.item.pszText = drive->getDisplayName(i);
		tvInsert.item.lParam = (LPARAM)drive->getDriveName(i);
		HTREEITEM hDrive = TreeView_InsertItem(hTreeView, &tvInsert);
		if (CheckNumItemPath(hDrive, getPathTree(hDrive))){
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hDrive;
			tvInsert.hInsertAfter = TVI_SORT;
			tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			tvInsert.item.iImage = listIcon[ICON_FOLDER];
			tvInsert.item.iSelectedImage = listIcon[ICON_FOLDER];
			tvInsert.item.pszText = L"Load";
			tvInsert.item.lParam = (LPARAM)L"Load";
			HTREEITEM hItem = TreeView_InsertItem(hTreeView, &tvInsert);
		}
		//loadTreeViewItem(hDrive, getPathTree(hDrive));
	}//for
	TreeView_SelectItem(hTreeView, hMyComputer);
}

void loadTreeViewItem(HTREEITEM &hParent, LPCWSTR path){

	TCHAR buffer[MAX_LEN_PATH];
	StrCpy(buffer, path);
	StrCat(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_SORT;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = listIcon[ICON_FOLDER];
	tvInsert.item.iSelectedImage = listIcon[ICON_FOLDER];

	WIN32_FIND_DATA ffd; //Contains information about the file that is found by Find first file and Find next file
	HANDLE hFind = FindFirstFileW(buffer, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	TCHAR* folderPath;
	do
	{
		DWORD fileAttribute = ffd.dwFileAttributes;
		if ((fileAttribute & FILE_ATTRIBUTE_DIRECTORY) //Get only directory and folder
			&& ((fileAttribute & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) //Not hidden
			&& (_tcscmp(ffd.cFileName, _T(".")) != 0) && (_tcscmp(ffd.cFileName, _T("..")) != 0)) //Ignore . (curr dir) and .. (parent dir)
		{
			//Set file name

			tvInsert.item.pszText = ffd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(ffd.cFileName) + 2];

			//Set path
			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, ffd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(hTreeView, &tvInsert);
			if (CheckNumItemPath(hItem, getPathTree(hItem))){
				TV_INSERTSTRUCT tvInsert;
				tvInsert.hParent = hItem;
				tvInsert.hInsertAfter = TVI_SORT;
				tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
				tvInsert.item.iImage = listIcon[ICON_FOLDER];
				tvInsert.item.iSelectedImage = listIcon[ICON_FOLDER];
				tvInsert.item.pszText = L"Load";
				tvInsert.item.lParam = (LPARAM)L"Load";
				HTREEITEM hItem = TreeView_InsertItem(hTreeView, &tvInsert);
			}
			//delete folderPath;
		}
	} while (FindNextFileW(hFind, &ffd));
}

bool CheckNumItemPath(HTREEITEM &hParent, LPCWSTR path){
	TCHAR buffer[MAX_LEN_PATH];
	StrCpy(buffer, path);
	StrCat(buffer, _T("\\*"));

	WIN32_FIND_DATA ffd; //Contains information about the file that is found by Find first file and Find next file

	HANDLE hFind = FindFirstFileW(buffer, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	do
	{
		DWORD fileAttribute = ffd.dwFileAttributes;
		if ((fileAttribute & FILE_ATTRIBUTE_DIRECTORY) //Get only directory and folder
			&& (fileAttribute != FILE_ATTRIBUTE_HIDDEN) //Not hidden
			&& (_tcscmp(ffd.cFileName, _T(".")) != 0) && (_tcscmp(ffd.cFileName, _T("..")) != 0)) //Ignore . (curr dir) and .. (parent dir)
		{
			return true;
		}
	} while (FindNextFileW(hFind, &ffd));
	return false;
}

void loadChildTree(HTREEITEM hSelected){
	HTREEITEM myDesktop = TreeView_GetRoot(hTreeView); //Return the top most or very first item of the TreeView
	HTREEITEM myComputer = TreeView_GetChild(hTreeView, myDesktop); //Return the first child item of Desktop (MyComputer)
	LPCWSTR data = getPathTree(hSelected);
	SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)data);
	if (hSelected == myComputer) //If currently select My Computer -> dont load
		return;

	HTREEITEM hCurrSelectedChild = TreeView_GetChild(hTreeView, hSelected); //Get the first child of treeview item
	if (data == L"Desktop") return;
	while (hCurrSelectedChild != NULL){
		TreeView_DeleteItem(hTreeView, hCurrSelectedChild);
		hCurrSelectedChild = TreeView_GetChild(hTreeView, hSelected);
	}
	loadTreeViewItem(hSelected, getPathTree(hSelected));
}


void addDriveToListView(DriveSystem *drive){
	initListView(true);
	LV_ITEM lv;
	TCHAR buffer[255];
	Counter = drive->getCount();
	wsprintf(buffer, L"%d items", Counter);
	SendMessage(hStatusBar, SB_SETTEXT, 0,
		(LPARAM)buffer);
	for (int i = 0; i < drive->getCount(); ++i)
	{
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

		lv.iItem = i;
		lv.iImage = listIcon[ICON_DRIVE];
		lv.iSubItem = 0;
		lv.pszText = drive->getDisplayName(i);
		lv.lParam = (LPARAM)drive->getDriveName(i);
		ListView_InsertItem(hListView, &lv);

		lv.mask = LVIF_TEXT;

		lv.iSubItem = 1;
		lv.pszText = drive->getDriveType(i);
		ListView_SetItem(hListView, &lv);

		//Load total size to third column
		lv.iSubItem = 2;
		if (wcscmp(drive->getDriveType(i), L"CD-ROM") != 0)
			lv.pszText = drive->getTotalSize(i);
		else
			lv.pszText = NULL;
		ListView_SetItem(hListView, &lv);

		//Load Free Space to last column
		lv.iSubItem = 3;
		if (wcscmp(drive->getDriveType(i), L"CD-ROM") != 0)
			lv.pszText = drive->getFreeSpace(i);
		else
			lv.pszText = NULL;

		//Set
		ListView_SetItem(hListView, &lv);
	}
}

void loadSelected(){

	LPCWSTR filePath = getPathList(ListView_GetSelectionMark(hListView));
	if (filePath == NULL) return;
	if (_tcscmp(filePath, _T("MyComputer")) == 0){
		ListView_DeleteAllItems(hListView);
		SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)filePath);
		addDriveToListView(Drive);
		return;
	}

	WIN32_FIND_DATA fd;

	//Retrieves attributes for a specified file or directory.
	if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fd) != 0)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ListView_DeleteAllItems(hListView);
			loadItemToLisview(filePath);
		}
		else
		{
			ShellExecute(NULL, _T("open"), filePath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}



void loadListViewItem(LPCWSTR path){
	if (path == NULL)
		return;
	SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)path);
	//if (_tcscmp(path, _T("C:\\Windows\\WinSxS")) == 0) return;
	HBITMAP hbmp = NULL;     // handle to bitmap 
	LV_ITEM lv;
	if (_tcscmp(path, _T("Desktop")) == 0)
	{
		initListView(false);

		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = 0;
		lv.iSubItem = 0;
		lv.pszText = _T("My Computer");
		lv.iImage = listIcon[ICON_COMPUTER];
		lv.lParam = (LPARAM)_T("MyComputer");
		ListView_InsertItem(hListView, &lv); //Inserts a new item in a list-view control
		TCHAR buffer[255];
		Counter = 1;
		wsprintf(buffer, L"%d items", Counter);
		SendMessage(hStatusBar, SB_SETTEXT, 0,
			(LPARAM)buffer);
	}
	else if (_tcscmp(path, _T("MyComputer")) == 0)
	{
		//Load My Computer to Listview (Drives, Volume,..)
		initListView(true);
		addDriveToListView(Drive);
		hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP16));
		SendMessage(
			(HWND)hButtonPrev,
			(UINT)BM_SETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			(LPARAM)hbmp);
	}
	else
		loadItemToLisview(path);
}
void loadItemToLisview(LPCWSTR path, LPCWSTR find){
	initListView(false);
	SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)path);
	pathVisit = new TCHAR[255];
	StrCpy(pathVisit, path);
	TCHAR buffer[MAX_LEN_PATH];
	StrCpy(buffer, path);
	if (wcslen(path) == 3){
		StrCat(buffer, _T("*"));
		StrCat(buffer, find);
		StrCat(buffer, _T("*"));
	}
	else{
		StrCat(buffer, _T("\\*"));
		StrCat(buffer, find);
		StrCat(buffer, _T("*"));
	}

	WIN32_FIND_DATA hFile;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LV_ITEM lv;
	int Index = 0;
	TCHAR* tempPath;
	hFind = FindFirstFileW(buffer, &hFile);

	if (hFind == INVALID_HANDLE_VALUE)
		return;


	pathPrev = new TCHAR[255];
	pathPrev = getPrevPath(path);

	HBITMAP hbmp = NULL;     // handle to bitmap 
	if (_tcscmp(path, pathPrev) != 0){
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = Index;
		lv.iSubItem = 0;
		lv.pszText = L"..";
		lv.iImage = listIcon[ICON_FOLDER];
		lv.lParam = (LPARAM)pathPrev;
		ListView_InsertItem(hListView, &lv);
		//SYSTEMTIME st;
		//FileTimeToSystemTime(&hFile.ftLastWriteTime, &st);
		//ListView_SetItemText(hListView, Index, 1, SYSTEMTIMEtoSTRING(st)); //Changes the text of a list - view item or subitem
		ListView_SetItemText(hListView, Index, 2, _T("File folder"));
		Index++;

		hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP15));
		SendMessage(
			(HWND)hButtonPrev,
			(UINT)BM_SETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			(LPARAM)hbmp);
	}
	else{
		hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP16));
		SendMessage(
			(HWND)hButtonPrev,
			(UINT)BM_SETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			(LPARAM)hbmp);
	}
	do
	{
		tempPath = new TCHAR[wcslen(path) + wcslen(hFile.cFileName) + 2];
		StrCpy(tempPath, path);
		if (wcslen(path) != 3)
			StrCat(tempPath, _T("\\"));
		StrCat(tempPath, hFile.cFileName);
		if ((hFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((hFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& (_tcscmp(hFile.cFileName, _T(".")) != 0) && (_tcscmp(hFile.cFileName, _T("..")) != 0))
		{
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = Index;
			lv.iSubItem = 0;
			lv.pszText = hFile.cFileName;
			lv.iImage = listIcon[ICON_FOLDER];
			lv.lParam = (LPARAM)tempPath;
			ListView_InsertItem(hListView, &lv);

			SYSTEMTIME st;
			FileTimeToSystemTime(&hFile.ftLastWriteTime, &st);
			ListView_SetItemText(hListView, Index, 1, SYSTEMTIMEtoSTRING(st)); //Changes the text of a list - view item or subitem
			ListView_SetItemText(hListView, Index, 2, _T("File folder"));
			Index++;
		}
		else{
			if (((hFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
				((hFile.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
				((hFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)){
				//Add name and path to first column
				lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lv.iItem = Index;
				lv.iSubItem = 0;
				lv.pszText = hFile.cFileName;
				lv.lParam = (LPARAM)tempPath;
				if (CheckExtension(tempPath, L".exe")){
					lv.iImage = listIcon[ICON_EXE];
				}
				else if (CheckExtension(tempPath, L".txt")){
					lv.iImage = listIcon[ICON_TXT];
				}
				else if (CheckExtension(tempPath, L".png")){
					lv.iImage = listIcon[ICON_PNG];
				}
				else if (CheckExtension(tempPath, L".mp3")){
					lv.iImage = listIcon[ICON_MP3];
				}
				else if (CheckExtension(tempPath, L".mp4")){
					lv.iImage = listIcon[ICON_MP4];
				}
				else if (CheckExtension(tempPath, L".JPG")){
					lv.iImage = listIcon[ICON_JPG];
				}
				else if (CheckExtension(tempPath, L".doc") || CheckExtension(tempPath, L".docx")){
					lv.iImage = listIcon[ICON_DOC];
				}
				else if (CheckExtension(tempPath, L".pdf")){
					lv.iImage = listIcon[ICON_PDF];
				}
				else if (CheckExtension(tempPath, L".zip") || CheckExtension(tempPath, L".rar") || CheckExtension(tempPath, L".7z")){
					lv.iImage = listIcon[ICON_ZIP];
				}
				else{
					lv.iImage = listIcon[ICON_FILE];
				}
				ListView_InsertItem(hListView, &lv);
				SYSTEMTIME st;
				FileTimeToSystemTime(&hFile.ftLastWriteTime, &st);
				ListView_SetItemText(hListView, Index, 1, SYSTEMTIMEtoSTRING(st));

				ListView_SetItemText(hListView, Index, 2, GetType(hFile));
				//Last column is Size
				DWORD fileSizeLow = hFile.nFileSizeLow; //The low-order DWORD value of the file size, in bytes
				ListView_SetItemText(hListView, Index, 3, SizeDrive::Convert(fileSizeLow));
				Index++;
			}
		}

		//Continues a file search from a previous call to the FindFirstFileW function
		//Return non-zero if successfully found, otherwise return zero
	} while (FindNextFileW(hFind, &hFile));
	//WCHAR* buffer = new WCHAR[255];
	Counter = Index - 1;
	wsprintf(buffer, L"%d items", Counter);
	SendMessage(hStatusBar, SB_SETTEXT, 0,
		(LPARAM)buffer);
}

void loadItemToLisview(LPCWSTR path){
	initListView(false);
	SendMessage(hTextBoxPath, WM_SETTEXT, 0, (LPARAM)path);
	pathVisit = new TCHAR[255];
	StrCpy(pathVisit, path);
	TCHAR buffer[MAX_LEN_PATH];
	StrCpy(buffer, path);
	if (wcslen(path) == 3)
		StrCat(buffer, _T("*"));
	else
		StrCat(buffer, _T("\\*"));
	WIN32_FIND_DATA hFile;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LV_ITEM lv;
	int Index = 0;
	TCHAR* tempPath;
	hFind = FindFirstFileW(buffer, &hFile);

	if (hFind == INVALID_HANDLE_VALUE)
		return;


	pathPrev = new TCHAR[255];
	pathPrev = getPrevPath(path);

	HBITMAP hbmp = NULL;     // handle to bitmap 
	if (_tcscmp(path, pathPrev) != 0){
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = Index;
		lv.iSubItem = 0;
		lv.pszText = L"..";
		lv.iImage = listIcon[ICON_FOLDER];
		lv.lParam = (LPARAM)pathPrev;
		ListView_InsertItem(hListView, &lv);
		//SYSTEMTIME st;
		//FileTimeToSystemTime(&hFile.ftLastWriteTime, &st);
		//ListView_SetItemText(hListView, Index, 1, SYSTEMTIMEtoSTRING(st)); //Changes the text of a list - view item or subitem
		ListView_SetItemText(hListView, Index, 2, _T("File folder"));
		Index++;

		hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP15));
		SendMessage(
			(HWND)hButtonPrev,
			(UINT)BM_SETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			(LPARAM)hbmp);
	}
	else{
		hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP16));
		SendMessage(
			(HWND)hButtonPrev,
			(UINT)BM_SETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			(LPARAM)hbmp);
	}
	do
	{
		tempPath = new TCHAR[wcslen(path) + wcslen(hFile.cFileName) + 2];
		StrCpy(tempPath, path);
		if (wcslen(path) != 3)
			StrCat(tempPath, _T("\\"));
		StrCat(tempPath, hFile.cFileName);
		if ((hFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((hFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& (_tcscmp(hFile.cFileName, _T(".")) != 0) && (_tcscmp(hFile.cFileName, _T("..")) != 0))
		{
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = Index;
			lv.iSubItem = 0;
			lv.pszText = hFile.cFileName;
			lv.iImage = listIcon[ICON_FOLDER];
			lv.lParam = (LPARAM)tempPath;
			ListView_InsertItem(hListView, &lv);

			SYSTEMTIME st;
			FileTimeToSystemTime(&hFile.ftLastWriteTime, &st);
			ListView_SetItemText(hListView, Index, 1, SYSTEMTIMEtoSTRING(st)); //Changes the text of a list - view item or subitem
			ListView_SetItemText(hListView, Index, 2, _T("File folder"));
			Index++;
		}
		else{
			if (((hFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
				((hFile.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
				((hFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)){
				//Add name and path to first column
				lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lv.iItem = Index;
				lv.iSubItem = 0;
				lv.pszText = hFile.cFileName;
				lv.lParam = (LPARAM)tempPath;
				if (CheckExtension(tempPath, L".exe")){
					lv.iImage = listIcon[ICON_EXE];
				}
				else if (CheckExtension(tempPath, L".txt")){
					lv.iImage = listIcon[ICON_TXT];
				}
				else if (CheckExtension(tempPath, L".png")){
					lv.iImage = listIcon[ICON_PNG];
				}
				else if (CheckExtension(tempPath, L".mp3")){
					lv.iImage = listIcon[ICON_MP3];
				}
				else if (CheckExtension(tempPath, L".mp4")){
					lv.iImage = listIcon[ICON_MP4];
				}
				else if (CheckExtension(tempPath, L".JPG")){
					lv.iImage = listIcon[ICON_JPG];
				}
				else if (CheckExtension(tempPath, L".doc") || CheckExtension(tempPath, L".docx")){
					lv.iImage = listIcon[ICON_DOC];
				}
				else if (CheckExtension(tempPath, L".pdf")){
					lv.iImage = listIcon[ICON_PDF];
				}
				else if (CheckExtension(tempPath, L".zip") || CheckExtension(tempPath, L".rar") || CheckExtension(tempPath, L".7z")){
					lv.iImage = listIcon[ICON_ZIP];
				}
				else{
					lv.iImage = listIcon[ICON_FILE];
				}
				ListView_InsertItem(hListView, &lv);
				SYSTEMTIME st;
				FileTimeToSystemTime(&hFile.ftLastWriteTime, &st);
				ListView_SetItemText(hListView, Index, 1, SYSTEMTIMEtoSTRING(st));

				ListView_SetItemText(hListView, Index, 2, GetType(hFile));
				//Last column is Size
				DWORD fileSizeLow = hFile.nFileSizeLow; //The low-order DWORD value of the file size, in bytes
				ListView_SetItemText(hListView, Index, 3, SizeDrive::Convert(fileSizeLow));

				ListView_SetItemText(hListView, Index, 4, GetFileAttr(hFile.dwFileAttributes));
				Index++;
			}
		}

		//Continues a file search from a previous call to the FindFirstFileW function
		//Return non-zero if successfully found, otherwise return zero
	} while (FindNextFileW(hFind, &hFile));
	Counter = Index - 1;
	wsprintf(buffer, L"%d items", Counter);
	SendMessage(hStatusBar, SB_SETTEXT, 0,
		(LPARAM)buffer);
}

void initListView(bool isDrive){
	LVCOLUMN lvCol;
	if (isDrive)
	{
		lvCol.mask = LVCF_TEXT | LVCF_FMT;

		lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
		lvCol.cx = 100;
		lvCol.pszText = _T("Type");
		ListView_SetColumn(hListView, 1, &lvCol);

		lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
		lvCol.cx = 80;
		lvCol.pszText = _T("Total Size");
		ListView_SetColumn(hListView, 2, &lvCol);

		lvCol.cx = 80;
		lvCol.pszText = _T("Free Space");
		ListView_SetColumn(hListView, 3, &lvCol);
	}
	else
	{
		lvCol.mask = LVCF_WIDTH;
		lvCol.cx = 180;
		ListView_SetColumn(hListView, 0, &lvCol);

		lvCol.mask = LVCF_TEXT | LVCF_FMT;
		lvCol.fmt = LVCFMT_RIGHT;
		lvCol.pszText = _T("Date Modified");
		ListView_SetColumn(hListView, 1, &lvCol);


		lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvCol.fmt = LVCFMT_LEFT;
		lvCol.cx = 130;
		lvCol.pszText = _T("Type");
		ListView_SetColumn(hListView, 2, &lvCol);

		lvCol.pszText = _T("Size");
		ListView_SetColumn(hListView, 3, &lvCol);


		lvCol.fmt = LVCFMT_LEFT;
		lvCol.pszText = _T("Attr");
		lvCol.cx = 50;
		ListView_InsertColumn(hListView, 4, &lvCol);
	}
}

LPCWSTR getPathTree(HTREEITEM hItem){
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(hTreeView, &tv);
	return (LPCWSTR)tv.lParam;
}

LPCWSTR getPathList(int Item){
	if (Item == -1) return NULL;
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = Item;
	lv.iSubItem = 0;
	ListView_GetItem(hListView, &lv);
	return (LPCWSTR)lv.lParam;

}

LPWSTR SYSTEMTIMEtoSTRING(SYSTEMTIME time){
	std::string result = "";
	TCHAR *buffer = new TCHAR[50];
	result += FillString(std::to_string(time.wYear)) + "/";
	result += FillString(std::to_string(time.wMonth)) + "/";
	result += FillString(std::to_string(time.wDay)) + " ";
	result += FillString(std::to_string(time.wHour)) + ":";
	result += FillString(std::to_string(time.wMinute)) + ":";
	result += FillString(std::to_string(time.wSecond));
	size_t outSize;
	mbstowcs_s(&outSize, buffer, 50, result.c_str(), strlen(result.c_str()) + 1);
	return buffer; //result.c_str();
}
LPWSTR GetFileAttr(DWORD fileAttr)
{
	LPWSTR fileAttrStr = L"";

	switch (fileAttr)
	{
	case FILE_ATTRIBUTE_ARCHIVE:
		fileAttrStr = L"A";
		break;
	case FILE_ATTRIBUTE_COMPRESSED:
		fileAttrStr = L"C";
		break;
	}

	return fileAttrStr;
}
std::string FillString(std::string str){
	if (str.length() == 1){
		return "0" + str;
	}
	return str;
}

void MakeImageList(){
	HIMAGELIST himl = NULL;  // handle to image list 
	HBITMAP hbmp = NULL;     // handle to bitmap 
	himl = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 8, 3);

	//// Add the open file, closed file, and document bitmaps. 
	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	listIcon[ICON_DESKTOP] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	listIcon[ICON_COMPUTER] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	listIcon[ICON_FOLDER] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
	listIcon[ICON_DRIVE] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5));
	listIcon[ICON_EXE] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
	listIcon[ICON_FILE] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
	listIcon[ICON_TXT] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP8));
	listIcon[ICON_ZIP] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
	listIcon[ICON_DOC] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP10));
	listIcon[ICON_PDF] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP11));
	listIcon[ICON_JPG] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP12));
	listIcon[ICON_PNG] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP13));
	listIcon[ICON_MP3] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP14));
	listIcon[ICON_MP4] = ImageList_Add(himl, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);
	//// Associate the image list with the tree-view control. 
	TreeView_SetImageList(hTreeView, himl, TVSIL_NORMAL);
	ListView_SetImageList(hListView, himl, LVSIL_SMALL);
}

bool CheckExtension(LPCTSTR path, LPCTSTR type){
	LPCTSTR p = _tcsrchr(path, _T('.'));
	if (p == NULL)
		return false;
	if (lstrcmpi(p, type) == 0) return true;
	return false;
}
TCHAR* getPrevPath(LPCWSTR path){
	int len = lstrlen(path);
	std::wstring str = path;
	TCHAR* result;
	if (_tcscmp(path, L"MyComputer") == 0){
		result = new TCHAR[12];
		StrCpy(result, L"DeskTop");
		return result;
	}
	if (str[len - 1] == '\\' && str[len - 2] == ':') {
		result = new TCHAR[12];
		StrCpy(result, L"MyComputer");
		return result;
	}
	str = str.substr(0, str.find_last_of(L'\\'));
	if (str[str.length() - 1] == ':') str = str + L"\\";
	result = new TCHAR[str.length() + 1];
	StrCpy(result, str.c_str());
	return result;
}

LPWSTR GetType(const WIN32_FIND_DATA &fd)
{

	std::wstring buffer = fd.cFileName;
	std::size_t found = buffer.find(L".");
	if (found == std::wstring::npos)
		buffer = L"File";
	else
		buffer = buffer.substr(buffer.find_last_of(L".") + 1);

	TCHAR *szExtension = new TCHAR[buffer.length()];
	StrCpy(szExtension, buffer.c_str());

	return szExtension;
}

int dirExists(std::wstring path)
{
	if (path == L"Desktop") return 1;
	if (path == L"MyComputer") return 1;
	std::string _path = ws2s(path);
	DWORD ftyp = GetFileAttributesA(_path.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return -1;  //something is wrong with your path!

	if ((ftyp & FILE_ATTRIBUTE_DIRECTORY) &&
		((ftyp & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)){
		return 1;
	}
	if (((ftyp & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
		((ftyp & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
		((ftyp & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)){
		return 0;
	}
	if (ftyp == 22) return 1;
	return -1;    // this is not a directory!
}

std::string ws2s(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), 0, 0, 0, 0);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), &strTo[0], size_needed, 0, 0);
	return strTo;
}
SizeW ReadConfig(){
	TCHAR width[255];
	TCHAR height[255];
	GetPrivateProfileString(TEXT("Size Window"),
		TEXT("width"),
		TEXT("-1"),
		width,
		255,
		TEXT(".\\config.ini"));
	GetPrivateProfileString(TEXT("Size Window"),
		TEXT("height"),
		TEXT("-1"),
		height,
		255,
		TEXT(".\\config.ini"));
	if (_tcscmp(width, L"-1") == 0 || _tcscmp(height, L"-1") == 0){
		return SizeW(-1, -1);
	}
	else{
		return SizeW(_wtoi(width), _wtoi(height));
	}
}

void SaveConfig(){
	RECT rect;
	int width;
	int height;
	if (GetWindowRect(hWndMain, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	TCHAR buffer1[255];
	wsprintf(buffer1, L"%d", width);
	TCHAR buffer2[255];
	wsprintf(buffer2, L"%d", height);
	WritePrivateProfileString(TEXT("Size Window"),
		TEXT("width"),
		buffer1,
		TEXT(".\\config.ini")
		);
	WritePrivateProfileString(TEXT("Size Window"),
		TEXT("height"),
		buffer2,
		TEXT(".\\config.ini")
		);
}