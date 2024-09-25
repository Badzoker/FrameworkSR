#include "ParticleSystem.h"
#include "GameObject.h"
#include "Transform.h"

// ����ü ������ ���� ������ �ֳ׿� LSB ������ ����ü �����ؾ���
//const DWORD PARTICLE::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE;
const DWORD PARTICLE::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;


// d3d::Util
DWORD Engine::FtoDW(_float _f)
{
	return *((DWORD*)&_f);
}

_float Engine::Get_RandomFloat(_float _fMin, _float _fMax)
{
	_float fNum = rand() % 1000 * 0.001f;
	return fNum * (_fMax - _fMin) + _fMin;
}

_vec3 Engine::Get_RandomVec3(_vec3 _vMin, _vec3 _vMax)
{
	_vec3 vReturn;
	vReturn.x = Get_RandomFloat(_vMin.x, _vMax.x);
	vReturn.y = Get_RandomFloat(_vMin.y, _vMax.y);
	vReturn.z = Get_RandomFloat(_vMin.z, _vMax.z);

	return vReturn;
}

_vec3 Engine::Get_RandomVec3(BOUNDINGBOX tBox)
{
	return Engine::Get_RandomVec3(tBox.vMin, tBox.vMax);
}

CParticleSystem::CParticleSystem()
{
}

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 _pGraphicDev)
	: CComponent(_pGraphicDev)
	, m_dwOptions(0)
{
	ZeroMemory(&m_tParam, sizeof(CParticleSystem::PARAM));
}

CParticleSystem::CParticleSystem(const CParticleSystem& _rhs)
	: CComponent(_rhs)
	, m_dwBufferOffset(_rhs.m_dwBufferOffset)
	, m_dwbufferBatchSize(_rhs.m_dwbufferBatchSize)
	, m_dwBufferSize(_rhs.m_dwBufferSize)
	, m_pVB(_rhs.m_pVB)
	, m_tParam(_rhs.m_tParam)
	, m_dwOptions(_rhs.m_dwOptions)
{
	//m_ParticleList.assign(_rhs.m_ParticleList.begin(), _rhs.m_ParticleList.end());
}

CParticleSystem::~CParticleSystem()
{
}

CParticleSystem* CParticleSystem::Create(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	CParticleSystem* pParticleSystem = new CParticleSystem(_pGraphicDev);

	if (FAILED(pParticleSystem->Ready_ParticleSystem()))
	{
		Safe_Release(pParticleSystem);
		MSG_BOX("particle system create failed");
		return nullptr;
	}

	return pParticleSystem;
}

HRESULT CParticleSystem::Ready_ParticleSystem()
{
	m_dwBufferOffset = 0;
	m_dwbufferBatchSize = 128;
	m_dwBufferSize = 2048;

	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer
	(
		m_dwBufferSize * sizeof(VTXPARTICLE),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		VTXPARTICLE::FVF,
		D3DPOOL_DEFAULT,
		&m_pVB,
		NULL
	), E_FAIL);

	return S_OK;
}

_int CParticleSystem::Update_Component(const _float& _fTimeDelta)
{
	//Temp snow code
	std::list<PARTICLEINFO>::iterator iter;
	for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); ++iter)
	{
		if (!iter->bIsAlive)
			continue;
		iter->fAge += _fTimeDelta;

		// death over boudary
		if (Check_Option(OPTION::DEATH_OVER_BOUNDARY))
		{
			if (iter->vPosition.y <= -3.f ||
				iter->vPosition.y >= 20.f ||
				iter->vPosition.x >= 20.f ||
				iter->vPosition.x <= -10.f)
				iter->bIsAlive = FALSE;
		}

		if (m_dwOptions & 1 << (_ulong)OPTION::DEATH_OVER_TIME)
		{
			if (iter->fAge > iter->fLifeTime)
				iter->bIsAlive = FALSE;
		}
		if (m_dwOptions & 1 << (_ulong)OPTION::GRAVITY)
		{
			iter->vVelocity.y += -m_tParam.fGravity * _fTimeDelta;
		}

		iter->vVelocity += iter->vAcceleration * _fTimeDelta;
		iter->vPosition += iter->vVelocity * _fTimeDelta;
	}

	if (m_dwOptions & 1 << (_ulong)OPTION::REPEAT)
	{
		for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); ++iter)
		{
			if (!iter->bIsAlive)
				Reset_Particle(&(*iter));
		}
	}

	return 0;
}

void CParticleSystem::LateUpdate_Component()
{

#pragma region ALPHA_SORTING
	// Get Camera Pos
	_vec3 vCameraPos = { 0, 0, 0.f };
	_matrix matCamWorld{};
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

	memcpy(&vCameraPos, &matCamWorld.m[3][0], sizeof(_vec3));

	// ķ�� ���÷� �����ϴ���, ��ƼŬ���� ���� ����� �̵��ϴ��� �ʿ���.
	// ���� ķ�� ���÷� �����ϴ°� ���귮�� ��������,
	// ��¶�� ���� ���� ���� ������Ʈ�� ���� ��ǥ�� �ʿ��� !
	// �̷��� Get_owner()�� �����޶�����!
	CGameObject* pGameObject = GetOwner();
	CComponent* pTransformCom = pGameObject->Get_Component(COMPONENTID::ID_DYNAMIC, L"Com_Transform");
	_matrix matInversedWorld = *(dynamic_cast<CTransform*>(pTransformCom)->Get_WorldMatrix());
	D3DXMatrixInverse(&matInversedWorld, 0, &matInversedWorld);

	D3DXVec3TransformCoord(&vCameraPos, &vCameraPos, &matInversedWorld);

	m_ParticleList.sort([this, &vCameraPos](const PARTICLEINFO& _tSrc, const PARTICLEINFO& _tDst)->bool
		{
			return this->Compute_ParticleViewZ(_tSrc, vCameraPos) > this->Compute_ParticleViewZ(_tDst, vCameraPos);
		});
#pragma endregion
}

