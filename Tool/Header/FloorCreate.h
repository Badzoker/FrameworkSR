#pragma once

#include "Tool_Include.h"

class CToolView;
class CFloorCreate
{
public:
	CFloorCreate();
	~CFloorCreate();

public:
	void		Set_MainView(CToolView* pMainView) { m_MainView = pMainView; }

public:
	HRESULT		Initialize();
	void		Update();
	void		Render();
	void		Release();

private:
	vector<TILE*>		m_vecTile;
	CToolView*			m_MainView;

};

// ������ �������� �̿��Ͽ� Ÿ�� �浹�� �ض�

// y = ax + b 
// 
// a : ����(y ������ / x ������)
// b : y���� (x���� 0�� ���� y ��)