#include "WallTBTex.h"

CWallTBTex::CWallTBTex()
	: m_pPos(nullptr)
{
}

CWallTBTex::CWallTBTex(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CVIBuffer(_pGraphicDev)
	, m_pPos(nullptr)
{
}

CWallTBTex::CWallTBTex(const CWallTBTex& _rhs)
	:CVIBuffer(_rhs)
	, m_pPos(_rhs.m_pPos)
{
}

CWallTBTex::~CWallTBTex()
{
}

CWallTBTex* CWallTBTex::Create(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	CWallTBTex* pInstance = new CWallTBTex(_pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("WallTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CWallTBTex::Ready_Buffer()
{
	m_dwTriCnt = 2;
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_eIdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = { 0.f, 2.f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { 2.f, 2.f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { 2.f, 0.f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { 0.f, 0.f, 0.f };
	pVertex[3].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();


	return S_OK;
}

void CWallTBTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CWallTBTex::Clone()
{
	return new CWallTBTex(*this);
}

void CWallTBTex::Free()
{
}
