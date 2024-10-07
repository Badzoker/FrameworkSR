#pragma once
#include "TileContorl.h"
BEGIN(Engine)

class CWallTBTex;
class CTransform;
class CTexture;
class CCollider;

END
class CWallTB : public CTileContorl
{
private:
	explicit CWallTB(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CWallTB();

public:
	static CWallTB* Create_InfoNumberDirectionTrigger(LPDIRECT3DDEVICE9 _pGraphicDev, _vec3 _vecPos, const _int& _iNumber, Engine::TILE_DIRECTION _eTileDirection, const _int& _iTrigger);
	static CWallTB* Create_InfoNumberDirectionTrigger2(LPDIRECT3DDEVICE9 _pGraphicDev, _vec3 _vecPos, _vec3 _vecRot, const _int& _iNumber, const _int& _iTrigger);

	virtual _vec3 Get_VecPos() { return m_vecPos; }
	virtual _int Get_Number() { return m_iNumber; }
	virtual _int Get_Number_Type() { return m_iNumber_Type; }
	_int Get_Trigger() { return m_iTriggerNumber; }//10.06
	_vec3 Get_TileDirection() {	return m_vecWallDirection;	}
	void Set_TileDirection(const _vec3& _vecDir);
	void Set_Number(const _int& _iNumber) { m_iNumber = _iNumber; }
	void Set_Trigger(const _int& _iTrigger) { m_iTriggerNumber = _iTrigger; }//10.06

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int Update_GameObject(const _float& _fTimeDelta);
	virtual void LateUpdate_GameObject();
	virtual void Render_GameObject();
	void Set_TileDirection(Engine::TILE_DIRECTION _eTileDirection);

private:
	virtual HRESULT Add_Component();
	virtual HRESULT Setup_Material();
	void Setup_Position(_vec3 _vecPos);
	void Setup_Angle(_vec3 _vecRot);

private:
	virtual void Free();

private:
	Engine::CWallTBTex* m_pBufferCom;
	Engine::CCollider* m_pColliderCom;
	_vec3 m_vecWallDirection;
};

