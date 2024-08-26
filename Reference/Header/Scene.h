#pragma once

#include "Base.h"
#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene 
	: public CBase
{
protected :
	explicit CScene(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CScene();

public :
	CComponent* Get_Component(COMPONENTID _eID, const _tchar* _pLayerTag, const _tchar* _pObjTag, const _tchar* _pComponentTag);

public :
	virtual	HRESULT	Ready_Scene();
	virtual	_int Update_Scene(const _float& _fTimeDelta);
	virtual	void LateUpdate_Scene();
	virtual	void Render_Scene();

protected :
	virtual	void Free();

protected :
	map<const _tchar*, CLayer*>	m_mapLayer;
	LPDIRECT3DDEVICE9 m_pGraphicDev;
};

END