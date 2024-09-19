#include "pch.h"
#include "FloorCreate.h"
#include "Device.h"
#include "TextureMgr.h"
#include "ToolView.h"

CFloorCreate::CFloorCreate()
{
	m_vecTile.reserve(TILEX * TILEY);
}


CFloorCreate::~CFloorCreate()
{
	Release();
}

HRESULT CFloorCreate::Initialize()
{

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(L"../Client/Bin/Resource/Texture/MMJ_Floor/Floor%d.png", TEX_MULTI, L"Floor", L"FloorTile", 10)))
	{
		AfxMessageBox(L"Tile Texture Failed");
		return E_FAIL;
	}

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			TILE* pTile = new TILE;

			// x��ǥ�� ¦�� �� ��� ���� �������� Ÿ���� �߽����� ����
			// y��ǥ�� Ȧ�� �� ��� ������ 
			float	fX = (TILECX * j) + (i % 2) * (TILECX / 2.f);
			float	fY = (TILECY / 2.f) * i;

			pTile->vPos = { fX, fY, 0.f };
			pTile->vSize = { (float)TILECX, (float)TILECY };
			pTile->byOption = 0; // ���ϸ� �� ���ڸ� �ɼ� ��ȣ�� �����ϵ��� ���� ����
			pTile->byDrawID = 1;

			m_vecTile.push_back(pTile);
		}
	}

	return S_OK;
}

void CFloorCreate::Update()
{
}

void CFloorCreate::Render()
{
	D3DXMATRIX		matWorld, matScale, matTrans;

	TCHAR	szBuf[MIN_STR] = L"";
	int		iIndex(0);

	for (auto pTile : m_vecTile)
	{
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans,
			pTile->vPos.x - m_MainView->GetScrollPos(0),
			pTile->vPos.y - m_MainView->GetScrollPos(1), 0.f);

		matWorld = matScale * matTrans;

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Floor", L"FloorTile", pTile->byDrawID);

		float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,
			nullptr, // ����� �̹��� ������ RECT �ּ�, NULL�� ��� 0, 0 �������� ���
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // ����� �̹����� �߽� ��ǥ, VEC3�ּ�, NULL �� ��� 0, 0�� �߽� ��ǥ
			nullptr, // �ؽ�ó�� ����� ��ġ ��ǥ, VEC3�ּ�, ��ũ���� 0, 0 ��ǥ�� ���
			D3DCOLOR_ARGB(255, 255, 255, 255)); // ����� ���� �̹����� ���� ����, 0xffffffff�� �Ѱ��ָ� ������ ����


		////////////////Ÿ�� �ε��� ���//////////////////

		//swprintf_s(szBuf, L"%d", iIndex);

		////D3DXMATRIX	matTrans2;
		////D3DXMatrixTranslation(&matTrans2, 100.f, 100.f, 0.f);
		////CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matTrans2);

		//CDevice::Get_Instance()->Get_Font()->DrawTextW(CDevice::Get_Instance()->Get_Sprite(), szBuf, lstrlen(szBuf), nullptr, 0, D3DCOLOR_ARGB(255, 255, 255, 255));
		//++iIndex;
	}

}

void CFloorCreate::Release()
{
	
	/*for_each(m_vecTile.begin(), m_vecTile.end(), Safe_Delete<TILE*>);
	m_vecTile.clear();*/
}
