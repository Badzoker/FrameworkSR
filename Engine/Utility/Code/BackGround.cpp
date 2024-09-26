#include "pch.h"
#include "..\Header\BackGround.h"
#include "Export_Utility.h"
#include "Export_System.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 _pGraphicDev)
	: Engine::CGameObject(_pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pAnimator(nullptr)
{
}

CBackGround::~CBackGround()
{
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	CBackGround* pBackGround = new CBackGround(_pGraphicDev);

	if (FAILED(pBackGround->Ready_GameObject()))
	{
		Safe_Release(pBackGround);
		MSG_BOX("pBackGround Create Failed");
		return nullptr;
	}

	return pBackGround;
}

HRESULT CBackGround::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	Set_Animation();

	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& _fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(_fTimeDelta);

	Engine::Add_RenderGroup(RENDERID::RENDER_PRIORITY, this);

	return iExit;
}

void CBackGround::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CBackGround::Render_GameObject()
{
	m_pAnimator->Render_Animator();
	m_pBufferCom->Render_Buffer();
}

HRESULT CBackGround::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[(_uint)COMPONENTID::ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_Loading"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[(_uint)COMPONENTID::ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pAnimator = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[(_uint)COMPONENTID::ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CBackGround::Set_Animation()
{
	m_pAnimator->CreateAnimation(L"Loading", m_pTextureCom, 3.f);

	m_pAnimator->PlayAnimation(L"Loading", true);
}

void CBackGround::Free()
{
	Engine::CGameObject::Free();
}