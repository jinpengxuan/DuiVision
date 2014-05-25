// DlgBase.cpp
#include "stdafx.h"
#include <winuser.h>
#include <Windows.h>
#include <Windowsx.h>
#include <shlwapi.h>
#include <cmath>
#include "DlgBase.h"

IMPLEMENT_DYNAMIC(CDlgBase, CDialog)

CDlgBase::CDlgBase(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
: CDialog(nIDTemplate, pParent)
{
	m_nMainThreadId = ::GetCurrentThreadId();
	m_nIDTemplate = nIDTemplate;
	m_pParentDuiObject = NULL;
	m_strXmlFile = _T("");
	m_strXmlContent = _T("");
	m_strTitle = _T("");
	m_hIcon = NULL;
	m_bAppWin = FALSE;
	m_MinSize.cx = 0;
	m_MinSize.cy = 0;
	m_bChangeSize = FALSE;
	m_bInit = false;

	m_nFrameTopBottomSpace = 3;
	m_nFrameLeftRightSpace = 3;

	m_nOverRegioX = 100;
	m_nOverRegioY = 100;
	m_sizeBKImage.cx = 100;
	m_sizeBKImage.cy = 100;

	m_bTracking = false;
	m_bIsSetCapture = false;
	m_clrBK = RGB(186, 226, 239);
	m_bDrawImage = FALSE;

	m_bIsLButtonDown = FALSE;
	m_bIsLButtonDblClk = FALSE;
	m_pOldMemBK = NULL;
	m_pControl = NULL;
	m_pFocusControl = NULL;
	m_pWndPopup = NULL;

	m_strFramePicture = _T("");
	m_nFrameSize = 3;
	m_nFrameWLT = 0;
	m_nFrameHLT = 0;
	m_nFrameWRB = 0;
	m_nFrameHRB = 0;

	m_uTimerAnimation = 0;
	m_uTimerAutoClose = 0;

	m_bAutoClose = FALSE;
	m_bAutoHide = FALSE;
	m_uAutoCloseDelayTime = 0;

	m_strBkImg = _T("");
	m_crlBack = RGB(0,0,0);

	m_nTooltipCtrlID = 0;

	m_pTrayHandler = NULL;
	m_strTrayMenuXml = _T("");

	// ��ʼ��ͼ��
	CStringA strTrayIcon = DuiSystem::Instance()->GetSkin("IDB_TRAY_ICON");
	if(!strTrayIcon.IsEmpty())
	{
		if(strTrayIcon.Find(".") != -1)	// ����ͼ���ļ�
		{
			CString strIconFile = DuiSystem::GetSkinPath() + CEncodingUtil::AnsiToUnicode(strTrayIcon);
			WORD wIndex = 0;
			m_hIcon = ::ExtractAssociatedIcon(NULL, strIconFile.GetBuffer(0), &wIndex);
		}else	// ����ͼ����Դ
		{
			UINT nResourceID = atoi(strTrayIcon);
			m_hIcon = AfxGetApp()->LoadIcon(nResourceID);
		}
	}
}

CDlgBase::~CDlgBase()
{
	// ������ʱ��
	CTimer::KillTimer();

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

	if(m_BKImage.m_hObject != NULL)
	{
		m_BKImage.DeleteObject();
		m_MemBKDC.SelectObject(m_pOldMemBK);
		m_MemBK.DeleteObject();
	}

	if(m_TitleFont.m_hObject != NULL)
	{
		m_TitleFont.DeleteObject();
	}
	m_Rgn.DeleteObject();

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			delete pControlBase;
		}		
	}
}

BEGIN_MESSAGE_MAP(CDlgBase, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
	ON_WM_NCACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_KEYDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER_CLOSEWND, OnUserCloseWindow)
	ON_MESSAGE(WM_SKIN, OnMessageSkin)
	ON_MESSAGE(WM_UI_TASK, OnMessageUITask)
	ON_MESSAGE(WM_SYSTEM_TRAYICON, OnSystemTrayIcon)
	ON_REGISTERED_MESSAGE(WM_CHECK_ITS_ME, OnCheckItsMe)
END_MESSAGE_MAP()

// ������С���ڴ�С
void CDlgBase::SetMinSize(int iWidth, int iHeight)
{
	m_MinSize.cx = iWidth;
	m_MinSize.cy = iHeight;
}

CSize CDlgBase::GetMinSize()
{
	return	m_MinSize;
}

void CDlgBase::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = m_MinSize.cx;
	lpMMI->ptMinTrackSize.y = m_MinSize.cy;

	CRect	rc(0, 0, 1024, 768);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	lpMMI->ptMaxPosition.x = rc.left;
	lpMMI->ptMaxPosition.y = rc.top;
	lpMMI->ptMaxSize.x = rc.Width();
	lpMMI->ptMaxSize.y = rc.Height();
}

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDlgBase::_CreateControlByName(LPCSTR lpszName)
{
	return DuiSystem::CreateControlByName(lpszName, GetSafeHwnd(), this);
}

// �жϵ�ǰ�Ƿ������߳�
void CDlgBase::TestMainThread()
{
    // ���㿴�����������ʱ���Ҳ��ҵĸ����㣬��������߳���ˢ����
    // ����һ����Σ�յ�����
    DWORD dwCurThreadID = GetCurrentThreadId();
    BOOL bOK = (m_nMainThreadId == dwCurThreadID); // ��ǰ�̺߳͹������ʱ���߳�һ��
	if(!bOK)
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("TestMainThread failed"));
	}
    ASSERT(bOK);
}

// CDlgBase message handlers

BOOL CDlgBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ���ش��ڵ�XML�ļ�
	TiXmlDocument xmlDoc;
	TiXmlElement* pDlgElem = NULL;
	//xmlDoc.Parse(strXml, NULL, TIXML_ENCODING_UTF8);

	if(!m_strXmlFile.IsEmpty())	// ����XML�ļ�
	{
		if(m_strXmlFile.Find(_T("xml:")) == 0)
		{
			m_strXmlFile.Delete(0, 4);
			m_strXmlFile = DuiSystem::GetXmlPath() + m_strXmlFile;
		}
		//CStringA strFile = CT2A(m_strXmlFile, CP_UTF8);
		xmlDoc.LoadFile(CEncodingUtil::UnicodeToAnsi(m_strXmlFile), TIXML_ENCODING_UTF8);
	}else
	if(!m_strXmlContent.IsEmpty())	// ����XML����
	{
		xmlDoc.Parse(CT2A(m_strXmlContent, CP_UTF8), NULL, TIXML_ENCODING_UTF8);
	}

	if(!xmlDoc.Error())
	{
		pDlgElem = xmlDoc.FirstChildElement("dlg");//RootElement();
		if(pDlgElem != NULL)
		{
			// ����dlg�ڵ�����
			Load(pDlgElem);
			// ���´��ڴ�С
			SetMinSize(m_MinSize.cx, m_MinSize.cy);
			SetRect(CRect(0, 0, m_MinSize.cx, m_MinSize.cy));
		}
	}

	// ��ʼ���Ի���������Ԥ��ֵ����
	InitDialogValue();

	SetWindowText(m_strTitle);

	CFont	tmpFont;
	tmpFont.Attach(GetStockObject(DEFAULT_GUI_FONT));

	LOGFONT font;
	memset(&font, 0, sizeof(font));
	tmpFont.GetLogFont(&font);

	CWindowDC dc(this);
	wcscpy(font.lfFaceName,DuiSystem::GetDefaultFont());
	font.lfHeight = -10 * GetDeviceCaps(dc.m_hDC, LOGPIXELSY) / 72;
	font.lfWeight = 600;

	m_TitleFont.CreateFontIndirect(&font);

	::SetWindowPos(m_hWnd, NULL, 0, 0, m_MinSize.cx, m_MinSize.cy, SWP_HIDEWINDOW | SWP_NOMOVE);
	
	CRect	rc;
	GetClientRect(rc);

	// ʹ��XML�ڵ��ʼ�����ڻ����ؼ�����ͨ�ؼ�
	if(pDlgElem != NULL)
	{
		TiXmlElement* pBaseElem = pDlgElem->FirstChildElement("base");
		if(pBaseElem != NULL)
		{
			InitBaseUI(rc, pBaseElem);
		}

		TiXmlElement* pBodyElem = pDlgElem->FirstChildElement("body");
		if(pBodyElem != NULL)
		{
			InitUI(rc, pBodyElem);
		}

		// ��ʼ��Ԥ��ֵ�Ŀؼ�ֵ
		InitControlValue();
	}

	if(!m_bAppWin)
	{
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	}

	//���ر���ͼ
	if(!m_strBkImg.IsEmpty())	// ������������˱���ͼƬ���ԣ����ô˱���ͼƬ
	{
		// ͨ��Skin��ȡ
		CString strBkSkin = _T("");
		if(m_strBkImg.Find(_T("skin:")) == 0)
		{
			strBkSkin = CEncodingUtil::AnsiToUnicode(DuiSystem::Instance()->GetSkin(CEncodingUtil::UnicodeToAnsi(m_strBkImg)));
		}else
		{
			strBkSkin = m_strBkImg;
		}

		if(strBkSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
		{
			CString strImgFile = DuiSystem::GetSkinPath() + strBkSkin;
			LoadImage(strImgFile);
		}else	// ����ͼƬ��Դ
		{
			UINT nResourceID = _wtoi(strBkSkin);
			LoadImage(nResourceID, TEXT("PNG"));
		}
	}else
	if(m_crlBack != RGB(0,0,0))	// ������������˱�����ɫ���ԣ����ô˱�����ɫ
	{
		DrawBackground(m_crlBack);
	}else
	{
		// ����DuiSystem��ȡ������Ϣ������
		InitWindowBkSkin();
	}

	CenterWindow();
	ShowWindow(SW_SHOW);

	//������ʱ��
	m_uTimerAnimation = CTimer::SetTimer(30);

	// ��ʼ��Tooltip
	m_wndToolTip.Create(this);

	m_bInit = true;

	// �����¼���������ĳ�ʼ������
	if(m_pDuiHandler)
	{
		m_pDuiHandler->OnInit();
	}

	return TRUE;
}

