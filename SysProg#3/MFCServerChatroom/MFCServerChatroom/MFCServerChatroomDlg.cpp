
// MFCServerChatroomDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCServerChatroom.h"
#include "MFCServerChatroomDlg.h"
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


// CMFCServerChatroomDlg dialog



CMFCServerChatroomDlg::CMFCServerChatroomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSERVERCHATROOM_DIALOG, pParent), m_serverSocket(INVALID_SOCKET), m_bListening(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCServerChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_editIP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Control(pDX, IDC_BUTTON_START, m_buttonStart);
	DDX_Control(pDX, IDC_LIST_CLIENTS, m_listClients);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
}

BEGIN_MESSAGE_MAP(CMFCServerChatroomDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CMFCServerChatroomDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMFCServerChatroomDlg message handlers

BOOL CMFCServerChatroomDlg::OnInitDialog()
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
	m_editIP.SetWindowTextW(L"0.0.0.0");
	m_editPort.SetWindowTextW(L"4444");

	CRect rect;
	CListCtrl* m_listClients = (CListCtrl*)GetDlgItem(IDC_LIST_CLIENTS);
	m_listClients->GetClientRect(rect);
	m_listClients->InsertColumn(0, _T("IP Address"), LVCFMT_LEFT, rect.Width()*2 / 3);
	m_listClients->InsertColumn(1, _T("Port"), LVCFMT_LEFT, rect.Width() / 3);

	CRect rect1;
	CListCtrl* m_listLog = (CListCtrl*)GetDlgItem(IDC_LIST_LOG);
	m_listLog->GetClientRect(rect1);
	m_listLog->InsertColumn(0, _T("Time"), LVCFMT_LEFT, rect1.Width() / 3);
	m_listLog->InsertColumn(1, _T("Message"), LVCFMT_LEFT, rect1.Width()*2 / 3);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCServerChatroomDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCServerChatroomDlg::OnPaint()
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
HCURSOR CMFCServerChatroomDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCServerChatroomDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	if (!m_bListening)
	{
		StartServer();
	}
	else
	{
		StopServer();
	}
}

void CMFCServerChatroomDlg::StartServer()
{
	CString strIP, strPort;
	m_editIP.GetWindowText(strIP);
	m_editPort.GetWindowText(strPort);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LogMessage(_T("WSAStartup failed"));
		return;
	}

	UINT nPort = _ttoi(strPort);
	m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverSocket != INVALID_SOCKET) {
		SOCKADDR_IN serverAddr = {};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(nPort);
		InetPton(AF_INET, strIP, &serverAddr.sin_addr.s_addr);

		if (bind(m_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			AfxMessageBox(_T("Failed to bind the server socket."));
			return;
		}

		if (listen(m_serverSocket, SOMAXCONN) != SOCKET_ERROR) {
			m_bListening = TRUE;
			LogMessage(_T("Server started listening."));
			std::thread(&CMFCServerChatroomDlg::AcceptConnections, this).detach();

			m_buttonStart.SetWindowText(_T("STOP"));
		}
		else {
			AfxMessageBox(_T("Failed to listen on the specified port."));
		}
	}
	else {
		AfxMessageBox(_T("Failed to create server socket."));
		WSACleanup();
	}
}
void CMFCServerChatroomDlg::AcceptConnections() {
	while (m_bListening) {
		SOCKET clientSocket = accept(m_serverSocket, nullptr, nullptr);
		if (clientSocket != INVALID_SOCKET) {
			WCHAR clientIP[IP4_ADDRESS_STRING_LENGTH];
			UINT clientPort;
			sockaddr_in clientAddr;
			int addrLen = sizeof(clientAddr);
			getpeername(clientSocket, (sockaddr*)&clientAddr, &addrLen);
			InetNtop(AF_INET, &clientAddr.sin_addr, clientIP, IP4_ADDRESS_STRING_LENGTH);
			clientPort = ntohs(clientAddr.sin_port);
			CString str3 = clientIP;
			AddClientToList(str3, clientPort);
			LogMessage(_T("Client connected: ") + str3 + _T(":") + std::to_wstring(clientPort).c_str());

			m_mutex.lock();
			m_clientSockets.push_back(clientSocket);
			m_mutex.unlock();

			m_threads.emplace_back(&CMFCServerChatroomDlg::HandleClient, this, clientSocket);
		}
		else {
			closesocket(clientSocket);
		}
	}
}

void CMFCServerChatroomDlg::HandleClient(SOCKET clientSocket) {
	char buffer[1024];
	while (true) {
		int received = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (received > 0) {
			buffer[received] = '\0';
			BroadcastMessage(buffer, clientSocket);
		}
		else {
			break;
		}
	}

	WCHAR clientIP[IP4_ADDRESS_STRING_LENGTH];
	UINT clientPort;

	sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	getpeername(clientSocket, (sockaddr*)&clientAddr, &addrLen);
	InetNtop(AF_INET, &clientAddr.sin_addr, clientIP, IP4_ADDRESS_STRING_LENGTH);
	clientPort = ntohs(clientAddr.sin_port);
	CString str3 = clientIP;

	LogMessage(_T("Client disconnected: ") + str3 + _T(":") + std::to_wstring(clientPort).c_str());

	m_mutex.lock();
	auto it = std::find(m_clientSockets.begin(), m_clientSockets.end(), clientSocket);
	if (it != m_clientSockets.end()) {
		m_clientSockets.erase(it);
	}
	m_mutex.unlock();

	closesocket(clientSocket);
}

void CMFCServerChatroomDlg::BroadcastMessage(const char* message, SOCKET sender) {
	m_mutex.lock();
	for (auto& client : m_clientSockets) {
		if (client != sender) {
			send(client, message, strlen(message), 0);
		}
	}
	m_mutex.unlock();

	CString strMessage;
	strMessage.Format(_T("Broadcast: %S"), message);
	LogMessage(strMessage);
}

void CMFCServerChatroomDlg::AddClientToList(CString ip, UINT port) {
	int index = m_listClients.GetItemCount();
	m_listClients.InsertItem(index, ip);
	CString strPort;
	strPort.Format(_T("%u"), port);
	m_listClients.SetItemText(index, 1, strPort);
}

void CMFCServerChatroomDlg::StopServer()
{
	m_bListening = FALSE;
	closesocket(m_serverSocket);

	for (auto& client : m_clientSockets) {
		closesocket(client);
	}
	m_clientSockets.clear();

	for (auto& t : m_threads) {
		if (t.joinable()) t.join();
	}
	m_threads.clear();
	m_buttonStart.SetWindowText(_T("START"));
	m_listClients.DeleteAllItems();
	LogMessage(_T("Server stopped."));
}

void CMFCServerChatroomDlg::LogMessage(const CString& message)
{
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("%H:%M:%S"));
	int nIndex = m_listLog.InsertItem(m_listLog.GetItemCount(), strTime);
	m_listLog.SetItemText(nIndex, 1, message);
}