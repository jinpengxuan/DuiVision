#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"
[!if TAB_CHECK_1]
#include "DuiHandler[!output TAB_CLASS_1].h"
[!else]
#include "DuiHandlerHome.h"
[!endif]
[!if TAB_CHECK_2]
#include "DuiHandler[!output TAB_CLASS_2].h"
[!endif]
[!if TAB_CHECK_3]
#include "DuiHandler[!output TAB_CLASS_3].h"
[!endif]
[!if TAB_CHECK_4]
#include "DuiHandler[!output TAB_CLASS_4].h"
[!endif]
[!if TAB_CHECK_5]
#include "DuiHandler[!output TAB_CLASS_5].h"
[!endif]
[!if TAB_CHECK_6]
#include "DuiHandler[!output TAB_CLASS_6].h"
[!endif]

//////////////////////////////////////////////////////////////
// CDuiHandlerMain

CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerMain::~CDuiHandlerMain(void)
{
}

// ��ʼ��
void CDuiHandlerMain::OnInit()
{
[!if OPTION_CHECK_TRAYICON]
	// ��ʼ������ͼ��
	DuiSystem::Instance()->InitTray();
[!endif]
	
[!if TAB_CHECK_1]
	// ��tabҳ'[!output TAB_TITLE_1]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_1]* pDuiHandler[!output TAB_CLASS_1] = new CDuiHandler[!output TAB_CLASS_1]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_1], _T("tab.[!output TAB_CLASS_1]"));
	pDuiHandler[!output TAB_CLASS_1]->OnInit();
[!else]
	CDuiHandlerHome* pDuiHandlerHome = new CDuiHandlerHome();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandlerHome, _T("tab.Home"));
	pDuiHandlerHome->OnInit();
[!endif]
[!if TAB_CHECK_2]
	// ��tabҳ'[!output TAB_TITLE_2]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_2]* pDuiHandler[!output TAB_CLASS_2] = new CDuiHandler[!output TAB_CLASS_2]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_2], _T("tab.[!output TAB_CLASS_2]"));
	pDuiHandler[!output TAB_CLASS_2]->OnInit();
[!endif]
[!if TAB_CHECK_3]
	// ��tabҳ'[!output TAB_TITLE_3]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_3]* pDuiHandler[!output TAB_CLASS_3] = new CDuiHandler[!output TAB_CLASS_3]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_3], _T("tab.[!output TAB_CLASS_3]"));
	pDuiHandler[!output TAB_CLASS_3]->OnInit();
[!endif]
[!if TAB_CHECK_4]
	// ��tabҳ'[!output TAB_TITLE_4]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_4]* pDuiHandler[!output TAB_CLASS_4] = new CDuiHandler[!output TAB_CLASS_4]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_4], _T("tab.[!output TAB_CLASS_4]"));
	pDuiHandler[!output TAB_CLASS_4]->OnInit();
[!endif]
[!if TAB_CHECK_5]
	// ��tabҳ'[!output TAB_TITLE_5]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_5]* pDuiHandler[!output TAB_CLASS_5] = new CDuiHandler[!output TAB_CLASS_5]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_5], _T("tab.[!output TAB_CLASS_5]"));
	pDuiHandler[!output TAB_CLASS_5]->OnInit();
[!endif]
[!if TAB_CHECK_6]
	// ��tabҳ'[!output TAB_TITLE_6]'ע���¼��������
	CDuiHandler[!output TAB_CLASS_6]* pDuiHandler[!output TAB_CLASS_6] = new CDuiHandler[!output TAB_CLASS_6]();
	DuiSystem::RegisterHandler(m_pDlg, pDuiHandler[!output TAB_CLASS_6], _T("tab.[!output TAB_CLASS_6]"));
	pDuiHandler[!output TAB_CLASS_6]->OnInit();
[!endif]

	// ����������ʱ��
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

[!if OPTION_CHECK_SKINWND]
// Ƥ����Ϣ����(ʵ��Ƥ���ı���ͻ�ȡ)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// ��ȡSkin����
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// ��ȡSkinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// ����Skinֵ
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}
[!endif]

[!if OPTION_CHECK_TRAYICON]
// ����˫����Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgTrayIconDClick(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DuiSystem::ShowDuiDialog(_T("dlg_login"), NULL);
	return TRUE;
}
[!endif]

// DUI��ʱ���¼�����
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

// ���̼���Ϣ����
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// �����в���,���Զ������в������д���,Ҳ����ֱ����ʾ������
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, _T("ִ���������в���:") + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(_T("dlg_main"));
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
	return TRUE;
}