// ���ش��ڻ����ؼ�
void CDlgBase::InitBaseUI(CRect rcClient, TiXmlElement* pNode)
{
	if(pNode)
	{
		TiXmlElement* pControlElem = NULL;
		for (pControlElem = pNode->FirstChildElement(); pControlElem != NULL; pControlElem=pControlElem->NextSiblingElement())
		{
			if(pControlElem != NULL)
			{
				CStringA strControlName = pControlElem->Value();
				CControlBase* pControl = _CreateControlByName(strControlName);
				if(pControl)
				{
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()) || pControl->IsClass(CFrame::GetClassName()))
					{
						// Area��Frame������Ӧ���,����ӵ�Area�б���
						m_vecBaseArea.push_back(pControl);
					}else
					{
						m_vecBaseControl.push_back(pControl);
					}
				}
			}
		}
	}
}

// ��ʼ�����ڿؼ�
void CDlgBase::InitUI(CRect rcClient, TiXmlElement* pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// �����ڵ�͸���Ƚ�����ɰ�(���ݴ��ڵ�frame����������)
	if(!m_strFramePicture.IsEmpty())
	{
		pControlBase = new CDuiPicture(GetSafeHwnd(), this, FRAME_MAINWND, rcClient);
		((CDuiPicture*)pControlBase)->OnAttributeImage(CEncodingUtil::UnicodeToAnsi(m_strFramePicture), FALSE);
		if(m_nFrameWLT != 0)
		{
			// �Ź���ģʽ
			((CDuiPicture*)pControlBase)->SetShowModeMID(m_nFrameWLT, m_nFrameHLT, m_nFrameWRB, m_nFrameHRB);
		}else
		{
			// �߿�ģʽ
			((CDuiPicture*)pControlBase)->SetShowMode(enSMFrame, m_nFrameSize);
		}
		pControlBase->OnAttributePosChange("0,0,-0,-0", FALSE);
		m_vecBaseArea.push_back(pControlBase);
	}

	// �������д��ڿؼ�
	if(pNode)
	{
		TiXmlElement* pControlElem = NULL;
		for (pControlElem = pNode->FirstChildElement(); pControlElem != NULL; pControlElem=pControlElem->NextSiblingElement())
		{
			if(pControlElem != NULL)
			{
				CStringA strControlName = pControlElem->Value();
				CControlBase* pControl = _CreateControlByName(strControlName);
				if(pControl)
				{
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()) || pControl->IsClass(CFrame::GetClassName()))
					{
						// Area��Frame������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
						// �Ի����ʼ��ʱ����ʾһ�����пؼ�������ʹedit��Windowsԭ���ؼ������Զ���������
						//BOOL bIsVisible = pControl->GetVisible();
						//pControl->SetControlWndVisible(bIsVisible);
					}
				}
			}
		}
	}
}

// ���ӿؼ�Ԥ������Ϣ
void CDlgBase::SetControlValue(CString strName, CString strType, CString strValue)
{
	CONTROL_VALUE ctrlValue;
	ctrlValue.strName = strName;
	ctrlValue.strType = strType;
	ctrlValue.strValue = strValue;
	m_vecControlValue.push_back(ctrlValue);
}

// ��ʼ��Ԥ���öԻ�������
void CDlgBase::InitDialogValue()
{
	for (size_t i = 0; i < m_vecControlValue.size(); i++)
	{
		CONTROL_VALUE* pCtrlValue = &(m_vecControlValue.at(i));

		// �Ի����������Ը���
		if(pCtrlValue->strName.IsEmpty())
		{
			if(pCtrlValue->strType == _T("width"))
			{
				m_MinSize.cx = _wtoi(pCtrlValue->strValue);
				// ���´��ڴ�С
				SetMinSize(m_MinSize.cx, m_MinSize.cy);
				SetRect(CRect(0, 0, m_MinSize.cx, m_MinSize.cy));
			}else
			if(pCtrlValue->strType == _T("height"))
			{
				m_MinSize.cy = _wtoi(pCtrlValue->strValue);
				// ���´��ڴ�С
				SetMinSize(m_MinSize.cx, m_MinSize.cy);
				SetRect(CRect(0, 0, m_MinSize.cx, m_MinSize.cy));
			}
		}
	}
}

// ��ʼ��Ԥ���ÿؼ�ֵ
void CDlgBase::InitControlValue()
{
	for (size_t i = 0; i < m_vecControlValue.size(); i++)
	{
		CONTROL_VALUE* pCtrlValue = &(m_vecControlValue.at(i));

		// �Ի����������Ը���
		if(pCtrlValue->strName.IsEmpty())
		{
			continue;
		}

		CControlBase* pControl = GetControl(pCtrlValue->strName);
		if(pControl == NULL)
		{
			// �������ͨ�ؼ���δ�ҵ�,����һ�»����ؼ�
			pControl = GetBaseControl(pCtrlValue->strName);
		}
		if(pControl != NULL)
		{
			if(pCtrlValue->strType == _T("visible"))
			{
				pControl->SetVisible(_wtoi(pCtrlValue->strValue));
			}else
			if(pCtrlValue->strType == _T("disable"))
			{
				pControl->SetDisable(_wtoi(pCtrlValue->strValue));
			}else
			if(pCtrlValue->strType == _T("title"))
			{
				((CControlBaseFont*)pControl)->SetTitle(pCtrlValue->strValue);
			}else
			if(pCtrlValue->strType == _T("image"))
			{
				((CControlBaseFont*)pControl)->SetImage(pCtrlValue->strValue);
			}else
			if(pCtrlValue->strType == _T("check"))
			{
				((CCheckButton*)pControl)->SetCheck(pCtrlValue->strValue == _T("true"));
			}
		}
	}
}

// ���ô����Զ��رյĶ�ʱ��
void CDlgBase::SetAutoCloseTimer(UINT uDelayTime, BOOL bHideWnd)
{
	// ��ɾ����ʱ��,�ٴ����µĶ�ʱ��
	if(m_uTimerAutoClose != 0)
	{
		CTimer::KillTimer(m_uTimerAutoClose);
		m_uTimerAutoClose = 0;
	}
	m_uAutoCloseDelayTime = uDelayTime;
	m_bAutoClose = FALSE;
	m_bAutoHide = FALSE;
	if(uDelayTime != 0)
	{
		// ����ӳ�ʱ�����õ��Ƿ�0,�������Զ��ر�
		if(bHideWnd)
		{
			m_bAutoHide = TRUE;	// ���ش���ģʽ
		}else
		{
			m_bAutoClose = TRUE;	// �رմ���ģʽ
		}
		if(!m_bTracking)
		{
			// �����겻�ڴ��ڷ�Χ�ڲŴ�����ʱ��
			m_uTimerAutoClose = CTimer::SetTimer(uDelayTime);
		}
	}
}

