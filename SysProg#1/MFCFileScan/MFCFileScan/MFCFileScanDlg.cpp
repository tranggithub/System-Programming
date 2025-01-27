
// MFCFileScanDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCFileScan.h"
#include "MFCFileScanDlg.h"
#include "afxdialogex.h"
#include <filesystem>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
#include <vector>

using namespace std;


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
public:
	afx_msg void OnBnClickedButtonFind();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_Find, &CAboutDlg::OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CMFCFileScanDlg dialog



CMFCFileScanDlg::CMFCFileScanDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCFILESCAN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCFileScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCFileScanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Find, &CMFCFileScanDlg::OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CMFCFileScanDlg message handlers

BOOL CMFCFileScanDlg::OnInitDialog()
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
	CListCtrl* mylist = (CListCtrl*)GetDlgItem(IDC_ListControl_Output);
	mylist->GetClientRect(rect);
	mylist->InsertColumn(0, L"Filename", 0, rect.Width() / 8);
	mylist->InsertColumn(1, L"Size", 0, rect.Width() / 8);
	mylist->InsertColumn(2, L"Full Path", 0, rect.Width() * 4 / 8);
	mylist->InsertColumn(3, L"Created", 0, rect.Width() / 8);
	mylist->InsertColumn(4, L"Modified", 0, rect.Width() / 8);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCFileScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCFileScanDlg::OnPaint()
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
HCURSOR CMFCFileScanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAboutDlg::OnBnClickedButtonFind()
{
	// TODO: Add your control notification handler code here
	
}

void CMFCFileScanDlg::OnBnClickedButtonFind()
{
	// TODO: Add your control notification handler code here
	CString folder, filename;
	GetDlgItem(IDC_EDIT_FOLDER)->GetWindowTextW(folder);
	GetDlgItem(IDC_EDIT_FILENAME)->GetWindowTextW(filename);
	CListCtrl* mylist = (CListCtrl*)GetDlgItem(IDC_ListControl_Output);

	if (folder.IsEmpty() || GetFileAttributes(folder) == INVALID_FILE_ATTRIBUTES)
	{
		AfxMessageBox(_T("Invalid folder or empty folder "));
		return;
	}



	// Check if folder end with /
	if (folder.Right(1) != "\\")
		folder += "\\";

	CString searchPath = folder + filename;

	mylist->DeleteAllItems();

	// B?t ??u tìm ki?m
	int itemIndex = 0;
	SearchFilesInFolder(folder, filename, mylist, itemIndex);
	
	if (itemIndex == 0)
	{
		AfxMessageBox(_T("No File Found"));
	}

}

void CMFCFileScanDlg::SearchFilesInFolder(const CString& folderPath, const CString& fileNamePattern, CListCtrl* pListCtrl, int& itemIndex)
{
	CString searchPath = folderPath + "\\*"; // Tìm t?t c? các m?c trong folder
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		// B? qua "." và ".."
		if (_tcscmp(findData.cFileName, _T(".")) == 0 || _tcscmp(findData.cFileName, _T("..")) == 0)
			continue;

		CString fullPath = folderPath + "\\" + findData.cFileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// N?u là th? m?c, ?? quy tìm ki?m trong th? m?c con
			SearchFilesInFolder(fullPath, fileNamePattern, pListCtrl, itemIndex);
		}
		else
		{
			// Ki?m tra tên file có kh?p v?i pattern
			if (PathMatchSpec(findData.cFileName, fileNamePattern))
			{
				// L?y kích th??c file
				ULONGLONG fileSize = ((ULONGLONG)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
				CString size;
				size.Format(_T("%llu bytes"), fileSize);

				// Chuy?n FILETIME thành th?i gian d? ??c
				SYSTEMTIME stUTC, stLocal;
				FileTimeToSystemTime(&findData.ftLastWriteTime, &stUTC);
				SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

				CString modifyDate;
				modifyDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
					stLocal.wDay, stLocal.wMonth, stLocal.wYear,
					stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

				FileTimeToSystemTime(&findData.ftCreationTime, &stUTC);
				SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
				CString createDate;
				createDate.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
					stLocal.wDay, stLocal.wMonth, stLocal.wYear,
					stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

				// Thêm thông tin vào CListCtrl
				pListCtrl->InsertItem(itemIndex, findData.cFileName);
				pListCtrl->SetItemText(itemIndex, 1, size);
				pListCtrl->SetItemText(itemIndex, 2, fullPath);
				pListCtrl->SetItemText(itemIndex, 3, createDate);
				pListCtrl->SetItemText(itemIndex, 4, modifyDate);

				itemIndex++;
			}
		}
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
}