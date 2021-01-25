//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

// KhuDaNetAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KhuDaNetApp.h"
#include "KhuDaNet.h"
#include "KhuDaNetAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER_ {
	unsigned short		bfType;
	unsigned long		bfSize;
	unsigned short		bfReserved1;
	unsigned short		bfReserved2;
	unsigned long		bfOffBits;
} BITMAPFILEHEADER_;

typedef struct tagBITMAPINFOHEADER_ {
	unsigned long       biSize;
	long				biWidth;
	long				biHeight;
	unsigned short      biPlanes;
	unsigned short      biBitCount;
	unsigned long       biCompression;
	unsigned long       biSizeImage;
	long				biXPelsPerMeter;
	long				biYPelsPerMeter;
	unsigned long       biClrUsed;
	unsigned long       biClrImportant;
} BITMAPINFOHEADER_;

typedef struct tagRGBQUAD_ {
	unsigned char    rgbBlue;
	unsigned char    rgbGreen;
	unsigned char    rgbRed;
	unsigned char    rgbReserved;
} RGBQUAD_;
#pragma pack(pop)

#define BI_RGB_        0L

int GetBmp24Size(int nW, int nH) {
	return (nW*3+3)/4*4 * nH;
}

int GetBmp24Pos(int nW, int nH, int x, int y) {
	return (nW*3+3)/4*4 * (nH-1-y) + x*3;
}

bool SaveBmp(char *FileName, unsigned char *Image1D, int nW, int nH) {
	unsigned long dwBitsSize;

	unsigned long size;

	size = GetBmp24Size(nW, nH);
	dwBitsSize = sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_) + size;

	// 헤더 정보 저장
	BITMAPINFOHEADER_ bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER_);
	bmiHeader.biWidth = nW;
	bmiHeader.biHeight = nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB_;
	bmiHeader.biSizeImage = size;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER_ bmiFileHeader;
	bmiFileHeader.bfType = 'M'*0x0100 + 'B';
	bmiFileHeader.bfSize = dwBitsSize;
	bmiFileHeader.bfReserved1 = 0;
	bmiFileHeader.bfReserved2 = 0;

	bmiFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_);

	FILE *fp;

	fopen_s(&fp, FileName, "wb");

	if(!fp) return false;

	fwrite(&bmiFileHeader, sizeof(BITMAPFILEHEADER_), 1, fp);
	fwrite(&bmiHeader, sizeof(BITMAPINFOHEADER_), 1, fp);

	fwrite(Image1D, sizeof(unsigned char), size, fp);

	fclose(fp);

	return true;
}

unsigned char *ReadBmp(char *FileName, int *pW, int *pH) {
	BITMAPFILEHEADER_ bmiFileHeader;
	BITMAPINFOHEADER_ bmiHeader;

	FILE *fp;
	int nColors;

	fopen_s(&fp, FileName, "rb");
	if(!fp) return NULL;

	fread(&bmiFileHeader, sizeof(BITMAPFILEHEADER_), 1, fp);

	if(bmiFileHeader.bfType != 'M'*0x0100 + 'B') {
		fclose(fp);
		return NULL;
	}

	fread(&bmiHeader, sizeof(BITMAPINFOHEADER_), 1, fp);

	if(bmiHeader.biCompression != BI_RGB_) {
		fclose(fp);
		return NULL;
	}

	if(bmiHeader.biClrUsed == 0) 
		nColors = 1 << bmiHeader.biBitCount;
	else
		nColors = bmiHeader.biClrUsed;

	RGBQUAD_ *Palette = NULL;

	switch (bmiHeader.biBitCount) {
	case 24:
		break;

	case 1:
	case 4:
	case 8:
		Palette = new RGBQUAD_[nColors];

		int i;
		for(i = 0 ; i < nColors ; i++) {
			unsigned char r, g, b, temp;

			fread(&b, sizeof(unsigned char), 1, fp);
			fread(&g, sizeof(unsigned char), 1, fp);
			fread(&r, sizeof(unsigned char), 1, fp);
			fread(&temp, sizeof(unsigned char), 1, fp);

			Palette[i].rgbRed = r;
			Palette[i].rgbGreen = g;
			Palette[i].rgbBlue = b;
		}
		break;
	}

	fseek(fp, bmiFileHeader.bfOffBits, SEEK_SET);

	*pW = bmiHeader.biWidth;
	*pH = bmiHeader.biHeight;

	unsigned char *Image1D = NULL;

	Image1D = new unsigned char[GetBmp24Size(*pW, *pH)];

	if(bmiHeader.biBitCount == 24)
		fread(Image1D, sizeof(unsigned char), *pH*((*pW*3+3)/4*4), fp);

	long Row, Col;

	for(Row = 0 ; Row < bmiHeader.biHeight ; Row++)
	{
		if(bmiHeader.biBitCount != 24) 
		{
			int BitCount = 0;
			unsigned int mask = (1 << bmiHeader.biBitCount) - 1;

			unsigned char ReadByte = 0;
			int ReadByteCnt = 0;

			for(Col = 0; Col < *pW ; Col++) 
			{				
				int PaletteIndex = 0;

				if (BitCount <= 0) {
					BitCount = 8;
					fread(&ReadByte, sizeof(unsigned char), 1, fp);
					ReadByteCnt++;
				}

				BitCount -= bmiHeader.biBitCount;

				PaletteIndex = (ReadByte >> BitCount) & mask;

				int Pos;

				Pos = (((*pW*3+3)/4*4) * Row) + Col*3;	
				Image1D[Pos++] = Palette[PaletteIndex].rgbBlue;
				Image1D[Pos++] = Palette[PaletteIndex].rgbGreen;
				Image1D[Pos] = Palette[PaletteIndex].rgbRed;
			}

			while (ReadByteCnt&3) 
			{
				char temp;
				fread(&temp, sizeof(char), 1, fp);
				ReadByteCnt++;
			}
		}
	}

	if(Palette) delete [] Palette;

	fclose(fp);

	return Image1D;
}


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
//	afx_msg void OnDestroy();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CKhuDaNetAppDlg dialog