void CDlgBase::OnSize(CRect rcClient)
{
}

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetControl(UINT uControlID)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}else
			{
				// �����ӿؼ�
				CControlBase * pSubControl = pControlBase->GetControl(uControlID);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}else
			{
				// �����ӿؼ�
				CControlBase * pSubControl = pControlBase->GetControl(uControlID);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	return NULL;
}

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetControl(CString strControlName)
{
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}else
			{
				// �����ӿؼ�
				CControlBase * pSubControl = pControlBase->GetControl(strControlName);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}else
			{
				// �����ӿؼ�
				CControlBase * pSubControl = pControlBase->GetControl(strControlName);
				if(pSubControl != NULL)
				{
					return pSubControl;
				}
			}
		}
	}

	return NULL;
}

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetBaseControl(UINT uControlID)
{
	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetControlID() == uControlID)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ��ȡ�ӿؼ�����
CControlBase *CDlgBase::GetBaseControl(CString strControlName)
{
	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			if (pControlBase->GetName() == strControlName)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ���õ�ǰ����ؼ�
void CDlgBase::SetFocusControl(CControlBase* pFocusControl)
{
	if(pFocusControl != m_pFocusControl)
	{
		if(m_pFocusControl != NULL)
		{
			m_pFocusControl->OnFocus(FALSE);
		}
		if(pFocusControl != NULL)
		{
			pFocusControl->OnFocus(TRUE);
		}
		m_pFocusControl = pFocusControl;
	}
}

// ��ȡ��ǰ����ؼ�
CControlBase* CDlgBase::GetFocusControl()
{
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && (pControlBase == m_pFocusControl) && pControlBase->IsTabStop())
		{
			return pControlBase;
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// �����ӿؼ�
			pControlBase = pControlBase->GetFocusControl(m_pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
		}
	}

	return NULL;
}

// ��ȡ��һ�����Ի�ȡ������ӿؼ�
CControlBase* CDlgBase::GetPrevFocusableControl()
{
	BOOL bStartSearch = FALSE;
	// �Ȱ��ս���ؼ�����һ��
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// �ٱ����ӿؼ�����
	if(m_pFocusControl == NULL)
	{
		bStartSearch = TRUE;
	}
	for (int i = m_vecControl.size()-1; i >= 0; i--)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if(m_pControl == NULL)
		{
			m_pControl = pControlBase;
		}
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// �����ӿؼ�
			pControlBase = pControlBase->GetPrevFocusableControl(m_pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
			//bStartSearch = TRUE;
		}
	}

	return NULL;
}

// ��ȡ��һ�����Ի�ȡ������ӿؼ�
CControlBase* CDlgBase::GetNextFocusableControl()
{
	BOOL bStartSearch = FALSE;
	// �Ȱ��ս���ؼ�����һ��
	for (int i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pFocusControl))
		{
			bStartSearch = TRUE;
		}
	}

	// �ٱ����ӿؼ�����
	if(m_pFocusControl == NULL)
	{
		bStartSearch = TRUE;
	}
	for (int i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase* pControlBase = m_vecControl.at(i);
		if(m_pControl == NULL)
		{
			m_pControl = pControlBase;
		}
		if (pControlBase && pControlBase->GetVisible() && !pControlBase->GetDisable() && bStartSearch && pControlBase->IsTabStop())
		{
			return pControlBase;			
		}else
		if (pControlBase && (pControlBase == m_pControl))
		{
			// �����ӿؼ�
			pControlBase = pControlBase->GetNextFocusableControl(m_pFocusControl);
			if(pControlBase != NULL)
			{
				return pControlBase;
			}
			//bStartSearch = TRUE;
		}
	}

	return NULL;
}

int CDlgBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// ���ô��ڷ��
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE)
		| WS_MAXIMIZEBOX | WS_MINIMIZEBOX
		| WS_SYSMENU | WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_DBLCLKS;
	dwStyle &= ~(WS_CAPTION);

	// �ı䴰�ڴ�С
	if(!m_bChangeSize)
	{
		dwStyle &= ~(WS_THICKFRAME);
	}

	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	return CDialog::OnCreate(lpCreateStruct);
}

// ��XML����resize����
HRESULT CDlgBase::OnAttributeResize(const CStringA& strValue, BOOL bLoading)
{
    if (strValue.IsEmpty()) return E_FAIL;

	// ��ȡresize���ԣ�����������һ�´��ڷ��
	m_bChangeSize = atoi(strValue);

	// ���ô��ڷ��
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE)
		| WS_MAXIMIZEBOX | WS_MINIMIZEBOX
		| WS_SYSMENU | WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_DBLCLKS;
	dwStyle &= ~(WS_CAPTION);

	// �ı䴰�ڴ�С
	if(!m_bChangeSize)
	{
		dwStyle &= ~(WS_THICKFRAME);
	}

	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	
	return bLoading?S_FALSE:S_OK;
}

// ��ʼ�����ڱ���Ƥ��
void CDlgBase::InitWindowBkSkin()
{
	// �����������������ı���,����ȫ�ֵı���
	if(!m_strBkImg.IsEmpty() || (m_crlBack != RGB(0,0,0)))
	{
		return;
	}

	// ����DuiSystem��ȡ������Ϣ
	int nType = 0;
	int nIDResource = 0;
	COLORREF clr = RGB(0,0,0);
	CString strImgFile = _T("");
	BOOL bRet = DuiSystem::Instance()->GetWindowBkInfo(nType, nIDResource, clr, strImgFile);
	if(bRet && !((nType == BKTYPE_IMAGE_RESOURCE) && (nIDResource == 0)))
	{
		if(nType == BKTYPE_IMAGE_RESOURCE)	// ͼƬ��Դ
		{
			LoadImage(nIDResource);
		}else
		if(nType == BKTYPE_COLOR)	// ��ɫ
		{
			DrawBackground(clr);
		}else
		if(nType == BKTYPE_IMAGE_FILE)	// ͼƬ�ļ�
		{
			LoadImage(strImgFile);
		}
	}else
	{
		// Ĭ�ϼ��ص�һ�ű���ͼƬ
		CString strImgFile = CEncodingUtil::AnsiToUnicode(DuiSystem::Instance()->GetSkin("SKIN_PIC_0"));
		strImgFile = DuiSystem::GetSkinPath() + strImgFile;
		LoadImage(strImgFile);
	}
}

// ��קͼƬ���´��ڱ���ͼƬ
void CDlgBase::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFileName[MAX_PATH + 1] = {0};
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for(UINT i = 0; i < nFiles; i++)
	{		
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);
		if(PathIsDirectory(szFileName))
		{
			continue;
		}	
		CString strFileName = szFileName;
		strFileName = strFileName.Right(3);
		if (0 == strFileName.CompareNoCase(TEXT("bmp")) || 0 == strFileName.CompareNoCase(TEXT("jpg")) || 0 == strFileName.CompareNoCase(TEXT("png")))
		{
			LoadImage(szFileName);
			// ���汳����Ϣ
			DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_IMAGE_FILE, 0, RGB(0,0,0), szFileName);
			// ˢ�����д��ڵı���Ƥ��
			DuiSystem::Instance()->ResetAllWindowsBkSkin();
			break;
		}
	}
	// CDialog::OnDropFiles(hDropInfo);
}

void CDlgBase::LoadImage(UINT nIDResource, CString strType)
{
	CBitmap bitBackground;
	::LoadImage(nIDResource, bitBackground, m_sizeBKImage, strType);	
	DrawBackground(bitBackground);	
}

void CDlgBase::LoadImage(CString strFileName)
{
	CBitmap bitBackground;
	::LoadImage(strFileName, bitBackground, m_sizeBKImage);	
	DrawBackground(bitBackground);	
}

