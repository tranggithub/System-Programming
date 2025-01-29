
// MFCServerChatroomDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <thread>
#include <vector>
#include <mutex>
#include <winsock2.h>
#include <afxsock.h>

// CMFCServerChatroomDlg dialog
class CMFCServerChatroomDlg : public CDialogEx
{
// Construction
public:
	CMFCServerChatroomDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSERVERCHATROOM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();

protected:
	void StartServer();
	void StopServer();
	void LogMessage(const CString& message);
	void AcceptConnections();
	void AddClientToList(CString ip, UINT port);
	void HandleClient(SOCKET clientSocket);
	void BroadcastMessage(const char* message, SOCKET sender);

	CEdit m_editIP;
	CEdit m_editPort;
	CButton m_buttonStart;
	CListCtrl m_listClients;
	CListCtrl m_listLog;

	BOOL m_bListening;
	SOCKET m_serverSocket;
	std::vector<std::thread> m_threads;
	std::vector<SOCKET> m_clientSockets;
	std::mutex m_mutex;
};
