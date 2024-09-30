#include "pch.h"
#include "../Header/MapCreate.h"
#include "..\Header\MapCamera.h"
#include "..\Header\SkyBox.h"
#include "..\Header\Floor.h"
#include "..\Header\Wall.h"
#include "..\Header\WallTB.h"
#include "..\Header\GuideTerrain.h"


CMapCreate::CMapCreate(LPDIRECT3DDEVICE9 _pGraphicDev)
	: Engine::CScene(_pGraphicDev)
	, m_bCreateCheck(false)
{
}

CMapCreate::~CMapCreate()
{
}

CMapCreate* CMapCreate::Create(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	CMapCreate* pMapCreate = new CMapCreate(_pGraphicDev);

	if (FAILED(pMapCreate->Ready_Scene()))
	{
		Safe_Release(pMapCreate);
		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pMapCreate;
}

HRESULT CMapCreate::Ready_Scene()
{

	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_ToolEnvironment(L"Layer_ToolEnvironment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Terrain(L"Layer_GuideTerrain"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_PickingTile(L"Layer_PickingTile"), E_FAIL);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;

}

_int CMapCreate::Update_Scene(const _float& _fTimeDelta)
{
	if (Engine::Key_Hold(DIK_Z) && Engine::Mouse_Press(MOUSEKEYSTATE::DIM_LB))
	{
		PickingTile_PosCheck(Find_Layer_PickingTile());
		// �ٴ� Ÿ�� ����
		if (m_bCreateCheck == true)
		{
			Create_Layer_PickingFloor(Find_Layer_PickingTile());

		}
	}
	if (Engine::Key_Hold(DIK_X) && Engine::Mouse_Press(MOUSEKEYSTATE::DIM_LB))
	{
		Create_Layer_PickingWall(Find_Layer_PickingTile());
	}
	if (Engine::Key_Hold(DIK_C) && Engine::Mouse_Press(MOUSEKEYSTATE::DIM_LB))
	{
		Create_Layer_PickingWallTB(Find_Layer_PickingTile());
	}
	if (Engine::Mouse_Press(MOUSEKEYSTATE::DIM_RB))
	{
		//������ Ÿ�� ����
		PickingTile_PosDelete(Find_Layer_PickingTile());
	}
	if (Engine::Key_Press(DIK_O))
	{
		MapSave(Find_Layer_PickingTile());
	}
	if (Engine::Key_Press(DIK_P))
	{
		MapLoad(Find_Layer_PickingTile());
	}
	if (Engine::Key_Press(DIK_K))
	{

	}
	if (Engine::Key_Press(DIK_L))
	{

	}

	_int iExit = Engine::CScene::Update_Scene(_fTimeDelta);

	return iExit;
}

void CMapCreate::LateUpdate_Scene()
{
	Engine::CScene::LateUpdate_Scene();

}

void CMapCreate::Render_Scene()
{
	ShowGUI();
	ImGui::Render();
}

HRESULT CMapCreate::Ready_LightInfo()
{
	D3DLIGHT9 tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tLightInfo.Specular = { 1.f, 1.f, 1.f, 1.f };
	tLightInfo.Ambient = { 1.f, 1.f, 1.f, 1.f };
	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

HRESULT CMapCreate::Ready_Layer_ToolEnvironment(const _tchar* _pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	_vec3 vEye(0.f, 10.f, -10.f);
	_vec3 vAt(0.f, 0.f, 1.f);
	_vec3 vUp(0.f, 1.f, 0.f);

	pGameObject = CMapCamera::Create
	(
		m_pGraphicDev,
		&vEye,
		&vAt,
		&vUp
	);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);
	//dynamic_cast<CDynamicCamera*>(pGameObject)->Toggle_Active();


	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);


	m_mapLayer.insert({ _pLayerTag , pLayer });


	return S_OK;
}

HRESULT CMapCreate::Ready_Layer_Terrain(const _tchar* _pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CGuideTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"GuideTerrain", pGameObject), E_FAIL);

	m_mapLayer.insert({ _pLayerTag , pLayer });

	return S_OK;
}