// ������ͼƬ
void CDlgBase::DrawBackground(CBitmap &bitBackground)
{
	if(m_MinSize.cx - 2 > m_sizeBKImage.cx || m_MinSize.cy - 2 > m_sizeBKImage.cy || m_bChangeSize)
	{
		if(m_MinSize.cx - 2 > m_sizeBKImage.cx)
		{
			m_nOverRegioX = __min(100, m_sizeBKImage.cx - 2);
		}
		else
		{
			m_nOverRegioX = 0;
		}

		if(m_MinSize.cy - 2 > m_sizeBKImage.cy)
		{
			m_nOverRegioY = __min(100, m_sizeBKImage.cy - 2);
		}
		else
		{
			m_nOverRegioY = 0;
		}
	}
 	else
 	{
 		m_nOverRegioX = 0;
		m_nOverRegioY = 0;
 	}
	if(bitBackground.m_hObject)
	{
		m_bDrawImage = TRUE;
		int nWidth = m_sizeBKImage.cx;
		int nHeight = m_sizeBKImage.cy;

		if(m_MemBKDC.m_hDC)
		{
			m_MemBKDC.DeleteDC();
		}

		CDC *pDC = GetDC();
		
		::GetAverageColor(pDC, bitBackground, m_sizeBKImage, m_clrBK);

		m_MemBKDC.CreateCompatibleDC(pDC);

		if(m_MemBK.m_hObject)
		{
			m_MemBK.DeleteObject();
		}
		m_MemBK.CreateCompatibleBitmap(pDC, m_sizeBKImage.cx, m_sizeBKImage.cy);
		m_pOldMemBK =  m_MemBKDC.SelectObject(&m_MemBK);

		CDC TempDC;
		TempDC.CreateCompatibleDC(pDC);

		CBitmap* pOldBitmap = TempDC.SelectObject(&bitBackground);

		// ����ƽ��ͼƬ
		m_MemBKDC.FillSolidRect(0, 0, nWidth, nHeight, m_clrBK); 		

		if(m_nOverRegioX > 0 && m_nOverRegioY > 0)
		{
			int nOverRegio = __min(m_nOverRegioX, m_nOverRegioY);

			// ����
			m_MemBKDC.BitBlt(0, 0, nWidth - nOverRegio, nHeight - nOverRegio, &TempDC, 0, 0, SRCCOPY ); 

			// �м�
			CRect rc(0, 0, nWidth, nHeight);
			DrawRightBottomTransition(m_MemBKDC, TempDC, rc, nOverRegio, m_clrBK);

			// ����
			rc.SetRect(nWidth - nOverRegio, 0, nWidth, nHeight - nOverRegio);
			DrawHorizontalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);

			// ����
			rc.SetRect(0, nHeight - nOverRegio, nWidth - nOverRegio, nHeight);
			DrawVerticalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);
		}
		else if(m_nOverRegioX > 0 && m_nOverRegioY == 0)
		{
			// ���
			m_MemBKDC.BitBlt(0, 0, nWidth - m_nOverRegioX, nHeight, &TempDC, 0, 0, SRCCOPY ); 

			// �ұ�
			CRect rc(nWidth - m_nOverRegioX, 0, nWidth, nHeight);
			DrawHorizontalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);
		}
		else if(m_nOverRegioX == 0 && m_nOverRegioY > 0)
		{
			// ����
			m_MemBKDC.BitBlt(0, 0, nWidth, nHeight - m_nOverRegioY, &TempDC, 0, 0, SRCCOPY ); 

			// �±�
			CRect rc(0, nHeight - m_nOverRegioY, nWidth, nHeight);
			DrawVerticalTransition(m_MemBKDC, TempDC, rc, rc, 0, 100);
		}
		else
		{
			m_MemBKDC.BitBlt(0, 0, nWidth, nHeight, &TempDC, 0, 0, SRCCOPY ); 
		}

		TempDC.SelectObject(pOldBitmap);
		TempDC.DeleteDC();
		ReleaseDC(pDC);

		ResetControl();		
	}
}

// ������ͼƬ
void CDlgBase::DrawBackground(COLORREF clr)
{
	m_clrBK = clr;
	m_bDrawImage = FALSE;
	ResetControl();		
}

BOOL CDlgBase::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

HCURSOR CDlgBase::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDlgBase::OnPaint()
{
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
 		CRect rcUpdate;
		GetUpdateRect(&rcUpdate);

		CRect	rcClient;
 		GetClientRect(&rcClient);

		CPaintDC	dc(this);
		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap memBmp;
		memBmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		CBitmap *pOldmap =  MemDC.SelectObject(&memBmp);

		DrawImageStyle(MemDC, rcClient, rcUpdate);
		dc.BitBlt(rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(), &MemDC, rcUpdate.left, rcUpdate.top, SRCCOPY);

		MemDC.SelectObject(pOldmap);
		MemDC.DeleteDC();
		memBmp.DeleteObject();
	}
}

// �������Ϳؼ�
void CDlgBase::DrawImageStyle(CDC &dc, const CRect &rcClient, const CRect &rcUpdate)
{
	dc.FillSolidRect(rcUpdate.left, rcUpdate.top, rcUpdate.Width(), rcUpdate.Height(), m_clrBK);

	if(m_bDrawImage)
	{
		// ����
		CRect rcBk(1, 1, 1 + m_sizeBKImage.cx, 1 + m_sizeBKImage.cy);
		rcBk = rcBk & rcUpdate;
		if(!rcBk.IsRectEmpty())
		{
			dc.BitBlt(rcBk.left, rcBk.top, rcBk.Width() , rcBk.Height(), &m_MemBKDC, rcBk.left, rcBk.top, SRCCOPY ); 
		}	
	}

	// �ؼ�
	DrawControl(dc, rcUpdate);
	/*
 	int nTitle = 8;
 	// ͼ��
 	if (m_hIcon)
 	{
 		nTitle += 20;
 		DrawIconEx(dc.m_hDC, 8, 8, m_hIcon, 16, 16, 0, NULL, DI_NORMAL);
 	}
 
 	// Title ����
 	CString szTitile;
 	GetWindowText(szTitile);
 
 	CFont* pOldFont = dc.SelectObject(&m_TitleFont);
 	dc.SetBkMode(TRANSPARENT);
 	// ��Ӱ
 	dc.SetTextColor(RGB(255, 255, 255));
 	CRect	rcText(nTitle, 8, nTitle + 240, 26);
 	dc.DrawText(szTitile, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
 	// ����
 	rcText.OffsetRect(-1, -1);
 	dc.SetTextColor(RGB(0, 0, 0));
 	dc.DrawText(szTitile, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
 	dc.SelectObject(pOldFont);
	*/
}

// ���ÿؼ�
void CDlgBase::ResetControl()
{
	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);
		}
	}

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);			
		}
	}

	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetUpdate(FALSE, m_clrBK);			
		}
	}

	InvalidateRect(NULL);
}

// �ƶ��ؼ�
CControlBase * CDlgBase::SetControlRect(UINT uControlID, CRect rc)
{
	CControlBase *pControlBase = GetControl(uControlID);
	if(pControlBase)
	{
		pControlBase->SetRect(rc);
		UpdateHover();
	}
	return pControlBase;
}

// �ƶ��ؼ�
CControlBase * CDlgBase::SetControlRect(CControlBase *pControlBase, CRect rc)
{
	if(pControlBase)
	{
		pControlBase->SetRect(rc);
		UpdateHover();
	}
	return pControlBase;
}

// ��ʾ�ؼ�
CControlBase * CDlgBase::SetControlVisible(UINT uControlID, BOOL bVisible)
{
	CControlBase *pControlBase = GetControl(uControlID);
	if(pControlBase)
	{
		pControlBase->SetVisible(bVisible);
		UpdateHover();
	}
	return pControlBase;
}

// ��ʾ�ؼ�
CControlBase * CDlgBase::SetControlVisible(CControlBase *pControlBase, BOOL bVisible)
{
	if(pControlBase)
	{
		pControlBase->SetVisible(bVisible);
		UpdateHover();
	}
	return pControlBase;
}

// ���ÿؼ�
CControlBase * CDlgBase::SetControlDisable(UINT uControlID, BOOL bDisable)
{
	CControlBase *pControlBase = GetControl(uControlID);
	if(pControlBase)
	{
		pControlBase->SetDisable(bDisable);
		UpdateHover();
	}
	return pControlBase;
}

// ���ÿؼ�
CControlBase * CDlgBase::SetControlDisable(CControlBase *pControlBase, BOOL bDisable)
{
	if(pControlBase)
	{
		pControlBase->SetDisable(bDisable);
		UpdateHover();
	}
	return pControlBase;
}

// ����ѡ��
void CDlgBase::UpdateHover()
{
	CPoint point;
	GetCursorPos(&point);
	OnMouseMove(0, point);
}

void CDlgBase::DrawControl(CDC &dc, const CRect &rcUpdate)
{
	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);
		}
	}

	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);
		}
	}

	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);			
		}
	}

	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			pControlBase->Draw(dc, rcUpdate);			
		}
	}	
}

void CDlgBase::OnNcPaint()
{
}

LRESULT CDlgBase::OnNcHitTest(CPoint point)
{
	// ���ܸı䴰�ڴ�С
	if(!m_bChangeSize)
	{
		return	HTCLIENT;
	}
	CRect	rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left, -rc.top);
	ScreenToClient(&point);
	int x = point.x;
	int y = point.y;

	if ( x < m_nFrameLeftRightSpace && y < m_nFrameTopBottomSpace)
	{
		return	HTTOPLEFT;
	}
	if ( x <= m_nFrameLeftRightSpace && y >= rc.bottom - m_nFrameTopBottomSpace)
	{
		return	HTBOTTOMLEFT;
	}
	if ( x > rc.right - m_nFrameLeftRightSpace && y < m_nFrameTopBottomSpace)
	{
		return	HTTOPRIGHT;
	}
	if ( x >= rc.right - m_nFrameLeftRightSpace && y >= rc.bottom - m_nFrameTopBottomSpace)
	{
		return	HTBOTTOMRIGHT;
	}

	if ( x < m_nFrameLeftRightSpace)
	{
		return	HTLEFT;
	}
	if ( x >= rc.right - m_nFrameLeftRightSpace)
	{
		return	HTRIGHT;
	}
	if ( y < m_nFrameTopBottomSpace)
	{
		return	HTTOP;
	}
	if ( y > rc.bottom - m_nFrameTopBottomSpace)
	{
		return	HTBOTTOM;
	}
	// 	if ( y <= m_nFrameTopSpace)
	// 	{
	// 		return	HTCAPTION;
	// 	}
	return	HTCLIENT;
	//	return CDialog::OnNcHitTest(point);
}

