//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

// KhuDaNetAppDlg.h : header file
//

#pragma once
#include "afxwin.h"

#define TIMER_ID_MNIST_TRAIN	1


// CKhuDaNetAppDlg dialog
class CKhuDaNetAppDlg : public CDialogEx
{
// Construction
public:
	CKhuDaNet m_Network;

	char m_ExePath[MAX_PATH];
	char m_TrainImagePath[MAX_PATH];
	char m_TrainLabelPath[MAX_PATH];
	char m_TestImagePath[MAX_PATH];
	char m_TestLabelPath[MAX_PATH];

	int m_nMnistTrainCnt, m_nMnistTestCnt;
	int m_nEpochCnt, m_nBatchCnt;

	double **m_MnistTrainInput;
	int *m_MnistTrainOutput;

	double **m_MnistTestInput;
	int *m_MnistTestOutput;

	bool m_bRunMnistTrain;

	void DlgPrintf(CListBox &ListBox, LPCTSTR ptr, ...);

	void LoadMnistTrain();
	void LoadMnistTest();

	CKhuDaNetAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KHUDANETAPP_DIALOG };
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
	afx_msg void OnBnClickedTrain();
	CListBox m_MsgList1;
	CListBox m_MsgList2;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedReset();
	CListBox m_MsgList3;
	afx_msg void OnBnClickedRandomTest();
};
