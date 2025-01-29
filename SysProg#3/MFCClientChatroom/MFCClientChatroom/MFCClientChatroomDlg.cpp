
// MFCClientChatroomDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCClientChatroom.h"
#include "MFCClientChatroomDlg.h"
#include "afxdialogex.h"
#include "Ws2tcpip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMFCClientChatroomDlg dialog



CMFCClientChatroomDlg::CMFCClientChatroomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCCLIENTCHATROOM_DIALOG, pParent), m_clientSocket(INVALID_SOCKET), m_isConnected(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCClientChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SERVER_IP, m_editServerIP);
	DDX_Control(pDX, IDC_EDIT_SERVER_PORT, m_editServerPort);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_buttonConnect);
	DDX_Control(pDX, IDC_EDIT_CHATROOM, m_editChatroom);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMessage);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUsername);
}

BEGIN_MESSAGE_MAP(CMFCClientChatroomDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMFCClientChatroomDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMFCClientChatroomDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CMFCClientChatroomDlg message handlers

BOOL CMFCClientChatroomDlg::OnInitDialog()
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
	m_editServerIP.SetWindowTextW(L"127.0.0.1");
	m_editServerPort.SetWindowTextW(L"4444");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCClientChatroomDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCClientChatroomDlg::OnPaint()
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
HCURSOR CMFCClientChatroomDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCClientChatroomDlg::OnBnClickedButtonConnect()
{
	// TODO: Add your control notification handler code here
	if (!m_isConnected)
	{
		ConnectToServer();
	}
	else
	{
		DisconnectFromServer();
	}
}

void CMFCClientChatroomDlg::ConnectToServer()
{
	CString strIP, strPort;
	m_editServerIP.GetWindowText(strIP);
	m_editServerPort.GetWindowText(strPort);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		AfxMessageBox(_T("WSAStartup failed"));
		return;
	}

	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("Socket creation failed"));
		WSACleanup();
		return;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, strIP, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(_ttoi(strPort));

	if (connect(m_clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Connect failed"));
		closesocket(m_clientSocket);
		WSACleanup();
		return;
	}

	m_isConnected = true;
	m_buttonConnect.SetWindowText(_T("DISCONNECT"));
	AfxMessageBox(_T("Connected to server"));

	// B?t ??u lu?ng nh?n tin nh?n
	m_pReceiveThread = AfxBeginThread(ReceiveThread, this);
}

// Hàm lu?ng ?? nh?n tin nh?n t? server
UINT CMFCClientChatroomDlg::ReceiveThread(LPVOID pParam)
{
	CMFCClientChatroomDlg* pDlg = (CMFCClientChatroomDlg*)pParam;
	char buffer[1024];
	int bytesReceived;

	while (pDlg->m_isConnected)
	{
		bytesReceived = recv(pDlg->m_clientSocket, buffer, sizeof(buffer), 0);
		if (bytesReceived > 0)
		{
			buffer[bytesReceived] = '\0';  // ??m b?o k?t thúc chu?i
			CString strMessage(buffer);
			pDlg->UpdateChatroom(strMessage);  // C?p nh?t chatroom v?i tin nh?n m?i
		}
		else if (bytesReceived == 0)
		{
			// Server ?ã ?óng k?t n?i
			pDlg->DisconnectFromServer();
			break;
		}
		else
		{
			// L?i khi nh?n tin nh?n
			pDlg->DisconnectFromServer();
			break;
		}
	}
	return 0;
}

void CMFCClientChatroomDlg::DisconnectFromServer()
{
	m_isConnected = false;
	m_buttonConnect.SetWindowText(_T("CONNECT"));
	closesocket(m_clientSocket);
	WSACleanup();
	AfxMessageBox(_T("Disconnected from server"));
}

void CMFCClientChatroomDlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	if (m_isConnected)
	{
		SendMessageToServer();
	}
	else
	{
		AfxMessageBox(_T("Not connected to server"));
	}
}

void CMFCClientChatroomDlg::SendMessageToServer()
{
	// sending data
	CString strMessage;
	CString strUsername;

	m_editUsername.GetWindowText(strUsername);
	m_editMessage.GetWindowText(strMessage);

	if (send(m_clientSocket, CT2A(strUsername + _T(" : ") + strMessage), strMessage.GetLength() + strUsername.GetLength() + 3, 0) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Send failed"));
	}
	else
	{
		CString strChat;
		m_editChatroom.GetWindowText(strChat);
		strChat += _T("You: ") + strMessage + _T("\r\n");
		m_editChatroom.SetWindowText(strChat);
		m_editMessage.SetWindowText(_T(""));
	}
}

void CMFCClientChatroomDlg::UpdateChatroom(const CString& strMessage)
{
	// C?p nh?t chatroom v?i tin nh?n m?i
	CString strChat;
	m_editChatroom.GetWindowText(strChat);
	strChat += strMessage + _T("\r\n");
	m_editChatroom.SetWindowText(strChat);
}