void CParticleSystem::Render_Parcitle()
{
	if (m_ParticleList.empty())
		return;

	Set_PreRenderState();

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXPARTICLE));
	m_pGraphicDev->SetFVF(VTXPARTICLE::FVF);
	// m_pGraphicDev->SetTexture(0, m_pTexture);

	if (m_dwBufferOffset >= m_dwBufferSize)
		m_dwBufferOffset = 0;

	VTXPARTICLE* pParticle(nullptr);

	m_pVB->Lock(m_dwBufferOffset * sizeof(VTXPARTICLE),
		m_dwbufferBatchSize * sizeof(VTXPARTICLE),
		(void**)&pParticle,
		D3DLOCK_DISCARD);
	//m_dwBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	_ulong dwNumParticleInBatch(0);

	std::list<PARTICLEINFO>::iterator iter;
	for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); ++iter)
	{
		if (!iter->bIsAlive)
			continue;

		pParticle->vPosition = iter->vPosition;
		//pParticle->fSize = 20.f;
		//pParticle->dwColor = D3DCOLOR_COLORVALUE(
		//	iter->vColor.x,
		//	iter->vColor.y,
		//	iter->vColor.z,
		//	iter->vColor.w);
		pParticle->dwColor = iter->tColor;
		pParticle->vUV = { 0.5f, 1.5f };

		++pParticle;

		++dwNumParticleInBatch;

		if (dwNumParticleInBatch == m_dwbufferBatchSize)
		{
			m_pVB->Unlock();

			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_dwBufferOffset, m_dwbufferBatchSize);

			m_dwBufferOffset += m_dwbufferBatchSize;

			if (m_dwBufferOffset >= m_dwBufferSize)
				m_dwBufferOffset = 0;

			m_pVB->Lock(m_dwBufferOffset * sizeof(VTXPARTICLE),
				m_dwbufferBatchSize * sizeof(VTXPARTICLE),
				(void**)&pParticle,
				// ����� ���� ������K�� �� �����Ѱǵ� ? ��¥ �� �̰� ���ؾȵ�
				m_dwBufferOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			dwNumParticleInBatch = 0;
		}
	}

	m_pVB->Unlock();


	if (dwNumParticleInBatch > 0)
	{
		m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_dwBufferOffset, dwNumParticleInBatch);
	}

	m_dwBufferOffset += m_dwbufferBatchSize;

	Set_PostRenderState();
}

CParticleSystem* CParticleSystem::Clone()
{
	return new CParticleSystem(*this);
}

void CParticleSystem::Free()
{
	Safe_Release(m_pVB);
	CComponent::Free();
}

void CParticleSystem::Set_PreRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	// zwrite // ��������� �ϴµ� ��� ���� �������� ����
	if (Check_Option(OPTION::ZWRITE))
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// Alpha Test
	if (Check_Option(OPTION::ALPHATEST))
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, m_tParam.dwAlphaRef); // if alpha == 0 ? dicard :  
	}


	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE); // ī�޶� ��ġ�� ���� �۰� �׸�����

	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_tParam.fSize));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f));
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(20.0f));

	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_CONSTANT, m_tParam.tColor);
	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(255, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// render group ALPHA
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);



}

void CParticleSystem::Set_PostRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);

	if (Check_Option(OPTION::ALPHATEST))
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	if (Check_Option(OPTION::ZWRITE))
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}

void CParticleSystem::Reset()
{
	std::list<PARTICLEINFO>::iterator iter;
	for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); ++iter)
		Reset_Particle(&(*iter));
}

void CParticleSystem::Reset_Particle(PARTICLEINFO* _pInfo)
{
	_pInfo->bIsAlive = TRUE;

	_pInfo->vPosition = Get_RandomVec3(m_tParam.tStartBoundary);

	_pInfo->vVelocity = m_tParam.vInitVelocity + Get_RandomVec3(-m_tParam.vVelocityNoise, m_tParam.vVelocityNoise);
	_pInfo->vAcceleration = m_tParam.vAcceleration;

	_pInfo->tColor = m_tParam.tColor;
	_pInfo->fLifeTime = m_tParam.fLifeTime;
	_pInfo->fAge = 0.f;
}

void CParticleSystem::Add_Particle()
{
	PARTICLEINFO tParticleInfo;
	Reset_Particle(&tParticleInfo);
	m_ParticleList.push_back(tParticleInfo);
}

_bool CParticleSystem::Is_Empty()
{
	return m_ParticleList.empty();
}

_bool CParticleSystem::Is_Dead()
{
	std::list<PARTICLEINFO>::iterator iter;
	for (iter = m_ParticleList.begin(); iter != m_ParticleList.end(); ++iter)
	{
		if (iter->bIsAlive)
			return FALSE;
	}

	return TRUE;
}

void CParticleSystem::Remove_DeadParticles()
{
	std::list<PARTICLEINFO>::iterator iter;

	iter = m_ParticleList.begin();

	while (iter != m_ParticleList.end())
	{
		if (iter->bIsAlive == FALSE)
		{
			iter = m_ParticleList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

_float CParticleSystem::Compute_ParticleViewZ(const PARTICLEINFO& _tParticle, const _vec3& _vCameraPos)
{
	_vec3 vDist = _tParticle.vPosition - _vCameraPos;
	return D3DXVec3Length(&vDist);
}
