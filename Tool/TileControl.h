#pragma once
#include "FloorInfo.h"
#include "WallInfo.h"

class CTileControl
{

	// �׽�Ʈ�� �ִ� Ŭ����...�̻��ϰ� �¾ƿ�...
public:
	CTileControl();
	~CTileControl();

public:
	void Initialize();
	void Update();
	void LateUpdate();
	void Render();
	void Release();

//
//private:
//	CFloorInfo Add_Tile();
//
//private:
//	vector<CFloorInfo*> m_vecFloorList;
//	//vector<CWallInfo*> m_vecWallList;


};
