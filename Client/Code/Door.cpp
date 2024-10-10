#include "pch.h"
#include "../Header/Door.h"
#include "../Header/Player.h"
#include "../Header/UIScreen.h"
#include "Export_System.h"
#include "Export_Utility.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CTileContorl(_pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pColliderCom(nullptr)
    , m_vecWallDirection({ 0.f, 0.f, 0.f })
    , m_fMovingSpeed(0.f)
    , m_bIsOpen(false)
{
    m_iNumber = 0;
    m_iNumber_Type = 4;
}

CDoor::~CDoor()
{
}

CDoor* CDoor::Create_InfoNumberDirectionTrigger(LPDIRECT3DDEVICE9 _pGraphicDev, _vec3 _vecPos, const _int& _iNumber, Engine::TILE_DIRECTION _eTileDirection, const _int& _iTrigger)
{
    CDoor* pDoor = new CDoor(_pGraphicDev);


    if (FAILED(pDoor->Ready_GameObject()))
    {
        Safe_Release(pDoor);
        MSG_BOX("Door Create Failed");
        return nullptr;
    }

    if (_iNumber == 9)
    {
        pDoor->m_bLastDoor = true;
    }
    pDoor->Setup_Position(_vecPos);
    pDoor->m_vecPos = _vecPos;
    pDoor->Set_Number(_iNumber);
    pDoor->m_iNumber = _iNumber;
    pDoor->Set_TileDirection(_eTileDirection);
    pDoor->Set_Trigger(_iTrigger);

    return pDoor;
}

CDoor* CDoor::Create_InfoNumberDirectionTrigger2(LPDIRECT3DDEVICE9 _pGraphicDev, _vec3 _vecPos, _vec3 _vecRot, const _int& _iNumber, const _int& _iTrigger)
{
    CDoor* pDoor = new CDoor(_pGraphicDev);


    if (FAILED(pDoor->Ready_GameObject()))
    {
        Safe_Release(pDoor);
        MSG_BOX("Door Create Failed");
        return nullptr;
    }

    if (_iNumber == 9)
    {
        pDoor->m_bLastDoor = true;
    }
    pDoor->Setup_Position(_vecPos);
    pDoor->m_vecPos = _vecPos;
    pDoor->Set_Number(_iNumber);
    pDoor->m_iNumber = _iNumber;
    pDoor->Set_TileDirection(_vecRot);
    pDoor->Set_Trigger(_iTrigger);

    return pDoor;
}

void CDoor::Set_TileDirection(const _vec3& _vecDir)
{
    m_vecWallDirection = _vecDir;

    m_pColliderCom->SetShow(true);
}

HRESULT CDoor::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pColliderCom->SetTransform(m_pTransformCom);
    m_pColliderCom->SetRadius(1.5f);
    m_pColliderCom->SetShow(true);
    m_pColliderCom->SetActive(true);

    return S_OK;
}

_int CDoor::Update_GameObject(const _float& _fTimeDelta)
{
    if (!m_bIsRender)
        return 0;

    if (m_bIsOpen)
    {
        if (m_fMovingSpeed > 0.5f)
        {
            m_bIsOpen = false;
        }
        else
        {
            _vec3 vPos, vLook, vUp, vDir;
            m_pTransformCom->Get_Info(INFO::INFO_POS, &vPos);
            m_pTransformCom->Rotation(ROTATION::ROT_Y, D3DXToRadian(-5.5f));
            m_fMovingSpeed += _fTimeDelta;
        }


        //vPos.x -= _fTimeDelta * 2.f;
        //m_pTransformCom->Set_Pos(vPos);
    }

    Add_RenderGroup(RENDERID::RENDER_NONALPHA, this);

    Engine::Add_Collider(m_pColliderCom);

    return Engine::CGameObject::Update_GameObject(_fTimeDelta);
}

void CDoor::LateUpdate_GameObject()
{
    if (!m_bIsRender)
        return;

    Engine::CGameObject::LateUpdate_GameObject();
}

void CDoor::Render_GameObject()
{
    if (!m_bIsRender)
        return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(m_iNumber - 1);

    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDoor::Set_TileDirection(Engine::TILE_DIRECTION _eTileDirection)
{
    switch (_eTileDirection)
    {
    case Engine::TILE_DIRECTION::TILE_FORWARD:
        m_vecWallDirection = { 0.f, 0.f, 1.f };
        break;
    case Engine::TILE_DIRECTION::TILE_RIGHT:
        m_vecWallDirection = { 1.f, 0.f, 0.f };
        break;
    case Engine::TILE_DIRECTION::TILE_LEFT:
        m_vecWallDirection = { -1.f, 0.f, 0.f };
        break;
    case Engine::TILE_DIRECTION::TILE_BACK:
        m_vecWallDirection = { 0.f, 0.f, -1.f };
        break;
    }
}

void CDoor::Set_IsRender(const _bool& _bool)
{
    m_bIsRender = _bool;
    m_pColliderCom->SetActive(_bool);
    m_pColliderCom->SetShow(_bool);
}

HRESULT CDoor::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CWallTBTex*>(Engine::Clone_Proto(L"Proto_WallTBTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[(_uint)COMPONENTID::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_Door"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[(_uint)COMPONENTID::ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[(_uint)COMPONENTID::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_Collider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[(_uint)COMPONENTID::ID_DYNAMIC].insert({ L"Com_Collider", pComponent });
    pComponent->SetOwner(*this);

    return S_OK;
}

HRESULT CDoor::Setup_Material()
{
    D3DMATERIAL9 tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Ambient = { 1.f, 1.f, 1.f, 1.f };

    tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f };
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}

void CDoor::Setup_Position(_vec3 _vecPos)
{
    m_pTransformCom->Set_Pos(_vecPos.x, _vecPos.y, _vecPos.z);
}

void CDoor::Setup_Angle(_vec3 _vecRot)
{
    m_pTransformCom->Set_Angle(_vecRot.x, _vecRot.y, _vecRot.z);
}

void CDoor::Moving_Open()
{
    m_bIsOpen = true;
}

void CDoor::OnCollisionEnter(CCollider& _pOther)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pOther.GetOwner());
    if (pPlayer != nullptr)
    {
        Engine::Collision_With_Trigger(m_iTriggerNumber);
        Engine::Calculate_Trigger();
        Moving_Open();

        if (m_bLastDoor)
        {
            if (Engine::Get_ListUI(UITYPE::UI_SCREEN)->empty())
            {
                Engine::Activate_UI(UITYPE::UI_SCREEN);
                static_cast<CUIScreen*>(Engine::Get_ListUI(UITYPE::UI_SCREEN)->front())->Set_FloorTime(Engine::Get_Elapsed());
            }
        }
    }
}

void CDoor::OnCollisionExit(CCollider& _pOther)
{
}

void CDoor::Free()
{
    CTileContorl::Free();
}
