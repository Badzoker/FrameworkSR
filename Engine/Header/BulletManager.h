#pragma once
#include "Base.h"
#include "Bullet.h"

BEGIN(Engine)

class ENGINE_DLL CBulletManager 
	: public CBase
{
	DECLARE_SINGLETON(CBulletManager)
public:
	enum BULLETTYPE { BULLET_PISTOL, BULLET_SHOTGUN, BULLET_LASER, BULLET_MISSILE, BULLET_MINIGUN, BULLET_HEAD, BULLET_END };
private:
	explicit CBulletManager();
	virtual ~CBulletManager();

public:
	HRESULT Add_Bullet(CBullet* _pBullet);
	HRESULT Add_Missile(CBullet* _pMissile);
	HRESULT Add_Laser(CBullet* _pLaser);
	HRESULT Add_MiniGun(CBullet* _pMiniGun);
	HRESULT Add_HumanoidHead(CBullet* _pHead);
	virtual HRESULT Fire_Bullet(LPDIRECT3DDEVICE9 _pGraphicDev, const _vec3& _vStartPos, const _vec3& _vDir, const _float& _fAttackDamage, CBulletManager::BULLETTYPE _eBulletType, const _bool& _bIsBoss, const _vec3& vCurvePos);

public:
	_int Update_Bullet(const _float& _fTimeDelta);
	void LateUpdate_Bullet();

public:
	virtual void Free();

private:
	vector<CBullet*> m_vecBullet;
	vector<CBullet*> m_vecMissile;
	vector<CBullet*> m_vecLaser;
	vector<CBullet*> m_vecMiniGun;
	vector<CBullet*> m_vecHead;
};

END