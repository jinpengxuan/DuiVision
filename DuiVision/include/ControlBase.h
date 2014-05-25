#pragma once

#include "DuiObject.h"

#include <vector>

using namespace  std;

class CDlgBase;

// ��ť״̬
enum enumButtonState
{
	enBSNormal = 0,
	enBSHover,
	enBSDown,
	enBSDisable,
	enBSHoverDown,	
	enBSDisableDown
};

// ��ť��Ϣ����
#define				BUTTOM_DOWN			1
#define				BUTTOM_UP			2

// �˵�ID����
#define					WM_DUI_MENU		(WM_USER + 20)	

class CControlBase : public CDuiObject
{
public:
	CControlBase(HWND hWnd, CDuiObject* pDuiObject) ;
	CControlBase(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE , BOOL bRresponse = TRUE) ;
	virtual ~CControlBase(void);

	// λ�õļ��㷽ʽ,��ͨ��������Ϊ0���㡢���հٷֱȼ���
	enum PIT{PIT_NORMAL=0,PIT_CENTER,PIT_PERCENT};

	// λ����Ϣ��ÿ�������Ϣ
    struct DUIDLG_POSITION_ITEM
    {
		PIT pit;
        BOOL bMinus;
        float  nPos;
    };

	// λ����Ϣ�ṹ
    struct DUIDLG_POSITION
    {
        int nCount;
        union
        {
            struct
            {
                DUIDLG_POSITION_ITEM Left;
                DUIDLG_POSITION_ITEM Top;
                DUIDLG_POSITION_ITEM Right;
                DUIDLG_POSITION_ITEM Bottom;
            };
            DUIDLG_POSITION_ITEM Item[4];
        };
    };

	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }

	void TestMainThread();
	void Draw(CDC &dc, CRect rcUpdate);
	virtual void DrawControl(CDC &dc, CRect rcUpdate) = 0;
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);
	virtual BOOL IsDraw(CPoint point) { return FALSE; }
	void SetUpdate(BOOL bUpdate, COLORREF clr = 0);
	BOOL GetUpdate() { return m_bUpdate; };
	void UpdateMemDC(CDC &dc, int nWidth, int nHeight);
	void UpdateAnimateDC(CDC &dc, int nWidth, int nHeight);

	virtual void SetRect(CRect rc);
	virtual void SetControlRect(CRect rc) { m_rc = rc; };
	virtual CRect GetRect() { return m_rc;};
	virtual void OnPositionChange();	// λ��ˢ��

	virtual void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);

	void SetPosStr(CStringA strPos) { m_strPos = strPos; };
	CStringA GetPosStr() { return m_strPos;};
	int PositionItem2Value( const DUIDLG_POSITION_ITEM &pos ,int nMin, int nMax);
	LPCSTR ParsePosition(const char * pszPos,DUIDLG_POSITION_ITEM &pos);
	HRESULT OnAttributePosChange(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeWidth(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeHeight(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeShortcut(const CStringA& strValue, BOOL bLoading);
	
	void SetVisible(BOOL bIsVisible);
	virtual void SetControlVisible(BOOL bIsVisible) { m_bIsVisible = bIsVisible; }
	BOOL GetVisible() { return m_bIsVisible; }
	virtual void SetControlWndVisible(BOOL bIsVisible) { };	// ���ÿؼ��е�Windowsԭ���ؼ��Ƿ�ɼ���״̬
	void SetDisable(BOOL bIsDisable);
	virtual	void SetControlDisable(BOOL bIsDisable) { m_bIsDisable = bIsDisable; }
	BOOL GetDisable() { return m_bIsDisable; }
	BOOL IsTabStop() { return m_bTabStop; }
	void SetTooltip(CString strTooltip) { m_strTooltip = strTooltip; }
	CString GetTooltip() { return m_strTooltip; }
	void SetAction(CString strAction) { m_strAction = strAction; }
	CString GetAction() { return m_strAction; }

	virtual	BOOL PtInRect(CPoint point);	// �ж������Ƿ��ڿؼ���Χ��
	UINT GetControlID() { return m_uID; }	// �ؼ�ID����DUI�����ID
	void UpdateControl(BOOL bUpdate = false, BOOL bVisible = false);
	void UpdateControl(CRect rc, BOOL bVisible = false, BOOL bUpdate = false);
	BOOL GetDblClk() { return m_bDblClk; }

	virtual	BOOL SetWindowFocus();				// ���ô��ڽ���
	BOOL OnFocus(BOOL bFocus);
	virtual	BOOL SetControlFocus(BOOL bFocus) { return FALSE; }		// ���ÿؼ��Ľ���
	BOOL IsFocusControl();
	CControlBase* GetFocusControl(CControlBase* pFocusControl);
	CControlBase* GetPrevFocusableControl(CControlBase* pFocusControl);
	CControlBase* GetNextFocusableControl(CControlBase* pFocusControl);
	
	void SetRresponse(BOOL bRresponse) { m_bRresponse = bRresponse;};
	BOOL GetRresponse() {return m_bRresponse;};

	BOOL OnMouseMove(UINT nFlags, CPoint point);
	BOOL OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnScroll(BOOL bVertical, UINT nFlags, CPoint point);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL OnTimer();

	virtual BOOL OnMousePointChange(CPoint& point) { return FALSE; }
	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point) { return TRUE; }	// �ж�����Ƿ��ڿؼ�����Ӧ������
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual	BOOL OnControlTimer() { return FALSE;};

	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetControl(CString strControlName);
	void AddControl(CControlBase* pControl);
	BOOL RemoveControl(CControlBase* pControl);
	BOOL RemoveControl(CString strControlName, UINT uControlID);
	void RemoveControls(CStringA strClassName);
	CDlgBase* GetParentDialog(BOOL bEnablePopup = TRUE);
	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CallDuiHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ����֪ͨ��Ϣ
	LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

protected:

	CDuiObject*				m_pParentDuiObject;	// ���ؼ�����
	HWND					m_hWnd;
	
	CStringA				m_strPos;			// ����(XML�����е��ַ���)
	BOOL					m_bIsVisible;		// �ɼ�
	BOOL					m_bIsDisable;		// �Ƿ����
	BOOL					m_bRresponse;		// �Ƿ������Ӧ����¼�
	BOOL					m_bTabStop;			// �Ƿ������ӦTab��
	BOOL					m_bIsSelect;		// �Ƿ�ѡ��
	BOOL					m_bDblClk;			// �Ƿ�˫��
	BOOL					m_bMouseDown;		// �Ƿ���갴��

	UINT					m_nShortcutKey;		// ��ݼ�����
	UINT					m_nShortcutFlag;	// ��ݼ�ɨ����

	BOOL					m_bUpdate;			// ������ͼ

	BOOL					m_bIsRun;			// �Ƿ���ִ�й���
	BOOL					m_bRunTime;			// ִ�ж�ʱ��

	vector<CControlBase *>	m_vecControl;		// �ؼ��б�
	vector<CControlBase *>	m_vecArea;			// �����б�(��Ӱ������¼�)
	CControlBase*			m_pControl;			// ��ǰ�ؼ�

	// �ڴ�DC
	int						m_nDCWidth;			// �ڴ�DC����
	int						m_nDCHeight;		// �ڴ�DC�߶�
	CBitmap					m_bitmap;			// �ڴ�DC��λͼ����
	CDC						m_memDC;			// �ڴ�DC
	CBitmap*				m_pOldBitmap;		// �����ڴ�DC�ɵ�λͼ����

	// ����DC
	int						m_nAniDCWidth;		// ����DC����
	int						m_nAniDCHeight;		// ����DC�߶�
	CBitmap					m_aniBitmap;		// ����DC��λͼ����
	CDC						m_animateDC;		// ����DC
	CBitmap*				m_pOldAniBitmap;	// ���涯��DC�ɵ�λͼ����

	int						m_nWidth;			// �ؼ�����
	int						m_nHeight;			// �ؼ��߶�

	CString					m_strTooltip;		// �ؼ���Tooltip
	CString					m_strAction;		// �ؼ�ִ�еĶ���
	BOOL					m_bTaskMsg;			// �����¼���Ӧ����ʱ���������ʽ

	DUI_DECLARE_ATTRIBUTES_BEGIN()
        //DUI_STYLE_ATTRIBUTE("class", m_style, TRUE)
		DUI_INT_ATTRIBUTE("show", m_bIsVisible, TRUE)
		DUI_INT_ATTRIBUTE("disable", m_bIsDisable, TRUE)
		DUI_INT_ATTRIBUTE("response", m_bRresponse, TRUE)
		DUI_INT_ATTRIBUTE("tabstop", m_bTabStop, TRUE)
		DUI_CUSTOM_ATTRIBUTE("pos", OnAttributePosChange)
		DUI_CUSTOM_ATTRIBUTE("width", OnAttributeWidth)
		DUI_CUSTOM_ATTRIBUTE("height", OnAttributeHeight)
		DUI_TSTRING_ATTRIBUTE("action", m_strAction, FALSE)
		DUI_INT_ATTRIBUTE("taskmsg", m_bTaskMsg, TRUE)
		DUI_TSTRING_ATTRIBUTE("tip", m_strTooltip, FALSE)
		DUI_CUSTOM_ATTRIBUTE("shortcut", OnAttributeShortcut)
    DUI_DECLARE_ATTRIBUTES_END()
};

// DUI������Ϣ�ṹ
struct DuiFontInfo
{
	CString strName;	// ���������
	CString strFont;	// ������
	int nFontWidth;		// �����С
	FontStyle fontStyle;// ������
	CString strOS;		// ����ϵͳ
};

// �������ֵĿؼ�����
class CControlBaseFont : public CControlBase
{
public:
	enum {
        // Specify by "width" attribute
        SizeX_Mask          = 0x0007UL, 
        SizeX_Specify       = 0x0001UL, // width > 0
        SizeX_FitContent    = 0x0002UL, // width <= 0
        SizeX_FitParent     = 0x0004UL, // width = "full" default

        // Specify by "height" attribute
        SizeY_Mask          = 0x0070UL, 
        SizeY_Specify       = 0x0010UL, // height > 0
        SizeY_FitContent    = 0x0020UL, // height <= 0 default
        SizeY_FitParent     = 0x0040UL, // height = "full" default
        // Specify by "float" attribute

        Position_Mask       = 0x0300UL, 
        Position_Relative   = 0x0100UL, // float = 0 default
        Position_Float      = 0x0200UL, // float = 1

        // Specify by "valign" and "align" attribute, only using at float = 1 or panel control (Vert-Align)
        Align_Mask          = 0xF000UL, 
        VAlign_Top          = 0x0000UL, // valign = top
        VAlign_Middle       = 0x1000UL, // valign = middle
        VAlign_Bottom       = 0x2000UL, // valign = bottom
        Align_Left          = 0x0000UL, // align = left
        Align_Center        = 0x4000UL, // align = center
        Align_Right         = 0x8000UL, // align = right
    };

public:
	CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject);
	CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE , BOOL bRresponse = TRUE,
		CString strFont = _T(""), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular) ;
	virtual ~CControlBaseFont(void);

	void SetTitle(CString strTitle);
	virtual void SetControlTitle(CString strTitle) { m_strTitle = strTitle; };
	virtual CString GetTitle() { return m_strTitle;};
	virtual void SetAlignment(UINT uAlignment);
	virtual void SetVAlignment(UINT uVAlignment);
	virtual void SetAlignment(UINT uAlignment, UINT uVAlignment);

	virtual void SetFont(CString strFont = _T(""), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);

	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);
	void SetBitmapCount(int nCount);
	BOOL SetImage(CString strImage);
	HRESULT OnAttributeFont(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeImage(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeSkin(const CStringA& strValue, BOOL bLoading);

protected:
	CString				m_strTitle;			// ����
	CString				m_strFont;			// ����
	int					m_nFontWidth;		// �������
	FontStyle			m_fontStyle;		// ����Style
	UINT				m_uAlignment;		// ˮƽ���뷽ʽ
	UINT				m_uVAlignment;		// ��ֱ���뷽ʽ

	Image*				m_pImage;			// ͼƬ
	CSize				m_sizeImage;		// ͼƬ��С
	int					m_nImagePicCount;	// Image��Ĭ�ϰ�����ͼƬ����

	DUI_DECLARE_ATTRIBUTES_BEGIN()
        DUI_TSTRING_ATTRIBUTE("title", m_strTitle, FALSE)
		DUI_CUSTOM_ATTRIBUTE("font", OnAttributeFont)
		DUI_TSTRING_ATTRIBUTE("fontname", m_strFont, FALSE)
		DUI_INT_ATTRIBUTE("fontwidth", m_nFontWidth, FALSE)
		DUI_ENUM_ATTRIBUTE("valign", UINT, TRUE)
            DUI_ENUM_VALUE("top", VAlign_Top)
            DUI_ENUM_VALUE("middle", VAlign_Middle)
            DUI_ENUM_VALUE("bottom", VAlign_Bottom)
        DUI_ENUM_END(m_uVAlignment)
        DUI_ENUM_ATTRIBUTE("align", UINT, TRUE)
            DUI_ENUM_VALUE("left", Align_Left)
            DUI_ENUM_VALUE("center", Align_Center)
            DUI_ENUM_VALUE("right", Align_Right)
        DUI_ENUM_END(m_uAlignment)
		DUI_CUSTOM_ATTRIBUTE("image", OnAttributeImage)
		DUI_INT_ATTRIBUTE("img-count", m_nImagePicCount, FALSE)
		DUI_CUSTOM_ATTRIBUTE("skin", OnAttributeSkin)
    DUI_DECLARE_ATTRIBUTES_END()
};