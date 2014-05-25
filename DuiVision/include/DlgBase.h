

#ifndef __DLG_MY_BASE_X_H__
#define __DLG_MY_BASE_X_H__
#include <vector>
#include "DuiVision.h"

using namespace  std;

class  CDlgBase : public CDialog, public CTimer, public CDuiObject
{
public:
	static LPCSTR GetClassName() { return "dlg";}
	virtual BOOL IsClass(LPCSTR lpszName)
	{
		if(strcmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

	DECLARE_DYNAMIC(CDlgBase)

	// �Ի���ؼ�Ԥ���ýṹ
	struct CONTROL_VALUE
	{
		CString strName;	// �ؼ�������
		CString strType;	// �ؼ�����
		CString strValue;	// ���õ�ֵ
	};

public:
	UINT			m_nIDTemplate;				// �Ի�����ԴID
	BOOL			m_bInit;					// �Ƿ��ʼ�����
	CString			m_strXmlFile;				// ���ڵ�XML�ļ���
	CString			m_strXmlContent;			// ���ڵ�XML����

protected:
	CDuiObject*		m_pParentDuiObject;			// ���ؼ�����

	DWORD			m_nMainThreadId;			// ���߳�ID

	UINT			m_uTimerAnimation;			// ������ʱ��
	UINT			m_uTimerAutoClose;			// ���ڴ����Զ��رյĶ�ʱ��ID

	BOOL			m_bChangeSize;				// �ı䴰�ڴ�С
	CSize			m_MinSize;					// �����޶���С��С
	CRgn			m_Rgn;						// �����򴰿�����
	COLORREF		m_clrBK;					// �Զ���ǰ����ɫ

	CString			m_strTitle;					// ���ڱ���
	CFont			m_TitleFont;				// ���Ʊ�����������
	HICON			m_hIcon;					// ����ͼ����
	BOOL			m_bAppWin;					// �Ƿ�������

	CDlgPopup		*m_pWndPopup;				// ����ĵ�����ָ��

	CBitmap			m_BKImage;					// ��ܱ���ͼƬ
	CSize			m_sizeBKImage;
	CString			m_strFramePicture;			// ����ͼƬ
	CDC				m_MemBKDC;					// ����dc
	CBitmap			*m_pOldMemBK;
	CBitmap			m_MemBK;
	BOOL			m_bDrawImage;				// ͼƬ��ɫ����
	int				m_nFrameSize;				// �߿��С
	int				m_nFrameWLT;				// �߿����Ͻǿ���(�Ź���ģʽ)
	int				m_nFrameHLT;				// �߿����ϽǸ߶�(�Ź���ģʽ)
	int				m_nFrameWRB;				// �߿����½ǿ���(�Ź���ģʽ)
	int				m_nFrameHRB;				// �߿����½Ǹ߶�(�Ź���ģʽ)

	CString			m_strBkImg;					// ����ͼƬ
	COLORREF		m_crlBack;					// ������ɫ

	int				m_nFrameTopBottomSpace;
	int				m_nFrameLeftRightSpace;

	int				m_nOverRegioX;				//���ȵĴ�С
	int				m_nOverRegioY;				//���ȵĴ�С
	BOOL			m_bNCActive;

	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsSetCapture;

	BOOL			m_bAutoClose;				// �����Զ��رձ�־
	BOOL			m_bAutoHide;				// �����Զ����ر�־
	UINT			m_uAutoCloseDelayTime;		// �����Զ��رյ��ӳ�ʱ��

	CToolTipCtrl	m_wndToolTip;				// Tooltip
	int				m_nTooltipCtrlID;			// ��ǰTooltip��ʾ�Ŀؼ�ID

	CDuiHandler*	m_pTrayHandler;				// ����ͼ��Ͳ˵����¼���������
	CString			m_strTrayMenuXml;			// ���̲˵���XML�����ļ�

	vector<CControlBase *>	m_vecControl;		// �û����ӵĴ��ڿؼ�
	vector<CControlBase *>	m_vecArea;			// �û����ӵ�����(��Ӱ������¼�)
	CControlBase	*m_pControl;				// ��ǰ��ؼ�����
	CControlBase	*m_pFocusControl;			// ��ǰ����Ŀؼ�����

	vector<CONTROL_VALUE>	m_vecControlValue;	// �ؼ�Ԥ������Ϣ

private:
	vector<CControlBase *>	m_vecBaseControl;	// ���������õ���һЩĬ�Ͽؼ�
	vector<CControlBase *>	m_vecBaseArea;		// ����Ĭ������

public:
	CDlgBase(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CDlgBase();

	UINT GetIDTemplate() { return m_nIDTemplate; }

	void SetMinSize(int iWidth, int iHeight);	// ������С���ڴ�С
	CSize GetMinSize();

	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }

	void TestMainThread();	// �����Ƿ������߳�

	void SetXmlFile(CString strXmlFile) {m_strXmlFile = strXmlFile;}
	void SetXmlContent(CString strXmlContent) {m_strXmlContent = strXmlContent;}

	void SetTrayHandler(CDuiHandler* pDuiHandler) { m_pTrayHandler = pDuiHandler; }
	void SetTratMenuXml(CString strMenuXml) { m_strTrayMenuXml = strMenuXml; }

	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetControl(CString strControlName);
	CControlBase *GetBaseControl(UINT uControlID);
	CControlBase *GetBaseControl(CString strControlName);

	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	void SetFocusControl(CControlBase* pFocusControl);
	CControlBase* GetFocusControl();
	CControlBase* GetPrevFocusableControl();
	CControlBase* GetNextFocusableControl();

	// ȷ�Ϻͷ���
	void DoOK() { PostMessage(WM_USER_CLOSEWND, IDOK, 0); }
	void DoCancel() { PostMessage(WM_USER_CLOSEWND, IDCANCEL, 0); }
	void DoClose() { PostMessage(WM_USER_CLOSEWND, IDCANCEL, 0); }
	void DoYes() { PostMessage(WM_USER_CLOSEWND, IDYES, 0); }
	void DoNo() { PostMessage(WM_USER_CLOSEWND, IDNO, 0); }

	// �ƶ��ؼ�
	virtual CControlBase * SetControlRect(UINT uControlID, CRect rc);
	// �ƶ��ؼ�
	virtual CControlBase * SetControlRect(CControlBase *pControlBase, CRect rc);
	// ��ʾ�ؼ�
	virtual CControlBase * SetControlVisible(UINT uControlID, BOOL bVisible);
	// ��ʾ�ؼ�
	virtual CControlBase * SetControlVisible(CControlBase *pControlBase, BOOL bVisible);
	// ���ÿؼ�
	virtual CControlBase * SetControlDisable(UINT uControlID, BOOL bDisable);
	// ���ÿؼ�
	virtual CControlBase * SetControlDisable(CControlBase *pControlBase, BOOL bDisable);

	// ����resize����
	HRESULT OnAttributeResize(const CStringA& strValue, BOOL bLoading);

	// �򿪵����Ի���
	void OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID);
	// �رյ����Ի���
	void CloseDlgPopup();

	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCSTR lpszName);

