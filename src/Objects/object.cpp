#include "object.h"
#include "sdl2/sdl.h"
CObject::CObject()
{
    m_sName = "undefined";
    m_texture = NULL;
    m_iHealth = 0;
    m_direction_mask = eDirDummy;
}

CObject::~CObject()
{

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