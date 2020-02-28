
// JPEGDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CJPEGDlg 对话框




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


// CJPEGDlg 消息处理程序

BOOL CJPEGDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//读取jpeg图片
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

	//解压
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
	//RGB24转RGB32
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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CJPEGDlg::OnPaint()
{
	HDC hDstMemDc;
	HBITMAP hDstBmp;
	HBITMAP hDstOldBmp;

	HDC hSrcMemDc;
	HBITMAP hSrcBmp;
	HBITMAP hSrcOldBmp;

	//创建目标内存dc和位图
	HDC hdc2 = ::GetDC(m_hWnd);
	hSrcMemDc = CreateCompatibleDC(NULL);
	hSrcBmp = CreateCompatibleBitmap(hdc2, m_w, m_h);

	//将数据存入原位图中
	SetBitmapBits(hSrcBmp, m_Len, buf);

	//将原位图绘制到目标位图
	hSrcOldBmp = (HBITMAP)SelectObject(hSrcMemDc, hSrcBmp);
	SetStretchBltMode(hdc2, STRETCH_HALFTONE);
	StretchBlt(hdc2, 0, 0, 240, 180, hSrcMemDc, 0, 0, m_w, m_h, SRCCOPY);
	SelectObject(hSrcMemDc, hSrcOldBmp);

	//释放源内存dc和位图
	DeleteObject(hSrcBmp);
	DeleteDC(hSrcMemDc);
	::ReleaseDC(m_hWnd, hdc2);

	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CJPEGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

