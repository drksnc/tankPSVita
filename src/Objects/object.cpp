#include "object.h"
#include "sdl2/sdl.h"
#include "bullet.h"
#include "actor.h"
#include "globals.h"
#include "enemy.h"

CObject::CObject()
{
    m_sName = "undefined";
    m_texture = NULL;
    m_iHealth = 0;
    m_direction_mask = 0;
    m_uTimeBeforeDestroy = 0;
    m_nodes.clear();

    m_Rect.h = 0;
    m_Rect.w = 0;
    m_Rect.x = 0;
    m_Rect.y = 0;
}

CObject::~CObject()
{
    DisableNodes();
    if (m_object_collider) delete m_object_collider;
}

void CObject::OnSpawn(RawObject* raw_object)
{
    m_sName = raw_object->sName;
    m_iHealth = raw_object->iHealth;
    m_bNeedUpdateAINodes = raw_object->bNeedUpdateNodes;
    m_bBreakable = raw_object->bBreakable;
    m_ObjectType = raw_object->eClass;
    m_position.set(raw_object->startPosition.x, raw_object->startPosition.y);
    char texture_buf[BUF_SIZE]; sprintf(texture_buf, "%s%s%s", SPRITES_DIR, raw_object->sSprite.c_str(), PNG_EXT);
    SetTexture(g_Render->LoadTexture(texture_buf));

    m_object_collider = new CObjectCollider(this);
    m_uTimeBeforeDestroy = 100;

    SetDirection(eDirUp);
    UpdateAINodes();
}

void CObject::OnCollide(CObject* who_collide, CObjectCollider::CollisionSide collision_side)
{
    if (!who_collide)
        return;

    if (who_collide->Type() == eObjectType::eShoot) // we handle this in OnHit method
        return;

    m_bIsColliding = true;
}

void CObject::AfterCollide()
{
    m_bIsColliding = false;
}

void CObject::OnHit(CObject* who_hit, int dmg)
{
    if (m_iHealth <= dmg)
        Die();
    else
        m_iHealth -= dmg;
}

std::string CObject::Name()
{
    return m_sName;
}

uint8_t CObject::ID()
{
    return m_ID;
}

SDL_Texture* CObject::Texture() 
{
    return m_texture;
}

SDL_Rect& CObject::Rect()
{
    return m_Rect;
}

Fvector& CObject::Position()
{
    return m_position;
}

Fvector& CObject::PositionCenter()
{
    return Fvector().set(Position().x + Rect().w / 2, Position().y + Rect().h / 2);
}

uint8_t CObject::Direction()
{
    return m_direction_mask;
}

void CObject::SetDirection(eDirection dir)
{
    m_direction_mask = 0;
    m_direction_mask |= dir;
}

void CObject::Die()
{
    m_iHealth = 0; 
    m_uDeathTime = CurrentFrame();
}

bool CObject::is_Alive()
{
    return m_iHealth > 0;
}

bool CObject::NeedToRender()
{
    if (!is_Alive())
        return false;

    return true;
}

void CObject::Update()
{
    if (!is_Alive())
    {
        if (CurrentFrame() - m_uDeathTime < m_uTimeBeforeDestroy)
            m_bNeedToDestroy = true; 

        return;
    }

    if (m_bNeedUpdateAINodes) UpdateAINodes();
    m_object_collider->Update();
}

void CObject::UpdateAINodes()
{
    DisableNodes();

    int nodesX = g_Level->m_nodesXcount;

    int LeftBottom = GetNodeByPosition(Fvector().set(Position().x, Position().y + Rect().h));
    int LeftTop = GetNodeByPosition(Position());
    int RightBottom = GetNodeByPosition(Fvector().set(Position().x + Rect().w, Position().y + Rect().h));
    int RightTop = GetNodeByPosition(Fvector().set(Position().x + Rect().w, Position().y));

    SetNodeOccupied(LeftBottom, true);
    SetNodeOccupied(LeftTop, true);
    SetNodeOccupied(RightBottom, true);
    SetNodeOccupied(RightTop, true);
}

int CObject::GetNodeByPosition(Fvector& pos)
{
    int posX = pos.x;
    int posY = pos.y;

    int nodesX = g_Level->m_nodesXcount;
    int nodeID = (posX / AI_NODE_WIDTH) + nodesX * (posY / AI_NODE_HEIGHT);

    return nodeID;
}

void CObject::SetNodeOccupied(int nodeID, bool bOccupied)
{
    if (nodeID > g_Level->MaxAINodes())
        return;

    if (nodeID < 0)
        return;

    auto node = g_Level->AINodes()[nodeID];

    node->occupied = bOccupied;
    node->object_inside = bOccupied ? this : NULL;
    if (bOccupied) m_nodes.push_back(nodeID);
}

void CObject::DisableNodes()
{
    for (auto &it : m_nodes)
        SetNodeOccupied(it, false);
    
    m_nodes.clear();
}

bool CObject::NodeFree(int NodeID)
{
    if (NodeID > g_Level->MaxAINodes())
        return false;

    if (NodeID < 0)
        return false;

    if (g_Level->AINodes()[NodeID]->occupied)
        return false;

    return true;
}