void CDlgBase::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (!IsIconic())
	{
		BOOL bIsMaximize = IsZoomed();
		int border_offset[] = {/*3, 2, */1};
		if (bIsMaximize)
		{				
			SetupRegion(border_offset, 0);
			m_nFrameLeftRightSpace = m_nFrameTopBottomSpace = 0;
		}
		else
		{
			SetupRegion(border_offset, 1/*3*/);
			m_nFrameLeftRightSpace = m_nFrameTopBottomSpace = 3;	// ���ÿ�������϶��ı䴰�ڴ�С���������
		}	

		// �����ڵ�͸��������ɰ��ھŹ���ģʽ����������²Ż�ɼ�
		CControlBase *pControlBase = GetBaseControl(FRAME_MAINWND);
		if (pControlBase && (m_nFrameWLT == 0))
		{
			pControlBase->SetVisible(!bIsMaximize);
		}
	}

	// ���ô��ڴ�С
	CRect rc;
	GetClientRect(&rc);
	SetRect(rc);
	OnSize(rc);

	// ���������ؼ���λ��
	// ����AREA�Ĵ�С����
	for (size_t i = 0; i < m_vecBaseArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseArea.at(i);
		if (pControlBase)
		{
			pControlBase->OnPositionChange();
			UpdateHover();
		}
	}
	// �����ؼ��Ĵ�С����
	for (size_t i = 0; i < m_vecBaseControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecBaseControl.at(i);
		if (pControlBase)
		{
			pControlBase->OnPositionChange();
			UpdateHover();
		}
	}

	// �û�AREA�Ĵ�С����
	for (size_t i = 0; i < m_vecArea.size(); i++)
	{
		CControlBase * pControlBase = m_vecArea.at(i);
		if (pControlBase)
		{
			pControlBase->OnPositionChange();
			UpdateHover();
		}
	}
	// �û��ؼ��Ĵ�С����
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->OnPositionChange();
			UpdateHover();
		}
	}

	InvalidateRect(NULL);
}

// ���ô�������
void CDlgBase::SetupRegion(int border_offset[], int nSize)
{
	CDC* pDC = GetDC();

	CRect	rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left, -rc.top);

	CRgn	rgn;
	rgn.CreateRectRgn(0, 0, rc.Width(), rc.Height());
	CRgn	rgn_xor;
	CRect	rcXor;
	
	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(0, y, border_offset[y], y + 1);
		rgn_xor.CreateRectRgn(0, y, border_offset[y], y + 1);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(rc.right - border_offset[y], y, rc.right, y + 1);
		rgn_xor.CreateRectRgn(rc.right - border_offset[y], y, rc.right, y + 1);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(0, rc.bottom - y - 1, border_offset[y],  rc.bottom - y);
		rgn_xor.CreateRectRgn(0, rc.bottom - y - 1, border_offset[y],  rc.bottom - y);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}

	for (int y = 0; y < nSize; ++y)
	{
		rcXor.SetRect(rc.right - border_offset[y], rc.bottom - y - 1, rc.right, rc.bottom -  y);
		rgn_xor.CreateRectRgn(rc.right - border_offset[y], rc.bottom - y - 1, rc.right,rc.bottom -  y);
		rgn.CombineRgn(&rgn, &rgn_xor, RGN_XOR);
		rgn_xor.DeleteObject();
	}
// 	HWND hWnd = GetSafeHwnd();
// 	SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE) | WS_EX_LAYERED);
// 	SetLayeredWindowAttributes(RGB(255, 0, 255), 0, LWA_COLORKEY );	

	SetWindowRgn((HRGN)rgn, TRUE);
	m_Rgn.DeleteObject();
	m_Rgn.Attach(rgn.Detach());
	ReleaseDC(pDC);
}

void CDlgBase::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);

	CRect	rcWindow;

	if (bCalcValidRects && lpncsp->lppos)
	{
		rcWindow.SetRect(lpncsp->lppos->x, lpncsp->lppos->y,
			lpncsp->lppos->x + lpncsp->lppos->cx,
			lpncsp->lppos->y + lpncsp->lppos->cy);
	}
	else
	{
		GetWindowRect(rcWindow);
	}

	lpncsp->rgrc[0] = rcWindow;
}

void CDlgBase::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (lpwndpos->cx < m_MinSize.cx)
	{
		lpwndpos->cx = m_MinSize.cx;
	}
	if (lpwndpos->cy < m_MinSize.cy)
	{
		lpwndpos->cy = m_MinSize.cy;
	}
}

void CDlgBase::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	//	CDialog::OnWindowPosChanged(lpwndpos);

	::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED
		| SWP_NOSENDCHANGING | SWP_NOACTIVATE);

	SendMessage(WM_MOVE, 0, MAKELPARAM(lpwndpos->x, lpwndpos->y));
	SendMessage(WM_SIZE, 0, MAKELPARAM(lpwndpos->cx, lpwndpos->cy));

	OnSize(0, lpwndpos->cx, lpwndpos->cy);
}

void CDlgBase::PostNcDestroy()
{

}

// ���ڹر�
void CDlgBase::OnClose()
{
	// ���ص�WM_CLOSE��Ϣ��������,�����κ�����,����ͨ��Windows���������Թرճ���
	CControlBase* pControlBase = GetBaseControl(NAME_BT_CLOSE);
	if(pControlBase)
	{
		if(pControlBase->GetAction() == ACTION_HIDE_WINDOW)
		{
			// ��������˶���Ϊ���ش���,��ֻ���ش���
			ShowWindow(SW_HIDE);
		}else
		{
			// �رմ���
			DoClose();
		}
	}
}

// �Զ���Ĵ��ڹر���Ϣ����
LRESULT CDlgBase::OnUserCloseWindow(WPARAM wParam, LPARAM lParam)
{
	// ɾ�����ж�ʱ��
	CTimer::KillTimer();
	// wParam������ʾ�Ի���ķ���ֵ
	if(wParam == IDOK)
	{
		OnCancel();
	}else
	if(wParam == IDCANCEL)
	{
		OnOK();
	}else
	{
		EndDialog(wParam);
	}

	return 0;
}

// ������С��
void CDlgBase::OnMinimize()
{
	ShowWindow(SW_MINIMIZE);
}

// ��������л�
BOOL CDlgBase::OnMaximize()
{

 	if (IsZoomed())
 	{
 		ShowWindow(SW_RESTORE);
		return FALSE;
 	}
 	else
 	{
 		ShowWindow(SW_SHOWMAXIMIZED);
		return TRUE;
 	}
}

// ���ڵ�Ƥ��ѡ��
void CDlgBase::OnSkin()
{
	CControlBase* pControlBase = GetBaseControl(NAME_BT_SKIN);
	if(pControlBase == NULL)
	{
		return;
	}

	CDlgSkin *pDlgSkin = new CDlgSkin;
	CRect rc = pControlBase->GetRect();
	rc.OffsetRect(-95, rc.Height());
	ClientToScreen(&rc);
	pDlgSkin->LoadXmlFile(DuiSystem::Instance()->GetXmlFile("dlg_skin"));
	pDlgSkin->Create(this, rc, WM_SKIN);
	pDlgSkin->ShowWindow(SW_SHOW);
}

// ������С����ť����Ϣ����
LRESULT CDlgBase::OnMessageButtomMin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(BUTTOM_UP == uMsg)
	{
		OnMinimize();
	}

	return 0;
}

