
// MFCClientChatroomDlg.h : header file
//

#pragma once


// CMFCClientChatroomDlg dialog
class CMFCClientChatroomDlg : public CDialogEx
{
// Construction
public:
	CMFCClientChatroomDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCLIENTCHATROOM_DIALOG };
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
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonSend();
	void ConnectToServer();
	static UINT ReceiveThread(LPVOID pParam);
	void DisconnectFromServer();
	void SendMessageToServer();

	void UpdateChatroom(const CString& strMessage);

	SOCKET m_clientSocket;
	bool m_isConnected;

	CEdit m_editServerIP;
	CEdit m_editServerPort;
	CButton m_buttonConnect;
	CEdit m_editChatroom;
	CEdit m_editMessage;
	CEdit m_editUsername;

	CWinThread* m_pReceiveThread;  // Lu?ng ?? nh?n tin nh?n
};