CKhuDaNetAppDlg::CKhuDaNetAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_KHUDANETAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	GetModuleFileName(NULL, m_ExePath, MAX_PATH);

	int i;
	int LastBackSlash = -1;
	int nLen = strlen(m_ExePath);
	for(i = nLen-1 ; i >= 0 ; i--)
	{
		if(m_ExePath[i] == '\\') {
			LastBackSlash = i;
			break;
		}
	}

	if(LastBackSlash >= 0)
		m_ExePath[LastBackSlash] = 0;

	m_nMnistTrainCnt = 60000;
	m_nMnistTestCnt = 10000;

	m_MnistTrainInput = NULL;
	m_MnistTrainOutput = NULL;

	m_MnistTestInput = NULL;
	m_MnistTestOutput = NULL;

	m_bRunMnistTrain = false;

	LoadMnistTrain();
	LoadMnistTest();
}

void CKhuDaNetAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_MsgList1);
	DDX_Control(pDX, IDC_LIST2, m_MsgList2);
	DDX_Control(pDX, IDC_LIST3, m_MsgList3);
}

BEGIN_MESSAGE_MAP(CKhuDaNetAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TRAIN, &CKhuDaNetAppDlg::OnBnClickedTrain)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SAVE, &CKhuDaNetAppDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_LOAD, &CKhuDaNetAppDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_RESET, &CKhuDaNetAppDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_RANDOM_TEST, &CKhuDaNetAppDlg::OnBnClickedRandomTest)
END_MESSAGE_MAP()


// CKhuDaNetAppDlg message handlers

BOOL CKhuDaNetAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	SetDlgItemInt(IDC_BATCH_SIZE, 200);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKhuDaNetAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CKhuDaNetAppDlg::OnPaint()
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
HCURSOR CKhuDaNetAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKhuDaNetAppDlg::DlgPrintf(CListBox &ListBox, LPCTSTR ptr, ...)
{
	CString Buf;
	int Num;

	TCHAR ach[512];
	va_list args; 
	va_start(args, ptr); 
	_vstprintf_s(ach, 512, ptr, args);
	Buf.Format(ach);
	ListBox.AddString(Buf);
	Num = ListBox.GetCount();
	ListBox.SetCurSel(Num-1);
}

