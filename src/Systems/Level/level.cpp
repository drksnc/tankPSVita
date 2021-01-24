#include "level.h"
#include "globals.h"
#include "Systems/Render/render.h"
#include "Systems/engine.h"
#include "Systems/Utils/config_parser.h"
#include "objects/actor.h"
#include "Objects/object.h"
#include "Objects/bullet.h"
#include <sdl2/SDL.h>
#include <cstdio>

CObject* CLevel::CreateObject(int type)
{
    switch (type)
    {
    case eActor: { SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Created CActor"); return new CActor(); } break;
    case eEnemy: return new CObject(); break;
    default: return new CObject(); break;
    }
}

CLevel::CLevel()
{
    
}

CLevel::~CLevel()
{
    g_Render->DestroyTexture(m_BGTexture);
    m_current_control_entity = NULL;
    delete m_cfg_parser;
}

void CLevel::Init()
{
    if (!m_cfg_parser)
        m_cfg_parser = new CSettingsParser();

    m_cfg_parser->Init();
    m_cfg_parser->ParseLevelsCfg();
    InitializeObjects();
    InitializeBG();
}

void CLevel::InitializeObjects()
{
    for (uint8_t id = 0; id < MaxObjects(); ++id)
    {
        if (id < g_RawLevels[0].raw_objects.size())
        {
            auto raw_obj = &g_RawLevels[0].raw_objects[id];
            m_objects_pool[id] = CreateObject(raw_obj->eClass);
            CObject* object = m_objects_pool[id];
            object->m_ID = id;
            object->OnSpawn(raw_obj);
        }
    }
}

void CLevel::InitializeBG()
{
    char bg_texture_buf[BUF_SIZE]; sprintf(bg_texture_buf, "%s%s%s", BG_DIR, g_RawLevels[0].background_texture.c_str(), PNG_EXT);
    m_BGTexture = g_Render->LoadTexture(bg_texture_buf); 
}

void CLevel::Update()
{
    for (auto it : m_objects_pool)
        if (it) it->Update();
}

void CLevel::SetCurrentControlEntity(CObject* obj)
{
    if (m_current_control_entity) return;
    m_current_control_entity = obj;
}

CObject* CLevel::CurrentControlEntity()
{
    return m_current_control_entity;
}

void CLevel::CreateBullet(CObject* owner)
{
    if (!owner || !owner->is_Alive())
    {
        SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_WARN, "[%u] Can't create Bullet - owner is dead", CurrentFrame());
        return;        
    }

    if (m_uObjects_count >= MaxObjects())
    {
        SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_WARN, "[%u] Can't create Bullet - no free memory", CurrentFrame());
        return;
    }

    CBullet* bullet = new CBullet(owner);
    m_objects_pool.push_back(bullet);
    bullet->m_ID = m_uObjects_count++;

    RawObject raw_bullet;
    raw_bullet.iHealth = 1;
    raw_bullet.eClass = eShoot;
    raw_bullet.sName = "bullet";
    raw_bullet.sSprite = BULLET_SPRITE_ALIAS;

    uint8_t owner_direction = owner->Direction();
    Fvector start_pos;

    if (owner_direction & CObject::eDirRight)
        start_pos.set(owner->Position().x + owner->Rect().w, owner->Position().y + owner->Rect().h / 2);
    else if (owner_direction & CObject::eDirLeft)
        start_pos.set(owner->Position().x, owner->Position().y + owner->Rect().h / 2);
    else if (owner_direction & CObject::eDirUp)
        start_pos.set(owner->Position().x + owner->Rect().w / 2, owner->Position().y);
    else if (owner_direction & CObject::eDirDown)
        start_pos.set(owner->Position().x + owner->Rect().w / 2, owner->Position().y + owner->Rect().h);   

    raw_bullet.startPosition = start_pos;

    bullet->OnSpawn(&raw_bullet);
}
}