// ����
LRESULT CDlgBase::OnMessageSkin(WPARAM wParam, LPARAM lParam)
{
	if(MSG_CLOSE == wParam)
	{
	}
	else if(MSG_SKIN == wParam)
	{
		// ѡ���˱���Ƥ��
		SelectInfo *pSelectInfo = (SelectInfo *)lParam;
		if(pSelectInfo != NULL)
		{
			if(pSelectInfo->nType == BKTYPE_COLOR)
			{
				Color &clr = pSelectInfo->clr;
				COLORREF crlBack = RGB(clr.GetRed(), clr.GetGreen(), clr.GetBlue());
				DrawBackground(crlBack);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_COLOR, 0, crlBack, _T(""));
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
			}
			else if(pSelectInfo->nType == BKTYPE_IMAGE_RESOURCE)
			{
				CString strImgFile;
				strImgFile.Format(_T("%s\\SKIN_PIC_%d.png"), DuiSystem::GetExePath() + _T("bkimg"), pSelectInfo->uIndex);
				LoadImage(strImgFile);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_IMAGE_FILE, 0, RGB(0,0,0), strImgFile);
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
			}
		}
	}
	else if(MSG_SELECT_SKIN == wParam)
	{
		// ѡ��Ƥ���ļ�
 		CFileDialog DlgFile(TRUE,NULL,NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY ,
 			TEXT("Skin picture(*.bmp;*.png;*.jpg)|*.bmp;*.png;*.jpg||"),this, 0);
 		DlgFile.m_ofn.nFilterIndex=1;
 		DlgFile.m_ofn.hwndOwner=m_hWnd;
 		DlgFile.m_ofn.lStructSize=sizeof(OPENFILENAME);
 		DlgFile.m_ofn.lpstrTitle=TEXT("Open");
 		DlgFile.m_ofn.nMaxFile=MAX_PATH;
 		if(DlgFile.DoModal() == IDOK)
 		{
 			CString strFileName = DlgFile.GetPathName();
 			CString strFileType = strFileName.Right(3);
 			if (0 == strFileType.CompareNoCase(TEXT("bmp")) || 0 == strFileType.CompareNoCase(TEXT("jpg")) || 0 == strFileType.CompareNoCase(TEXT("png")))
 			{
 				LoadImage(strFileName);
				// ���汳����Ϣ
				DuiSystem::Instance()->SetWindowBkInfo(BKTYPE_IMAGE_FILE, 0, RGB(0,0,0), strFileName);
				// ˢ�����д��ڵı���Ƥ��
				DuiSystem::Instance()->ResetAllWindowsBkSkin();
 			}
 		}
	}

	return 0;
}

// תUI�̴߳���������
LRESULT CDlgBase::OnMessageUITask(WPARAM wParam, LPARAM lParam)
{
	DuiVision::IBaseTask* pTask = (DuiVision::IBaseTask*)wParam;
	DuiVision::CTaskMgr* pTaskMgr = (DuiVision::CTaskMgr*)lParam;

	BOOL bRet = FALSE;
	if((pTask != NULL) && (pTaskMgr != NULL))
	{
		bRet = pTask->TaskProcess(pTaskMgr);
		if(bRet)
		{
			pTask->TaskNotify(pTaskMgr, DuiVision::IBaseTask::TE_Completed);
		}else
		{
			pTask->TaskNotify(pTaskMgr, DuiVision::IBaseTask::TE_Canceled);
		}
	}
	return bRet;
}

// Tray��Ϣ
LRESULT CDlgBase::OnSystemTrayIcon(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		{
			CString strTrayDbClickMsg = DuiSystem::Instance()->GetConfig("trayDbClickMsg");
			if(strTrayDbClickMsg == _T("1"))
			{
				// ������˫����Ϣ
				DuiSystem::AddDuiActionTask(TRAY_ICON, MSG_TRAY_DBCLICK, 0, 0, NAME_TRAY_ICON, _T(""), this);
			}else
			{
				// ��ʾ����
				SetForegroundWindow();
				ShowWindow(SW_NORMAL);
				ShowWindow(SW_SHOW);
				BringWindowToTop();
			}
		}
		break;
	case WM_RBUTTONUP:
		{
			// ��ʾ���̲˵�
			CString strXmlFile = DuiSystem::Instance()->GetXmlFile("menu_tray");
			if(!m_strTrayMenuXml.IsEmpty())	// ������������̲˵��ļ�,��ʹ�����õ��ļ�
			{
				strXmlFile = m_strTrayMenuXml;
				if(strXmlFile.Find(_T(".xml")) == -1)
				{
					strXmlFile = DuiSystem::Instance()->GetXmlFile(CEncodingUtil::UnicodeToAnsi(strXmlFile));
				}else
				if(strXmlFile.Find(_T(":")) == -1)
				{
					strXmlFile = _T("xml:") + strXmlFile;
				}
			}
			if(!strXmlFile.IsEmpty())
			{
				SetForegroundWindow();
				// ���ز˵��ؼ�
				CMenuEx *pMenuEx = new CMenuEx();
				pMenuEx->SetParent(this);
				if(m_pTrayHandler != NULL)
				{
					// �����¼���������
					pMenuEx->RegisterHandler(m_pTrayHandler);
				}
				CPoint point;
				GetCursorPos(&point);
				pMenuEx->LoadXmlFile(strXmlFile, this, point, WM_DUI_MENU);
				// ����˵���λ�ò���ʾ
				CRect rc;
				pMenuEx->GetWindowRect(&rc);
				rc.OffsetRect(rc.Width()/2, -rc.Height());
				// ���������Ļ�Ҳ෶Χ,��˵����������ƶ�һЩ
				int nScreenWidth= GetSystemMetrics(SM_CXFULLSCREEN);
				if(rc.right > nScreenWidth)
				{
					rc.OffsetRect(nScreenWidth - rc.right -10, 0);
				}
				pMenuEx->MoveWindow(rc);
				pMenuEx->ShowWindow(SW_SHOW);
			}
		}
		break;
	}
	return 0;
}

// ���ڿ���̴�����Ϣ��ע����Ϣ,�ڶ���DuiVisionӦ�ý���ͨ������Ϣ��һЩ��Ϣ���ݸ��������е�DuiVisionӦ�ý���
LRESULT CDlgBase::OnCheckItsMe(WPARAM wParam, LPARAM lParam)
{
	// wParam��������Ӧ��,lParam���ھ�������,���͵�������ͨ���ڴ�ӳ���ļ�����
	// �˿������Ϣ��ͨ���㲥��ʽ���͵�,������д��ڶ����յ�,ֻ����������Ҫ��������Ϣ,��������Ӧ�ú���
	// �������յ���Ϣ֮������¼�����������д���

	// �ж��Ƿ�������
	if(DuiSystem::Instance()->GetDuiDialog(0) != this)
	{
		return FALSE;
	}

	// �ж��Ƿ�ǰ��DuiVisionӦ��
	if(wParam != DuiSystem::Instance()->GetAppID())
	{
		return FALSE;
	}

	// ��ȡ�ڴ�ӳ���ļ�������
	// number of characters in memory-mapped file
	const DWORD dwMemoryFileSize = sizeof(DUI_INTERPROCESS_MSG);

	// memory-mapped file name
	const LPCTSTR sMemoryFileName = _T("DF034858-1608-4147-0604-4A0CD86F6C9F");

	HANDLE hFileMapping = NULL;
	LPVOID pViewOfFile = NULL;

	// Create file mapping which can contain dwMemoryFileSize characters
	hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,           // system paging file
		NULL,                           // security attributes
		PAGE_READWRITE,                 // protection
		0,                              // high-order DWORD of size
		dwMemoryFileSize*sizeof(TCHAR), // low-order DWORD of size
		sMemoryFileName);               // name

	DWORD dwError = GetLastError();     // if ERROR_ALREADY_EXISTS 
	// this instance is not first (other instance created file mapping)

	if(! hFileMapping)
	{
		//TRACE(_T("Creating of file mapping failed.\n"));
		return FALSE;
	}

	pViewOfFile = MapViewOfFile(
		hFileMapping,               // handle to file-mapping object
		FILE_MAP_ALL_ACCESS,        // desired access
		0,
		0,
		0);                         // map all file

	if(!pViewOfFile)
	{
		//TRACE(_T("MapViewOfFile failed.\n"));
		CloseHandle(hFileMapping);
		return FALSE;
	}

	DUI_INTERPROCESS_MSG interMsg;
	memcpy(&interMsg, pViewOfFile, sizeof(DUI_INTERPROCESS_MSG));

	UnmapViewOfFile(pViewOfFile);
	CloseHandle(hFileMapping);

	// �����¼�����������д���
	DuiSystem::Instance()->CallDuiHandler(interMsg.uControlID, interMsg.wControlName, interMsg.uMsg, interMsg.wParam, (LPARAM)(&interMsg));

	return TRUE;
}

// ������󻯰�ť����Ϣ����
LRESULT CDlgBase::OnMessageButtomMax(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(BUTTOM_UP == uMsg)
	{
		OnMaximize();
	}

	return 0;
}