void CKhuDaNetAppDlg::LoadMnistTrain()
{
	sprintf(m_TrainImagePath, "%s\\train-images.idx3-ubyte", m_ExePath);
	sprintf(m_TrainLabelPath, "%s\\train-labels.idx1-ubyte", m_ExePath);

	int i;

	if(!m_MnistTrainInput){
		m_MnistTrainInput = new double *[m_nMnistTrainCnt];

		for(i = 0 ; i < m_nMnistTrainCnt ; i++)
			m_MnistTrainInput[i] = new double[28*28];
	}

	if(!m_MnistTrainOutput)
		m_MnistTrainOutput = new int [m_nMnistTrainCnt];


	FILE *fp = fopen(m_TrainImagePath, "rb");
	if(fp)
	{
		unsigned char Buf[28*28];
		fread(Buf, 1, 16, fp);

		int nCnt = 0;
		for(i = 0 ; i < m_nMnistTrainCnt ; ++i)
		{
			fread(Buf, 1, 28*28, fp);

			int k;
			for(k = 0 ; k < 28*28 ; k++)
				m_MnistTrainInput[nCnt][k] = (double)Buf[k]/255.;
			nCnt++;
		}
		fclose(fp);
	}

	fp = fopen(m_TrainLabelPath, "rb");
	if(fp)
	{
		unsigned char Buf[32];
		fread(Buf, 1, 8, fp);

		int nCnt = 0;
		for(i = 0 ; i < m_nMnistTrainCnt ; ++i)
		{
			fread(Buf, 1, 1, fp);
			m_MnistTrainOutput[nCnt] = Buf[0];
			nCnt++;
		}
		fclose(fp);
	}
}

void CKhuDaNetAppDlg::LoadMnistTest()
{
	sprintf(m_TestImagePath, "%s\\t10k-images.idx3-ubyte", m_ExePath);
	sprintf(m_TestLabelPath, "%s\\t10k-labels.idx1-ubyte", m_ExePath);

	int i;

	if(!m_MnistTestInput){
		m_MnistTestInput = new double *[m_nMnistTestCnt];

		for(i = 0 ; i < m_nMnistTestCnt ; i++)
			m_MnistTestInput[i] = new double[28*28];
	}

	if(!m_MnistTestOutput)
		m_MnistTestOutput = new int [m_nMnistTestCnt];

	FILE *fp = fopen(m_TestImagePath, "rb");
	if(fp)
	{
		unsigned char Buf[28*28];
		fread(Buf, 1, 16, fp);

		int nCnt = 0;
		for(i = 0 ; i < m_nMnistTestCnt ; ++i)
		{
			fread(Buf, 1, 28*28, fp);

			int k;
			for(k = 0 ; k < 28*28 ; k++)
				m_MnistTestInput[nCnt][k] = (double)Buf[k]/255.;
			nCnt++;
		}
		fclose(fp);
	}

	fp = fopen(m_TestLabelPath, "rb");
	if(fp)
	{
		unsigned char Buf[32];
		fread(Buf, 1, 8, fp);

		int nCnt = 0;
		for(i = 0 ; i < m_nMnistTestCnt ; ++i)
		{
			fread(Buf, 1, 1, fp);
			m_MnistTestOutput[nCnt] = Buf[0];
			nCnt++;
		}
		fclose(fp);
	}
}

void CKhuDaNetAppDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if(m_MnistTrainInput){
		for(int i = 0 ; i < m_nMnistTrainCnt ; ++i)
			delete [] m_MnistTrainInput[i];

		delete [] m_MnistTrainInput;
	}

	if(m_MnistTrainOutput)
		delete [] m_MnistTrainOutput;

	if(m_MnistTestInput){
		for(int i = 0 ; i < m_nMnistTestCnt ; ++i)
			delete [] m_MnistTestInput[i];

		delete [] m_MnistTestInput;
	}

	if(m_MnistTestOutput)
		delete [] m_MnistTestOutput;
}

void CKhuDaNetAppDlg::OnBnClickedTrain()
{
	UpdateData(TRUE);

	if(!m_bRunMnistTrain)
	{
		m_nEpochCnt = 0;
		m_nBatchCnt = 0;

		if(!m_Network.IsNetwork())
		{
			m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_INPUT | KDN_LT_FC, 0, 28*28, 0, 0, 0, 0, 0.15));
			m_Network.AddLayer(CKhuDaNetLayerOption(KDN_LT_FC | KDN_LT_OUTPUT, 0, 1, 0, 0, 0, KDN_AF_SIGMOID, 0.15));

			m_Network.InitWeight();
			
			DlgPrintf(m_MsgList3, _T("%s"), m_Network.GetInformation());
		}

		SetTimer(TIMER_ID_MNIST_TRAIN, 10, NULL);

		m_bRunMnistTrain = true;
	}
	else
	{
		KillTimer(TIMER_ID_MNIST_TRAIN);

		m_bRunMnistTrain = false;
	}
}

void CKhuDaNetAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	int nBatch = GetDlgItemInt(IDC_BATCH_SIZE);

	if(nBatch <= 0) 
	{
		nBatch = 1;
		SetDlgItemInt(IDC_BATCH_SIZE, nBatch);
	}

	if(nIDEvent == TIMER_ID_MNIST_TRAIN)
	{
		int i, j;
		int nIndex = (m_nBatchCnt*nBatch)%m_nMnistTrainCnt;

		if(nIndex+nBatch >= m_nMnistTrainCnt)
			nIndex = m_nMnistTrainCnt-nBatch;

		int nOutputCnt = 1;

		double **OutputList = new double*[nBatch];
		for(i = 0 ; i < nBatch ; ++i)
			OutputList[i] = new double[nOutputCnt];

		for(i = 0 ; i < nBatch ; ++i)
		{
			for(j = 0 ; j < nOutputCnt ; ++j)
			{
				OutputList[i][j] = 0;
				if(m_MnistTrainOutput[nIndex+i] > 4) OutputList[i][j] = 1;
			}
		}

		double Loss;
		int nTP = m_Network.TrainBatch(m_MnistTrainInput+nIndex, OutputList, nBatch, &Loss);

		for(i = 0 ; i < nBatch ; ++i)
			delete [] OutputList[i];
		delete [] OutputList;

		m_nBatchCnt++;

		DlgPrintf(m_MsgList1, _T("Train accuracy: %6.2lf, %5.3lf (batch index: %5d, total : %6d(%5.1lf), epoch: %2d"), (double)nTP/(double)nBatch*100, Loss, m_nBatchCnt, nIndex+nBatch, (double)(nIndex+nBatch)/m_nMnistTrainCnt*100, m_nEpochCnt+1);

		if(nIndex+nBatch == m_nMnistTrainCnt)
		{
			m_nEpochCnt++;

			int nTP = 0;
			int i;

			for(i = 0 ; i < m_nMnistTestCnt ; i++)
			{
				int nResult = m_Network.Forward(m_MnistTestInput[i]);
				if((m_MnistTestOutput[i]>4?1:0) == nResult) nTP++;
			}

			DlgPrintf(m_MsgList2, _T("Test accuracy: %7.3lf"), (double)nTP/(double)m_nMnistTestCnt*100.);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CKhuDaNetAppDlg::OnBnClickedSave()
{
	CFileDialog dlg(FALSE, "*.kdn", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "KhuDaNet Files (*.kdn)| *.kdn|All Files (*.*)|*.*||", this);
	
	if(dlg.DoModal() == IDOK)
	{
		m_Network.SaveKhuDaNet(dlg.GetPathName().GetBuffer());
	}
}


void CKhuDaNetAppDlg::OnBnClickedLoad()
{
	if(!m_bRunMnistTrain)
	{
		CFileDialog dlg(TRUE, "*.kdn", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "KhuDaNet Files (*.kdn)| *.kdn|All Files (*.*)|*.*||", this);

		if(dlg.DoModal() == IDOK)
		{
			m_Network.LoadKhuDaNet(dlg.GetPathName().GetBuffer());

			DlgPrintf(m_MsgList3, _T("%s"), m_Network.GetInformation());
		}
	}
}


void CKhuDaNetAppDlg::OnBnClickedReset()
{
	if(!m_bRunMnistTrain)
		m_Network.ClearAllLayers();
}


void CKhuDaNetAppDlg::OnBnClickedRandomTest()
{
	static bool bInitRand = false;

	if(bInitRand)
	{
		srand((unsigned)time(0));
	}

	int nSel = rand()%m_nMnistTestCnt;
	
	if(m_Network.IsNetwork())
	{
		char strResult[100];

		int nResult = m_Network.Forward(m_MnistTestInput[nSel]);
		if((m_MnistTestOutput[nSel]>4?1:0) == nResult) 
		{
			if(nResult < 1)
				sprintf(strResult, "Result: %d < 5 (true)", m_MnistTestOutput[nSel]);
			else
				sprintf(strResult, "Result: %d > 4 (true)", m_MnistTestOutput[nSel]);
		}
		else
		{
			if(nResult < 1)
				sprintf(strResult, "Result: %d < 5 (false)", m_MnistTestOutput[nSel]);
			else
				sprintf(strResult, "Result: %d > 4 (false)", m_MnistTestOutput[nSel]);
		}

		SetDlgItemText(IDC_TEST_RESULT, strResult);

		CClientDC dc(AfxGetMainWnd());

		for(int y = 0 ; y < 28 ; ++y)
			for(int x = 0 ; x < 28 ; ++x)
			{
				COLORREF c = RGB((int)(m_MnistTestInput[nSel][y*28+x]*255.), (int)(m_MnistTestInput[nSel][y*28+x]*255.), (int)(m_MnistTestInput[nSel][y*28+x]*255.));

				dc.SetPixelV(CPoint(270+x+15, y+15), c);
			}
	}
}
