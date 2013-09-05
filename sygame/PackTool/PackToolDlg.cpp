// PackToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PackTool.h"
#include "PackToolDlg.h"
#include "PngPack.h"
#include "des.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPackToolDlg 对话框




CPackToolDlg::CPackToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPackToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPackToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPackToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CPackToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OUT_PUT, &CPackToolDlg::OnBnClickedButtonOutPut)
	ON_BN_CLICKED(IDC_BUTTON_UP_ACK, &CPackToolDlg::OnBnClickedButtonUpAck)
	ON_BN_CLICKED(IDC_BUTTON_ENCODE, &CPackToolDlg::OnBnClickedButtonEncode)
END_MESSAGE_MAP()


// CPackToolDlg 消息处理程序

BOOL CPackToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPackToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPackToolDlg::OnPaint()
{
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPackToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString * SplitString(CString str, char split, int& iSubStrs)
{
    int iPos = 0; //分割符位置
    int iNums = 0; //分割符的总数
    CString strTemp = str;
    CString strRight;
    //先计算子字符串的数量
    while (iPos != -1)
    {
        iPos = strTemp.Find(split);
        if (iPos == -1)
        {
            break;
        }
        strRight = strTemp.Mid(iPos + 1, str.GetLength());
        strTemp = strRight;
        iNums++;
    }
    if (iNums == 0) //没有找到分割符
    {
        //子字符串数就是字符串本身
        CString* pStrSplit = new CString[1];
		pStrSplit[1] = str;
        return pStrSplit;
    }
    //子字符串数组
    iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1
    CString* pStrSplit;
    pStrSplit = new CString[iSubStrs];
    strTemp = str;
    CString strLeft;
    for (int i = 0; i < iNums; i++)
    {
        iPos = strTemp.Find(split);
        //左子串
        strLeft = strTemp.Left(iPos);
        //右子串
        strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
        strTemp = strRight;
        pStrSplit[i] = strLeft;
    }
    pStrSplit[iNums] = strTemp;
    return pStrSplit;
}
std::string CPackToolDlg::getPngName(CString filePath)
{
	CString* pStr = NULL;
	int iSubStrs = 0;
	pStr = SplitString(filePath, '\\', iSubStrs);
	//输出所有子字符串
	for (int i = 0; i < iSubStrs; i++)
	{
		//Convert CString to char
		const char* pCh = pStr[i].GetString();
		printf("%s\n", pCh);
	}
	if (iSubStrs >= 1)
	{
		filePath = pStr[iSubStrs - 1].GetString();
	}
	if (pStr)
		delete []pStr;
	return filePath.GetString();
}
void CPackToolDlg::OnBnClickedOk()
{
	
	CTreeCtrl* pNMTreeView = reinterpret_cast<CTreeCtrl*>(GetDlgItem(IDC_TREE_FILES));
	CString strText=_T("");
	pNMTreeView->DeleteAllItems();
    HTREEITEM hRoot=pNMTreeView->InsertItem(_T("Root"));//插入树根
	
	TCHAR szFilter[] = _T("图片(*.png)|*.png|所有文件(*.*)|*.*||");   
    // 构造打开文件对话框   
    CFileDialog openDlg(TRUE, _T("png"), NULL, OFN_ALLOWMULTISELECT, szFilter, this);   
    const int nMaxFiles = 1000;  
	const int nMaxPathBuffer = (nMaxFiles * (MAX_PATH + 1)) + 1;  
	char* pc = (char*)malloc(nMaxPathBuffer * sizeof(CHAR));  
	if( pc )  
	{
		openDlg.GetOFN().lpstrFile = pc;                            
		openDlg.GetOFN().lpstrFile[0] = NULL;
		openDlg.GetOFN().nMaxFile = nMaxPathBuffer;
	}

	if( openDlg.DoModal() == IDOK )  
	{  
		POSITION posStart = openDlg.GetStartPosition();  
		while( posStart )  
		{ 
			CString filePath = openDlg.GetNextPathName(posStart);  
			pngs.push_back(filePath.GetString());
			filePath = getPngName(filePath.GetString()).c_str();
			pNMTreeView->InsertItem(filePath,hRoot);
		}  
	}  
	if (pc)
		free(pc); 
}
void CPackToolDlg::OnBnClickedButtonOutPut()
{
	TCHAR szFilter[] = _T("打包文件(*.pack)|所有文件(*.*)|*.*||");   
    // 构造保存文件对话框   
    CFileDialog fileDlg(FALSE, _T("pack"), _T("temp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);   
    CString strFilePath;   
  
    // 显示保存文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“保存”按钮，则将选择的文件路径显示到编辑框里   
        strFilePath = fileDlg.GetPathName();   
		GetDlgItem(IDC_STATIC_PACK_NAME)->SetWindowTextA(getPngName(strFilePath.GetString()).c_str());
		PngPack pack;
		for (unsigned int i = 0; i < pngs.size();i++)
		{
			std::string &tempname = pngs[i];
			std::string pngName = getPngName(tempname.c_str());
			pack.addPng(tempname.c_str(),pngName.c_str());
		}
		if (pngs.size())
			pack.save(strFilePath.GetString());
    }   
}

void CPackToolDlg::OnBnClickedButtonUpAck()
{
	TCHAR szFilter[] = _T("图片(*.pack)|*.pack|所有文件(*.*)|*.*||");   
    // 构造打开文件对话框   
    CFileDialog openDlg(TRUE, _T("pack"), NULL, NULL, szFilter, this);   
    CTreeCtrl* pNMTreeView = reinterpret_cast<CTreeCtrl*>(GetDlgItem(IDC_TREE_FILES));
	CString strText=_T("");
	pNMTreeView->DeleteAllItems();
    HTREEITEM hRoot=pNMTreeView->InsertItem(_T("Root"));//插入树根

	if( openDlg.DoModal() == IDOK )  
	{  
		CString strFilePath = openDlg.GetPathName(); 
		PngPack pack;
		pack.load(strFilePath.GetString());
	//	pack.saveAllTextures();
		for (std::map<std::string,ObjectData>::iterator iter = pack.objects.begin(); iter != pack.objects.end();++iter)
		{
			pNMTreeView->InsertItem(iter->first.c_str(),hRoot);
		}
		pack.unpack();
		GetDlgItem(IDC_STATIC_PACK_NAME)->SetWindowTextA("解包成功");
	}  
}

void CPackToolDlg::OnBnClickedButtonEncode()
{
	for (unsigned int i = 0; i < pngs.size();i++)
	{
		std::string &tempname = pngs[i];
		std::string pngName = getPngName(tempname.c_str());
		FILE *fp = fopen(pngName.c_str(), "rb");

        fseek(fp,0,SEEK_END);
        unsigned long pSize = ftell(fp);
        fseek(fp,0,SEEK_SET);
        unsigned char *pBuffer = new unsigned char[pSize];
        pSize = fread(pBuffer,sizeof(unsigned char), pSize,fp);
        fclose(fp);

		CEncrypt encode;
		encode.setEncMethod(CEncrypt::ENCDEC_DES);
		const_ZES_cblock key = {1,0,0,1,0,1,1};
		encode.set_key_des(&key);
		encode.encdec(pBuffer,pSize,true);

		fp = fopen(pngName.c_str(),"wb");
		fwrite(pBuffer,pSize,1,fp);
		fclose(fp);
	}
}