HRESULT CMapCreate::Ready_Layer_PickingTile(const _tchar* _pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_mapLayer.insert({ _pLayerTag, pLayer });

	return S_OK;
}

CLayer* CMapCreate::Find_Layer_PickingTile()
{
	CLayer* pLayer = nullptr;

	auto iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_PickingTile"));
	pLayer = iter->second;

	return pLayer;
}

HRESULT CMapCreate::Create_Layer_PickingFloor(CLayer* _pLayer)
{
	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CFloor::Create_Info(m_pGraphicDev, TilePiking_OnTerrain(1),L"Proto_FirstFloor");
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	_pLayer->Add_GameObject(L"Floor", pGameObject);

	return S_OK;
}

HRESULT CMapCreate::Create_Layer_PickingWall(CLayer* _pLayer)
{
	Engine::CGameObject* pGameObject = nullptr;
	pGameObject = CWall::Create_Pos(m_pGraphicDev, TilePiking_OnTerrain(2));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	_pLayer->Add_GameObject(L"Wall", pGameObject);

	return S_OK;
}

HRESULT CMapCreate::Create_Layer_PickingWallTB(CLayer* _pLayer)
{
	Engine::CGameObject* pGameObject = nullptr;
	pGameObject = CWallTB::Create_Pos(m_pGraphicDev, TilePiking_OnTerrain(3));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	_pLayer->Add_GameObject(L"WallTB", pGameObject);

	return S_OK;
}

_vec3 CMapCreate::TilePiking_OnTerrain(int _iTile)
{
	CGuideTex* pGuideBufferCom = dynamic_cast<CGuideTex*>(Engine::Get_Component(Engine::COMPONENTID::ID_STATIC, L"Layer_GuideTerrain", L"GuideTerrain", L"Com_Buffer"));
	NULL_CHECK_RETURN(pGuideBufferCom, _vec3());
	//���⿡ �̳Ѱ����� ����ġ �� �־ ������ �� �ֵ���	
	switch (_iTile)
	{
	case 1:
		return FloorCreate_OnTerrain(g_hWnd, pGuideBufferCom);
		break;
	case 2:
		return WallCreate_OnTerrain1(g_hWnd, pGuideBufferCom);
		break;
	case 3:
		return WallCreate_OnTerrain2(g_hWnd, pGuideBufferCom);
		break;
	}
	
}

