
// MFCProcessViewerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCProcessViewer.h"
#include "MFCProcessViewerDlg.h"
#include "afxdialogex.h"
#include <Psapi.h>
#include <afxwin.h>
#include <afxcmn.h>
#include <tlhelp32.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
#include <winternl.h>


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCProcessViewerDlg dialog



CMFCProcessViewerDlg::CMFCProcessViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCPROCESSVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCProcessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCProcessViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Refresh, &CMFCProcessViewerDlg::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CMFCProcessViewerDlg message handlers

BOOL CMFCProcessViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rect;
	CListCtrl* mylist = (CListCtrl*)GetDlgItem(IDC_LIST_Process);
	mylist->GetClientRect(rect);
	mylist->InsertColumn(0, L"Process ID", 0, rect.Width() / 8);
	mylist->InsertColumn(1, L"Process Name", 0, rect.Width() / 8);
	mylist->InsertColumn(2, L"Command Line", 0, rect.Width() * 4 / 8);
	mylist->InsertColumn(3, L"Full Path", 0, rect.Width() * 2 / 8);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCProcessViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCProcessViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCProcessViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Hàm h? tr? l?y command line và ???ng d?n ??y ??
std::wstring GetProcessCommandLine(DWORD processID)
{
	std::wstring commandLine;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess) {
		PROCESS_BASIC_INFORMATION pbi;
		ZeroMemory(&pbi, sizeof(pbi));

		// L?y ??a ch? PEB
		ULONG ReturnLength = 0;
		NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &ReturnLength);

		// ??c PEB
		PEB peb;
		SIZE_T bytesRead = 0;
		if (ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), &bytesRead)) {
			// ??c ProcessParameters
			RTL_USER_PROCESS_PARAMETERS params;
			if (ReadProcessMemory(hProcess, peb.ProcessParameters, &params, sizeof(params), &bytesRead)) {
				// ??c Command Line
				WCHAR* buffer = new WCHAR[params.CommandLine.Length / sizeof(WCHAR) + 1];
				if (ReadProcessMemory(hProcess, params.CommandLine.Buffer, buffer, params.CommandLine.Length, &bytesRead)) {
					buffer[params.CommandLine.Length / sizeof(WCHAR)] = 0; // Null-terminate
					commandLine = buffer;
				}
				delete[] buffer;
			}
		}
		CloseHandle(hProcess);
	}

	return commandLine;
}

std::wstring GetProcessFullPath(DWORD processID)
{
	WCHAR filePath[MAX_PATH] = { 0 };
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	if (hProcess)
	{
		if (GetModuleFileNameEx(hProcess, NULL, filePath, MAX_PATH))
		{
			CloseHandle(hProcess);
			return filePath;
		}
		CloseHandle(hProcess);
	}

	return L"";
}

// Hàm li?t kê process và c?p nh?t vào CListCtrl
void PopulateProcessList(CListCtrl* pListCtrl)
{
	// Xóa danh sách c?
	pListCtrl->DeleteAllItems();

	// T?o snapshot c?a các process
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return;

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &processEntry))
	{
		int itemIndex = 0;

		do
		{
			DWORD processID = processEntry.th32ProcessID;
			std::wstring processName = processEntry.szExeFile;

			// L?y command line và full path
			std::wstring commandLine = GetProcessCommandLine(processID);
			std::wstring fullPath = GetProcessFullPath(processID);

			// Thêm vào CListCtrl
			int index = pListCtrl->InsertItem(itemIndex, std::to_wstring(processID).c_str());
			pListCtrl->SetItemText(index, 1, processName.c_str());
			pListCtrl->SetItemText(index, 2, commandLine.c_str());
			pListCtrl->SetItemText(index, 3, fullPath.c_str());

			itemIndex++;
		} while (Process32Next(hSnapshot, &processEntry));
	}

	CloseHandle(hSnapshot);
}


void CMFCProcessViewerDlg::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_Process);
	PopulateProcessList(pListCtrl);
}