// ���ڹرհ�ť����Ϣ����
LRESULT CDlgBase::OnMessageButtomClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(BUTTOM_UP == uMsg)
	{
		DoClose();
	}

	return 0;
}

// ����Skin��ť����Ϣ����
LRESULT CDlgBase::OnMessageButtomSkin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(BUTTOM_UP == uMsg)
	{
		OnSkin();
	}

	return 0;
}

BOOL CDlgBase::OnNcActivate(BOOL bActive)
{
	m_bNCActive = bActive;

	if(m_bNCActive)
	{
		
	}
	else
	{
		m_bTracking = false;
		m_bIsSetCapture = false;
		m_bIsLButtonDblClk = FALSE;
		
		if(m_bIsLButtonDown)
		{
			m_bIsLButtonDown = FALSE;
			if(m_pControl)
			{
				m_pControl->OnLButtonUp(0, CPoint(-1, -1));
				m_pControl = NULL;				
			}
		}	
		else
		{
			if(m_pControl)
			{
				m_pControl->OnMouseMove(0, CPoint(-1, -1));
				m_pControl = NULL;				
			}
		}		
	}
	InvalidateRect(NULL);

	return TRUE;	
}

void CDlgBase::PreSubclassWindow()
{
	// �ж��Ƿ�������קͼƬ�ļ���Ϊ���ڱ���
	CString strEnableDragFile = DuiSystem::Instance()->GetConfig("enableDragFile");
	if(strEnableDragFile == _T("1"))
	{
		DragAcceptFiles(TRUE);
	}
	
	CDialog::PreSubclassWindow();
}

// ���õ�ǰ��Tooltip
void CDlgBase::SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect)
{
	m_wndToolTip.DelTool(this, 1);
	m_wndToolTip.Activate(FALSE);
	if(!strTooltip.IsEmpty())
	{
		//int nCount = m_wndToolTip.GetToolCount();
		m_wndToolTip.SetMaxTipWidth(pControl->GetRect().Width());
		m_wndToolTip.AddTool(this, strTooltip, rect, 1);//TooltipĬ�϶���1��ΪID, pControl->GetID());
		m_wndToolTip.Activate(TRUE);
		m_nTooltipCtrlID = pControl->GetID();
	}
}

// �����ǰ��Tooltip
void CDlgBase::ClearTooltip()
{
	m_wndToolTip.DelTool(this, 1);
}

void CDlgBase::OnMouseMove(UINT nFlags, CPoint point) 
{	
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = ::_TrackMouseEvent(&tme);

		// ����д��ڹرն�ʱ��,����ֹͣ��ʱ��
		if(m_bTracking && (m_uTimerAutoClose != 0))
		{
			CTimer::KillTimer(m_uTimerAutoClose);
			m_uTimerAutoClose = 0;
		}
	}
	
	if (m_pControl)
	{
		if((m_nTooltipCtrlID != m_pControl->GetID()) && !m_pControl->GetTooltip().IsEmpty())
		{
			// �����ǰ�ؼ���Tooltip,������һ��Tooltip,Tooltip��IDĬ�϶���1
			m_wndToolTip.DelTool(this, 1);
			m_wndToolTip.Activate(FALSE);
			m_wndToolTip.AddTool(this, m_pControl->GetTooltip(), m_pControl->GetRect(), 1);
			m_wndToolTip.Activate(TRUE);
			m_nTooltipCtrlID = m_pControl->GetID();
		}

		if((m_pControl->PtInRect(point) || m_bIsLButtonDown) && m_bTracking)
		{			
			m_pControl->OnMouseMove(nFlags, point);			
			return;
		}
	}

	if (!m_bIsLButtonDown)
	{
		CControlBase * pOldControl = m_pControl;
		BOOL bIsSelect = FALSE;
		BOOL bIsSystemSelect = FALSE;

		if(m_bTracking)
		{
			// ���������Ļ����ؼ�
			for (size_t i = 0; i < m_vecBaseControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecBaseControl.at(i);
				if (pControlBase)
				{
					pControlBase->OnMouseMove(nFlags, point);
					if(pControlBase->PtInRect(point) && pControlBase->GetRresponse())
					{
						m_pControl = pControlBase;
						bIsSystemSelect = TRUE;
					}
				}		
			}
			
			// �û��ؼ�
			for (size_t i = 0; i < m_vecControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecControl.at(i);
				if (pControlBase)
				{
					pControlBase->OnMouseMove(nFlags, point);
					if (!bIsSystemSelect)
					{
						if(pControlBase->PtInRect(point) && pControlBase->GetRresponse())
						{
							m_pControl = pControlBase;
							bIsSelect = TRUE;
						}
					}
				}		
			}
		}

		if (!bIsSelect && !bIsSystemSelect)
		{
			m_pControl = NULL;
		}
	}
	
	//CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgBase::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	if (!m_bIsLButtonDown)
	{
		if (m_pControl)
		{
			m_pControl->OnMouseMove(0, CPoint(-1, -1));
		}
			
		m_pControl = NULL;
	}

	// ����д��ڹرջ����ر�־,�򴴽��رն�ʱ��
	if((m_bAutoClose || m_bAutoHide) && (m_uAutoCloseDelayTime != 0) && (m_uTimerAutoClose == 0))
	{
		m_uTimerAutoClose = CTimer::SetTimer(m_uAutoCloseDelayTime);
	}
	
	return 0;
}

LRESULT CDlgBase::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
 	//if (m_pControl)
 	//{
 	//	CPoint point;
 	//	GetCursorPos(&point);
 	//	//ScreenToClient(&point);
		//m_pControl->OnMouseMove(0, point);
 	//}
	return 0;
}

// ��������Ϣ����
BOOL CDlgBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ScreenToClient(&pt);
 	zDelta /= WHEEL_DELTA;
	UINT nFlag = (zDelta>0) ? SB_LINEUP : SB_LINEDOWN;
	zDelta = abs(zDelta);
	BOOL bResponse = FALSE;
	for(short i = 0; i < zDelta; i++)
	{
		if (m_pControl && m_pControl->OnScroll(TRUE, nFlag, pt))
		{
			bResponse = TRUE;
			continue;
		}else
		{
			// ���������Ļ����ؼ�
			for (size_t i = 0; i < m_vecBaseControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecBaseControl.at(i);
				if (pControlBase && pControlBase->OnScroll(TRUE, nFlag, pt))
				{
					bResponse = TRUE;
					break;
				}		
			}
			
			// �û��ؼ�
			for (size_t i = 0; i < m_vecControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecControl.at(i);
				if (pControlBase && pControlBase->OnScroll(TRUE, nFlag, pt))
				{
					bResponse = TRUE;
					break;
				}	
			}
		}
	}

    return bResponse;
}

void CDlgBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bIsSelect = false;

	if(m_pFocusControl != m_pControl && m_pFocusControl != NULL)
	{
		m_pFocusControl->OnFocus(false);
		m_pFocusControl = NULL;
	}
	/*// �л�����ؼ�
	CControlBase* pFocusControl = GetFocusControl();
	if(pFocusControl != m_pFocusControl)
	{
		if(m_pFocusControl != NULL)
		{
			m_pFocusControl->OnFocus(FALSE);
		}
		if(pFocusControl != NULL)
		{
			pFocusControl->OnFocus(TRUE);
		}
		m_pFocusControl = pFocusControl;
	}*/

	if (m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
		{
			if (m_pControl->PtInRect(point))
			{
				bIsSelect = TRUE;
				m_bIsLButtonDown = TRUE;

				m_pFocusControl = m_pControl;
				m_pControl->OnLButtonDown(nFlags, point);						
			}
		}
	}
	
	// ��ǰ�ؼ��Ƿ���Բ������
	if (bIsSelect && !m_bIsSetCapture && ((m_pControl == NULL) || m_pControl->OnCheckMouseResponse(nFlags, point)))
	{
		SetCapture();
		m_bIsSetCapture = TRUE;
		
		return;
	}

	// �����϶���Ϣ
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsSetCapture)
	{
		ReleaseCapture();
		m_bIsSetCapture = false;
	}

	m_bIsLButtonDown = FALSE;

	if (m_pControl)
	{
		if(m_pControl->GetVisible() && m_pControl->GetRresponse())
		{
			CRect rc = m_pControl->GetRect();
			m_pControl->OnLButtonUp(nFlags, point);				

			if (!rc.PtInRect(point))
			{
				m_pControl = NULL;
			}	
		}
		else
		{
			m_pControl = NULL;
		}	
	}
	
	m_bIsLButtonDblClk = FALSE;

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bIsLButtonDblClk = TRUE;

	if(m_pControl)
	{
		if(!m_pControl->GetDblClk())
		{
			return OnLButtonDown(nFlags, point);
		}
	}

	//OnMaximize();

	CDialog::OnLButtonDblClk(nFlags, point);
}

