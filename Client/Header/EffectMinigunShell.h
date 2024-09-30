#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CEffect;

END

class CEffectMinigunShell : public CGameObject
{
private:
	enum Obj
	{
		CAPSULE,
		BULLET,

		OBJ_END
	};

private:
	explicit CEffectMinigunShell(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CEffectMinigunShell();

public:
	virtual HRESULT	Ready_GameObject();
	virtual	_int	Update_GameObject(const _float& _fTimeDelta);
	virtual	void	LateUpdate_GameObject();
	virtual	void	Render_GameObject();

public:
	static CEffectMinigunShell* Create(LPDIRECT3DDEVICE9 _pGraphicDev);

private:
	virtual void Free();

private:
	HRESULT Add_Component();

public:
	static void OnOperate(void* _pParam);

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom[OBJ_END];
	Engine::CTexture* m_pTextureCom[OBJ_END];
	Engine::CEffect* m_pEffectCom;

	_uint m_iTotalFrame[OBJ_END];
	_uint m_iCurFrame[OBJ_END];

	_vec3 m_vVelocity[OBJ_END];
	_vec3 m_vVelocityNoise[OBJ_END];

	_uint m_iRandomSpriteOffset;

	// ���� �� Ŭ������ "Com_Transform" �̶�� ������Ʈ �±׸� ������� �ʴ´�.
	// �׷��� ����� ������ obj pool �� ����ϰ� �Ǹ� transform ������Ʈ�� ã�� ���Ѵ�.
	// �������ʹ� �θ� - �ڽ� ���� transform ������Ʈ�� �̿��ϵ��� ���� Ŭ������ Com_Transform �±׸� ������ ������Ʈ ���� ��
	// ���⿡ ������ ó�� ������ �־�� �ڴ�? ������ ���� ������ ���ٰ��̴�.
	// Render �Լ����� (������ ���� ��� * ���� ���� ���) �� �������� ���� ��ġ ���� �� ���� ���̴�.
	// �� Ŭ���������� ���� ��ġ�� ���� transform ������Ʈ�� �� ���̴�.
};

