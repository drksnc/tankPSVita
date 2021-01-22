#include "object.h"
#include "sdl2/sdl.h"
#include "systems/engine.h"
#include "actor.h"
#include "globals.h"

CObject::CObject()
{
    m_sName = "undefined";
    m_texture = NULL;
    m_iHealth = 0;
    m_direction_mask = eDirDummy;
    //m_object_collider = NULL;
}

CObject::~CObject()
{
    delete m_object_collider;
}

void CObject::OnSpawn(RawObject* raw_object)
{
    m_sName = raw_object->sName;
    m_iHealth = raw_object->iHealth;
    m_iColliderOff = raw_object->iCollideOff;
    m_position.set(raw_object->startPosition.x, raw_object->startPosition.y);
    char texture_buf[BUF_SIZE]; sprintf(texture_buf, "%s%s%s", SPRITES_DIR, raw_object->sSprite.c_str(), PNG_EXT);
    SetTexture(g_Render->LoadTexture(texture_buf));

    m_object_collider = new CObjectCollider(this);
}

void CObject::OnSpawn()
{

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

uint8_t CObject::Direction()
{
    return m_direction_mask;
}

void CObject::SetDirection(eDirection dir)
{
    m_direction_mask = eDirDummy;
    m_direction_mask |= dir;
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
   
}