// �����¼�����(�Ƶ�PreTranslateMessage��ʵ���ӿؼ��ļ����¼�����,��Ϊ�Ի����OnKeyDown�����о�����,���ܲ���ALT����ϼ�)
void CDlgBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CDlgBase::PreTranslateMessage(MSG* pMsg)
{
	if (( pMsg->message == WM_KEYDOWN ) || ( pMsg->message == WM_SYSKEYDOWN ))
	{
		// �����¼�����
		UINT nFlags = 0;
		BOOL bCtrl=::GetKeyState(VK_CONTROL)&0x8000;
		BOOL bShift=::GetKeyState(VK_SHIFT)&0x8000;
		BOOL bAlt=::GetKeyState(VK_MENU)&0x8000;	// ALT��ֻ��WM_SYSKEYDOWN��Ϣ���ܲ���
		nFlags |= (bCtrl ? VK_CONTROL : 0);
		nFlags |= (bShift ? VK_SHIFT : 0);
		nFlags |= (bAlt ? VK_MENU : 0);

		// ����tab����shift+tab��,�л�����
		if((pMsg->wParam == VK_TAB) && (nFlags == 0))
		{
			CControlBase* pFocusControl = GetFocusControl();
			CControlBase* pNextControl = GetNextFocusableControl();
			if(pNextControl != pFocusControl)
			{
				if(pFocusControl != NULL)
				{
					pFocusControl->OnFocus(FALSE);
				}
				if(pNextControl != NULL)
				{
					pNextControl->OnFocus(TRUE);
				}
				m_pFocusControl = pNextControl;
				return TRUE;
			}
		}else
		if((pMsg->wParam == VK_TAB) && (nFlags == VK_SHIFT))
		{
			CControlBase* pFocusControl = GetFocusControl();
			CControlBase* pPrevControl = GetPrevFocusableControl();
			if(pPrevControl != pFocusControl)
			{
				if(pFocusControl != NULL)
				{
					pFocusControl->OnFocus(FALSE);
				}
				if(pPrevControl != NULL)
				{
					pPrevControl->OnFocus(TRUE);
				}
				m_pFocusControl = pPrevControl;
				return TRUE;
			}
		}

		// ��ǰ�ؼ��Ƿ��ܴ���
		if (m_pControl && m_pControl->OnKeyDown(pMsg->wParam, 1, nFlags))
		{
			return TRUE;
		}

		// ���������Ļ����ؼ�
		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase && pControlBase->OnKeyDown(pMsg->wParam, 1, nFlags))
			{
				return TRUE;
			}		
		}
		
		// �û��ؼ�
		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase && pControlBase->OnKeyDown(pMsg->wParam, 1, nFlags))
			{
				return TRUE;
			}	
		}

		// ����ؼ�û�д���,��س���ESC������Ҫ���ε�
		if(pMsg->wParam == VK_ESCAPE) 
		{
			return TRUE;
		}
		if(pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}else
	if ( pMsg->message == WM_MOUSEMOVE ||
		 pMsg->message == WM_LBUTTONDOWN ||
		 pMsg->message == WM_LBUTTONUP )
	{
		m_wndToolTip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// ��ʱ����Ϣ
void CDlgBase::OnTimer(UINT uTimerID)
{
	if(m_uTimerAnimation == uTimerID)	// ������ʱ��
	{
		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnTimer();
			}		
		}

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				pControlBase->OnTimer();
			}		
		}
	}else
	if(m_uTimerAutoClose == uTimerID)	// �����Զ��رյĶ�ʱ��
	{
		CTimer::KillTimer(m_uTimerAutoClose);
		m_uTimerAutoClose = 0;
		if(m_bAutoHide)
		{
			ShowWindow(SW_HIDE);	// ���ش���ģʽ
		}else
		{
			DoClose();	// �رմ���ģʽ
		}
	}
}

// ��ʱ����Ϣ(����ʱ�����ֵĶ�ʱ����)
void CDlgBase::OnTimer(UINT uTimerID, CString strTimerName)
{
	// Ӧ�ô����Ķ�ʱ���������¼���������Ķ�ʱ��������
	DuiSystem::Instance()->CallDuiHandlerTimer(uTimerID, strTimerName);
}

// �����ؼ�����Ϣ��Ӧ
LRESULT CDlgBase::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CControlBase* pControlBase = GetBaseControl(uID);
	CControlBase* pControl = GetControl(uID);
	if((pControlBase == NULL) && (pControl == NULL))
	{
		return 0L;
	}

	if(pControlBase != NULL)
	{
		// 2.�ж��Ƿ񼸸�ϵͳ��ť,ִ����Ӧ��ϵͳ����
		if(pControlBase->IsThisObject(BT_MIN, NAME_BT_MIN))
		{
			OnMessageButtomMin(uMsg, wParam, lParam);
		}
		else if(pControlBase->IsThisObject(BT_MAX, NAME_BT_MAX))
		{
			OnMessageButtomMax(uMsg, wParam, lParam);
		}
		else if(pControlBase->IsThisObject(BT_CLOSE, NAME_BT_CLOSE))
		{
			if(pControlBase->GetAction() == ACTION_HIDE_WINDOW)
			{
				// ��������˶���Ϊ���ش���,��ֻ���ش���
				ShowWindow(SW_HIDE);
			}else
			{
				// �رմ���
				OnMessageButtomClose(uMsg, wParam, lParam);
			}
		}
		else if(pControlBase->IsThisObject(BT_SKIN, NAME_BT_SKIN))
		{
			OnMessageButtomSkin(uMsg, wParam, lParam);
		}
		else
		{
			// 3.����ϵͳ��ť,����ø����Ի���Ĭ�Ͽؼ����¼���������
			for (size_t i = 0; i < m_vecBaseControl.size(); i++)
			{
				CControlBase * pControlBase = m_vecBaseControl.at(i);
				if (pControlBase && (pControlBase->GetControlID() == uID))
				{
					pControlBase->OnMessage(uID, uMsg, wParam, lParam);
				}
			}
		}
	}else
	if(pControl != NULL)
	{
		// �Ի���ؼ����¼�����
		OnMessage(uID, uMsg, wParam, lParam);
	}

	return 0L; 
}

LRESULT CDlgBase::OnControlUpdate(CRect rcUpdate, BOOL bUpdate, CControlBase *pUpdateControlBase) 
{
	if(pUpdateControlBase == NULL) return 0;

	CRect rcAllUpDate = rcUpdate;
	if(bUpdate)
	{
		pUpdateControlBase->SetUpdate(FALSE, m_clrBK);
	}
	
	if(m_bInit)
	{
		BOOL bFind = false;
		for (size_t i = 0; i < m_vecArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecArea.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}
			}
		}

		for (size_t i = 0; i < m_vecBaseArea.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseArea.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}
			}
		}

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}		
			}
		}

		for (size_t i = 0; i < m_vecBaseControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecBaseControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE, m_clrBK);
					}
				}
				else if(pControlBase == pUpdateControlBase )
				{
					bFind = true;
				}			
			}
		}	
	}

	InvalidateRect(&rcAllUpDate);

	return 0L; 
};

// DUI��Ϣ��������
LRESULT CDlgBase::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����ÿ���ؼ������ĸ��ؼ��Ǵ�ID��������¼��Ĵ���
	CControlBase * pThisControlBase = NULL;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && (pControlBase->GetControlID() == uID))
		{
			pThisControlBase = pControlBase;
			pControlBase->OnMessage(uID, Msg, wParam, lParam);
		}
	}

	return 0;
}

// �򿪵����Ի���
void CDlgBase::OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID)
{
	ASSERT(pWndPopup);
	CloseDlgPopup();
	ClientToScreen(&rc);
	m_pWndPopup = pWndPopup;
	m_pWndPopup->Create(this, rc, uMessageID);
	m_pWndPopup->ShowWindow(SW_SHOW);
}

// �رյ����Ի���
void CDlgBase::CloseDlgPopup()
{
	if(m_pWndPopup)
	{
 		if(m_pFocusControl && m_pControl != m_pFocusControl)
 		{
 			m_pFocusControl->OnFocus(false);
 			m_pFocusControl = NULL;
 		}
		if(IsWindow(m_pWndPopup->GetSafeHwnd()))
		{
			m_pWndPopup->CloseWindow();			
		}
	}
	m_pWndPopup = NULL;
}

void CDlgBase::OnDestroy()
{
	__super::OnDestroy();

	// ������ʱ��
	CTimer::KillTimer();
}