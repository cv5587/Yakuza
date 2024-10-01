#include "Subtitle.h"

#include "GameInstance.h"
#include "ScriptManager.h"

CSubtitle::CSubtitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSubtitle::CSubtitle(const CSubtitle& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CSubtitle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubtitle::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	SUBTITLE_DESC Desc{};
	m_iScriptChapter = Desc.iScriptChapter;

	m_pScriptManager = CScriptManager::Create();
	if (m_pScriptManager == nullptr)
		return E_FAIL;

	m_fY = 20.f;

	return S_OK;
}

void CSubtitle::Priority_Tick(const _float& fTimeDelta)
{
}

void CSubtitle::Tick(const _float& fTimeDelta)
{
	if (m_isFinished || !m_isStart)
		return;

	if (m_fDuration < m_fScriptTime)
	{
		CScriptManager::SCRIPT_INFO Info = m_pScriptManager->Get_Script(m_iScriptChapter, m_iScriptIndex);
		m_strScript = Info.strLine;
		
		if (Info.strName == TEXT("³¡"))
		{
			m_isFinished = true;
			return;
		}

		m_iScriptIndex++;
		m_fScriptTime = 0;
	}
}

void CSubtitle::Late_Tick(const _float& fTimeDelta)
{
	if (!m_isFinished || m_isStart)
		m_pGameInstance->Add_Renderer(CRenderer::RENDER_UI, this);
}

HRESULT CSubtitle::Render()
{
	_float2 vPos;
	vPos.x = g_iWinSizeX * 0.5f;

	vPos.y = -m_fY + g_iWinSizeY;

	m_pGameInstance->AlignRender_Font(TEXT("Font_nanum24"), m_strScript, vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0);

	return S_OK;
}

CSubtitle* CSubtitle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSubtitle* pInstance = new CSubtitle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CSubtitle::Clone(void* pArg)
{
	CSubtitle* pInstance = new CSubtitle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSubtitle::Free()
{
	__super::Free();
	
	Safe_Release(m_pScriptManager);
}