_vec3 CMapCreate::FloorCreate_OnTerrain(HWND _hWnd, CGuideTex* _pGuideBufferCom)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(_hWnd, &ptMouse);

	_vec3	vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3		vRayPos, vRayDir;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMousePos - vRayPos;

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	const _vec3* pGuideTexPos = _pGuideBufferCom->Get_VtxPos();


	_ulong	dwVtxId[3]{};
	_float	fU, fV, fDist;

	for (_ulong i = 0; i < VTXTILEZ - 1; i++)
	{
		for (_ulong j = 0; j < VTXTILEX - 1; j++)
		{
			_ulong	dwIndex = i * VTXTILEX + j;

			dwVtxId[0] = dwIndex + VTXTILEX;
			dwVtxId[1] = dwIndex + VTXTILEX + 1;
			dwVtxId[2] = dwIndex + 1;

			if (D3DXIntersectTri
			(
				// �ͷ��� -> ���ؽ� �ε����� �׸��� ������ �����ϰ� ���� 231
				&pGuideTexPos[dwVtxId[1]],
				&pGuideTexPos[dwVtxId[2]],
				&pGuideTexPos[dwVtxId[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist
			))
			{
				return _vec3
				(
					(pGuideTexPos[dwVtxId[1]].x + fU * (pGuideTexPos[dwVtxId[2]].x - pGuideTexPos[dwVtxId[1]].x)) - 1,
					0.f,
					(pGuideTexPos[dwVtxId[1]].z + fV * (pGuideTexPos[dwVtxId[0]].z - pGuideTexPos[dwVtxId[1]].z) - 1)
				);
			}
			// �ͷ��� -> ���ؽ� �ε����� �׸��� ������ �����ϰ� ���� 210
			dwVtxId[0] = dwIndex + VTXTILEX;
			dwVtxId[1] = dwIndex + 1;
			dwVtxId[2] = dwIndex;

			if (D3DXIntersectTri
			(
				&pGuideTexPos[dwVtxId[2]],
				&pGuideTexPos[dwVtxId[0]],
				&pGuideTexPos[dwVtxId[1]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist
			))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)

				return _vec3
				(
					pGuideTexPos[dwVtxId[2]].x + fU * (pGuideTexPos[dwVtxId[0]].x - pGuideTexPos[dwVtxId[2]].x),
					0.f,
					pGuideTexPos[dwVtxId[2]].z + fV * (pGuideTexPos[dwVtxId[1]].z - pGuideTexPos[dwVtxId[2]].z)
				);
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);

}

_vec3 CMapCreate::WallCreate_OnTerrain1(HWND _hWnd, CGuideTex* _pGuideBufferCom)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(_hWnd, &ptMouse);

	_vec3	vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3		vRayPos, vRayDir;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMousePos - vRayPos;

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	const _vec3* pGuideTexPos = _pGuideBufferCom->Get_VtxPos();


	_ulong	dwVtxId[3]{};
	_float	fU, fV, fDist;

	for (_ulong i = 0; i < VTXTILEZ - 1; i++)
	{
		for (_ulong j = 0; j < VTXTILEX - 1; j++)
		{
			_ulong	dwIndex = i * VTXTILEX + j;

			dwVtxId[0] = dwIndex + VTXTILEX;
			dwVtxId[1] = dwIndex + VTXTILEX + 1;
			dwVtxId[2] = dwIndex + 1;

			if (D3DXIntersectTri
			(
				// �ͷ��� -> ���ؽ� �ε����� �׸��� ������ �����ϰ� ���� 231
				&pGuideTexPos[dwVtxId[1]],
				&pGuideTexPos[dwVtxId[2]],
				&pGuideTexPos[dwVtxId[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist
			))
			{
				return _vec3
				(
					(pGuideTexPos[dwVtxId[1]].x + fU * (pGuideTexPos[dwVtxId[2]].x - pGuideTexPos[dwVtxId[1]].x)),
					0.f,
					(pGuideTexPos[dwVtxId[1]].z + fV * (pGuideTexPos[dwVtxId[0]].z - pGuideTexPos[dwVtxId[1]].z) - 1)
				);
			}
			// �ͷ��� -> ���ؽ� �ε����� �׸��� ������ �����ϰ� ���� 210
			dwVtxId[0] = dwIndex + VTXTILEX;
			dwVtxId[1] = dwIndex + 1;
			dwVtxId[2] = dwIndex;

			if (D3DXIntersectTri
			(
				&pGuideTexPos[dwVtxId[2]],
				&pGuideTexPos[dwVtxId[0]],
				&pGuideTexPos[dwVtxId[1]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist
			))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)
				return _vec3
				(
					pGuideTexPos[dwVtxId[2]].x + fU * (pGuideTexPos[dwVtxId[0]].x - pGuideTexPos[dwVtxId[2]].x),
					0.f,
					pGuideTexPos[dwVtxId[2]].z + fV * (pGuideTexPos[dwVtxId[1]].z - pGuideTexPos[dwVtxId[2]].z)
				);
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

_vec3 CMapCreate::WallCreate_OnTerrain2(HWND _hWnd, CGuideTex* _pGuideBufferCom)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(_hWnd, &ptMouse);

	_vec3	vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3		vRayPos, vRayDir;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMousePos - vRayPos;

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	const _vec3* pGuideTexPos = _pGuideBufferCom->Get_VtxPos();


	_ulong	dwVtxId[3]{};
	_float	fU, fV, fDist;

	for (_ulong i = 0; i < VTXTILEZ - 1; i++)
	{
		for (_ulong j = 0; j < VTXTILEX - 1; j++)
		{
			_ulong	dwIndex = i * VTXTILEX + j;

			dwVtxId[0] = dwIndex + VTXTILEX;
			dwVtxId[1] = dwIndex + VTXTILEX + 1;
			dwVtxId[2] = dwIndex + 1;

			if (D3DXIntersectTri
			(
				// �ͷ��� -> ���ؽ� �ε����� �׸��� ������ �����ϰ� ���� 231
				&pGuideTexPos[dwVtxId[1]],
				&pGuideTexPos[dwVtxId[2]],
				&pGuideTexPos[dwVtxId[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist
			))
			{
				return _vec3
				(
					(pGuideTexPos[dwVtxId[1]].x + fU * (pGuideTexPos[dwVtxId[2]].x - pGuideTexPos[dwVtxId[1]].x))-1,
					0.f,
					(pGuideTexPos[dwVtxId[1]].z + fV * (pGuideTexPos[dwVtxId[0]].z - pGuideTexPos[dwVtxId[1]].z))
				);
			}
			// �ͷ��� -> ���ؽ� �ε����� �׸��� ������ �����ϰ� ���� 210
			dwVtxId[0] = dwIndex + VTXTILEX;
			dwVtxId[1] = dwIndex + 1;
			dwVtxId[2] = dwIndex;

			if (D3DXIntersectTri
			(
				&pGuideTexPos[dwVtxId[2]],
				&pGuideTexPos[dwVtxId[0]],
				&pGuideTexPos[dwVtxId[1]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist
			))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)
				return _vec3
				(
					pGuideTexPos[dwVtxId[2]].x + fU * (pGuideTexPos[dwVtxId[0]].x - pGuideTexPos[dwVtxId[2]].x),
					0.f,
					pGuideTexPos[dwVtxId[2]].z + fV * (pGuideTexPos[dwVtxId[1]].z - pGuideTexPos[dwVtxId[2]].z)
				);
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

HRESULT CMapCreate::PickingTile_PosDelete(CLayer* _pLayer)
{
	multimap<const _tchar*, CGameObject*>::iterator it = _pLayer->Get_LayerObjects()->begin();
	while (it != _pLayer->Get_LayerObjects()->end())
	{
		if (dynamic_cast<CFloor*>((*it).second)->Get_VecPos() == TilePiking_OnTerrain(true))
		{
			_pLayer->Get_LayerObjects()->erase(it++);
		}
		else
		{
			++it;
		}
	}
	return S_OK;
}

void CMapCreate::PickingTile_PosCheck(CLayer* _pLayer)
{
	multimap<const _tchar*, CGameObject*>::iterator it = _pLayer->Get_LayerObjects()->begin();
	while (it != _pLayer->Get_LayerObjects()->end())
	{
		if (dynamic_cast<CFloor*>((*it).second)->Get_VecPos() == TilePiking_OnTerrain(true))
		{
			m_bCreateCheck = false;
			break;
		}
		else
		{
			m_bCreateCheck = true;
			++it;
		}
	}
	if (_pLayer->Get_LayerObjects()->size() == 0)
	{
		m_bCreateCheck = true;
	}
}

void CMapCreate::MapSave(CLayer* _pLayer)
{

	HANDLE		hFile = CreateFile(L"../Data/Stage1.txt",	// ���� �̸����� ���Ե� ���
		GENERIC_WRITE,		// ���� ���� ���(GENERIC_WRITE : ����, GENERIC_READ : �б�)
		NULL,				// ���� ���(������ ���� �ִ� ���¿��� �ٸ� ���μ����� ���� �� �� �㰡 �� ���ΰ�)
		NULL,				// ���� �Ӽ�
		CREATE_ALWAYS,		// ���� ���(CREATE_ALWAYS : ������ ���ٸ� ����, ������ �����, OPEN_EXISTING : ������ �ִ� ��쿡�� �ҷ�����)
		FILE_ATTRIBUTE_NORMAL, // ���� �Ӽ�(�ƹ��� �Ӽ��� ���� ���Ϸ� ����)
		NULL);				// ������ ������ �Ӽ��� ������ ���ø� ����

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Save File", _T("Fail"), MB_OK);
		return;
	}

	DWORD	dwByte(0);

	int iMapSize(0);
	iMapSize = _pLayer->Get_LayerObjects()->size();

	_vec3 test;
	_matrix teste;

	int iSize(0);

	multimap<const _tchar*, CGameObject*>::iterator it;
	for (it = _pLayer->Get_LayerObjects()->begin(); it != _pLayer->Get_LayerObjects()->end(); it++)
	{

		//teste = (dynamic_cast<CFloor*>((*it).second)->Get_FloorTransform()->Get_WorldMatrix());

		//WriteFile(hFile, (*it).first, wcslen((*it).first) * 2, &dwByte, nullptr);
		//WriteFile(hFile, dynamic_cast<CFloor*>((*it).second)->Get_FloorName(), wcslen((dynamic_cast<CFloor*>((*it).second)->Get_FloorName())) * 2, &dwByte, nullptr);
		
		if ((*it).first == L"Floor")
		{
			WriteFile(hFile, dynamic_cast<CFloor*>((*it).second)->Get_VecPos(), sizeof(_vec3), &dwByte, nullptr);
			iSize++;
		}
		
	}
	m_iFloor = iSize;
	iSize = 0;

	for (it = _pLayer->Get_LayerObjects()->begin(); it != _pLayer->Get_LayerObjects()->end(); it++)
	{
		if ((*it).first == L"Wall")
		{
			WriteFile(hFile, dynamic_cast<CWall*>((*it).second)->Get_VecPos(), sizeof(_vec3), &dwByte, nullptr);
			iSize++;
		}
	}
	m_iWall = iSize;
	iSize = 0;
	for (it = _pLayer->Get_LayerObjects()->begin(); it != _pLayer->Get_LayerObjects()->end(); it++)
	{
		if ((*it).first == L"WallTB")
		{
			WriteFile(hFile, dynamic_cast<CWallTB*>((*it).second)->Get_VecPos(), sizeof(_vec3), &dwByte, nullptr);
			iSize++;
		}
	}

	m_iWallTB = iSize;
	CloseHandle(hFile);

	MessageBox(g_hWnd, L"Save �Ϸ�", _T("����"), MB_OK);

}

void CMapCreate::MapLoad(CLayer* _pLayer)
{

	multimap<const _tchar*, CGameObject*>::iterator it;
	it = _pLayer->Get_LayerObjects()->find(L"Floor");
	_pLayer->Get_LayerObjects()->erase(it, _pLayer->Get_LayerObjects()->end());
	
	it = _pLayer->Get_LayerObjects()->find(L"Wall");
	_pLayer->Get_LayerObjects()->erase(it, _pLayer->Get_LayerObjects()->end());
	
	it = _pLayer->Get_LayerObjects()->find(L"WallTB");
	_pLayer->Get_LayerObjects()->erase(it, _pLayer->Get_LayerObjects()->end());



	HANDLE		hFile = CreateFile(L"../Data/Stage1.txt",	// ���� �̸����� ���Ե� ���
		GENERIC_READ,		// ���� ���� ���(GENERIC_WRITE : ����, GENERIC_READ : �б�)
		NULL,				// ���� ���(������ ���� �ִ� ���¿��� �ٸ� ���μ����� ���� �� �� �㰡 �� ���ΰ�)
		NULL,				// ���� �Ӽ�
		OPEN_EXISTING,		// ���� ���(CREATE_ALWAYS : ������ ���ٸ� ����, ������ �����, OPEN_EXISTING : ������ �ִ� ��쿡�� �ҷ�����)
		FILE_ATTRIBUTE_NORMAL, // ���� �Ӽ�(�ƹ��� �Ӽ��� ���� ���Ϸ� ����)
		NULL);				// ������ ������ �Ӽ��� ������ ���ø� ����


	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Load File", _T("Fail"), MB_OK);
		return;
	}

	DWORD	dwByte(0);

	const _tchar* pObjName{};
	const _tchar* pImageName{};
	_vec3 pMatrix{};

	int iFloor = m_iFloor;
	int iWall = m_iWall;
	int iWallTB = m_iWallTB;


	while (true)
	{
		//ReadFile(hFile, &pObjName, sizeof(const _tchar*) * 5, &dwByte, nullptr);
		//ReadFile(hFile, &pImageName, wcslen(pImageName) * 2, &dwByte, nullptr);
 		ReadFile(hFile, &pMatrix, sizeof(_vec3 ), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		if (iFloor > 0)
		{
			Engine::CGameObject* pGameObject = nullptr;

			pGameObject = CFloor::Create_Info(m_pGraphicDev, pMatrix, L"Proto_FirstFloor");
			NULL_CHECK_RETURN(pGameObject, );
			_pLayer->Add_GameObject(L"Floor", pGameObject);
			iFloor--;
		}
		else if (iWall > 0)
		{
			Engine::CGameObject* pGameObject = nullptr;

			pGameObject = CWall::Create_Pos(m_pGraphicDev, pMatrix);
			NULL_CHECK_RETURN(pGameObject, );
			_pLayer->Add_GameObject(L"Wall", pGameObject);
			iWall--;
		}
		else if (iWallTB > 0)
		{
			Engine::CGameObject* pGameObject = nullptr;

			pGameObject = CWallTB::Create_Pos(m_pGraphicDev, pMatrix);
			NULL_CHECK_RETURN(pGameObject, );
			_pLayer->Add_GameObject(L"WallTB", pGameObject);
			iWallTB--;
		}
	}

	CloseHandle(hFile);

	MessageBox(g_hWnd, L"Load �Ϸ�", _T("����"), MB_OK);
}

void CMapCreate::ShowGUI()
{
	// ����Ű�� ������ ���� �ʰ�
	ImGui::GetIO().NavActive = false;
	// ���콺�� ������ �ֵ���
	ImGui::GetIO().WantCaptureMouse = true;

	ImGui::Begin(u8"�� ������", NULL, ImGuiWindowFlags_MenuBar);//â �̸�

	//static _vec2 vecPosition = _vec2(600.0f, 100.0f);
	//ImGui::SliderFloat2(u8"��ǥ", vecPosition, -800.f, 800.f);

	SetMenu();
	//�޴���
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			//�޴�
			ImGui::MenuItem("Save");
			ImGui::Separator();


			ImGui::MenuItem("Open");

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	//��ư
	ImGui::Text(u8"��ư");
	ImGui::SameLine(50.f, 0.0f);
	//�⺻ ��ư ��													RGB ���//HSV ��ĵ� ����
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.0f, 0.0f, 0.0f));
	//���콺�� �ö��� �� ��ư ��
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0.0f, 1.0f, 0.0f));
	//Ŭ������ �� ��ư ��
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(0.0f, 0.0f, 1.0f));
	//��ư ����
	ImGui::Button(u8"����");
	ImGui::PopStyleColor(3);


	ImGui::End();

}

void CMapCreate::SetMenu()
{
	if (!ImGui::CollapsingHeader("setting"))
		return;

	//�����̴� �������� ����
	static _vec2 Position = _vec2(100.f, 100.f);
	ImGui::SliderFloat2(u8"��ǥ", Position, -800.f, 800.f);
	static int Pos[2] = { 0,0 };
	ImGui::SliderInt2(u8"��ġ", Pos, 0, 10);
}

void CMapCreate::Free()
{
	Engine::CScene::Free();
}