	// ���ò�����������
	void SetupRegion(int border_offset[], int nSize);
	void DrawImageStyle(CDC &dc, const CRect &rcClient, const CRect &rcUpdate);
	
	// ��ʼ�����ڱ���Ƥ��
	void InitWindowBkSkin();
	// ����ͼƬ
	void LoadImage(UINT nIDResource, CString strType = TEXT("PNG"));
	void LoadImage(CString strFileName);
	// ��ʼ�����ڿؼ���Ĭ��ֵ
	void InitUIState();
	// ���ò�����������
	void SetupRegion(int nSize);
	// ������ͼƬ
	void DrawBackground(CBitmap &bitBackground);
	// ������ͼƬ
	void DrawBackground(COLORREF clr);
	// ǰ��ͼƬ
	virtual void DrawControl(CDC &dc, const CRect &rcClient);	
	// ���ÿؼ�
	virtual void ResetControl();
	// ����ѡ��
	void UpdateHover();

	// ����Tooltip
	void SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect);
	// ���Tooltip
	void ClearTooltip();

	void InitBaseUI(CRect rcClient, TiXmlElement* pNode);
	virtual void InitUI(CRect rcClient, TiXmlElement* pNode);
	virtual void OnSize(CRect rcClient);

	void SetControlValue(CString strName, CString strType, CString strValue);
	void InitDialogValue();
	void InitControlValue();

	void SetAutoCloseTimer(UINT uDelayTime, BOOL bHideWnd = FALSE);

	// ��ʱ����Ϣ
	virtual void OnTimer(UINT uTimerID);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnClose();
	virtual void OnMinimize();
	virtual BOOL OnMaximize();
	virtual void OnSkin();

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	afx_msg LRESULT OnUserCloseWindow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageSkin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageUITask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnSystemTrayIcon(WPARAM wParam, LPARAM lParam);

	// �����֪ͨ��Ϣ
	LRESULT OnCheckItsMe(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageButtomMin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtomMax(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtomClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtomSkin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("appwin", m_bAppWin, FALSE)
		DUI_TSTRING_ATTRIBUTE("title", m_strTitle, FALSE)
		DUI_INT_ATTRIBUTE("width", m_MinSize.cx, FALSE)
		DUI_INT_ATTRIBUTE("height", m_MinSize.cy, FALSE)
		DUI_CUSTOM_ATTRIBUTE("resize", OnAttributeResize)
		DUI_TSTRING_ATTRIBUTE("frame", m_strFramePicture, FALSE)
		DUI_INT_ATTRIBUTE("framesize", m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE("width-lt", m_nFrameWLT, FALSE)
		DUI_INT_ATTRIBUTE("height-lt", m_nFrameHLT, FALSE)
		DUI_INT_ATTRIBUTE("width-rb", m_nFrameWRB, FALSE)
		DUI_INT_ATTRIBUTE("height-rb", m_nFrameHRB, FALSE)
		DUI_TSTRING_ATTRIBUTE("bkimg", m_strBkImg, FALSE)
		DUI_RGBCOLOR_ATTRIBUTE("crbk", m_crlBack, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};

#endif __DLG_MY_BASE_X_H__
