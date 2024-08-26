#pragma once

#include "Scene.h"
#include "BackGround.h"

class CLogo 
	: public Engine::CScene
{
private :
	explicit CLogo(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CLogo();

public :
	static	CLogo* Create(LPDIRECT3DDEVICE9 _pGraphicDev);

public :
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(const _float& _fTimeDelta);
	virtual void LateUpdate_Scene();
	virtual void Render_Scene();

private:
	HRESULT	Ready_Prototype();

private :
	HRESULT Ready_Layer_Environment(const _tchar* _pLayerTag);
	HRESULT Ready_GameLogic_Environment(const _tchar* _pLayerTag) { return S_OK; }
	HRESULT Ready_UI_Environment(const _tchar* _pLayerTag) { return S_OK; }

private :
	virtual void Free();
};