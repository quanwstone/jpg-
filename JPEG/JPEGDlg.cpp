
// JPEGDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JPEG.h"
#include "JPEGDlg.h"
#include "afxdialogex.h"
extern "C"
{
#include"jpg/JPEGLIB.h"
};
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CJPEGDlg �Ի���




CJPEGDlg::CJPEGDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJPEGDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJPEGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CJPEGDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CJPEGDlg ��Ϣ�������

BOOL CJPEGDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	//��ȡjpegͼƬ
	FILE *pSrc = nullptr;
	
	errno_t t = fopen_s(&pSrc,"./qxiu.jpg","rb+");
	if(t != 0 )
	{
		AfxMessageBox(L"1");
	}

	struct jpeg_decompress_struct info;
	struct jpeg_error_mgr jerr;
	memset(&info,0,sizeof(jpeg_decompress_struct));
	memset(&jerr,0,sizeof(jpeg_error_mgr));

	info.err = jpeg_std_error(&jerr);
	
	jpeg_create_decompress(&info);

	jpeg_stdio_src(&info,pSrc);

	jpeg_read_header(&info,TRUE);

	BYTE *data = new BYTE [info.image_height*info.image_width*info.num_components];

	//��ѹ
	jpeg_start_decompress(&info);

	JSAMPROW row_pointer[1];

	while(info.output_scanline < info.output_height)
	{

		row_pointer[0] = &data[(info.output_height - info.output_scanline-1)*info.image_width *info.num_components];
		jpeg_read_scanlines(&info,row_pointer,1);
	}

	jpeg_finish_decompress(&info);

	jpeg_destroy_decompress(&info);
	buf = data;
	m_w = info.image_width;
	m_h = info.image_height;
	m_Len = info.image_height*info.image_width*info.num_components;

	BYTE *pRBG = new BYTE[m_w *m_h * 4];
	BYTE *p32 = pRBG;
	BYTE *p24 = data;
	//RGB24תRGB32
	for(int i = m_h - 1; i >= 0 ; i--)
	{
		BYTE* pDst = pRBG + i * m_w * 4;
		for(int j = 0; j < m_w; j++)
		{

			
			pDst[0] = p24[2];
			pDst[1] = p24[1];
			pDst[2] = p24[0];
			pDst[3] = 0;

			pDst += 4;
			p24 += 3;
		}
	}
	m_Len = m_w *m_h * 4;
	buf = pRBG;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CJPEGDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CJPEGDlg::OnPaint()
{
	HDC hDstMemDc;
	HBITMAP hDstBmp;
	HBITMAP hDstOldBmp;

	HDC hSrcMemDc;
	HBITMAP hSrcBmp;
	HBITMAP hSrcOldBmp;

	//����Ŀ���ڴ�dc��λͼ
	HDC hdc2 = ::GetDC(m_hWnd);
	hSrcMemDc = CreateCompatibleDC(NULL);
	hSrcBmp = CreateCompatibleBitmap(hdc2, m_w, m_h);

	//�����ݴ���ԭλͼ��
	SetBitmapBits(hSrcBmp, m_Len, buf);

	//��ԭλͼ���Ƶ�Ŀ��λͼ
	hSrcOldBmp = (HBITMAP)SelectObject(hSrcMemDc, hSrcBmp);
	SetStretchBltMode(hdc2, STRETCH_HALFTONE);
	StretchBlt(hdc2, 0, 0, 240, 180, hSrcMemDc, 0, 0, m_w, m_h, SRCCOPY);
	SelectObject(hSrcMemDc, hSrcOldBmp);

	//�ͷ�Դ�ڴ�dc��λͼ
	DeleteObject(hSrcBmp);
	DeleteDC(hSrcMemDc);
	::ReleaseDC(m_hWnd, hdc2);

	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CJPEGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

