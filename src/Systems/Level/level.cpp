#include "level.h"
#include "globals.h"
#include "Systems/Render/render.h"
#include "Systems/engine.h"
#include "Systems/Utils/config_parser.h"
#include "objects/actor.h"
#include "Objects/object.h"
#include "Objects/bullet.h"
#include "Objects/enemy.h"
#include <sdl2/SDL.h>
#include <cstdio>
#include <memory>

CObject* CLevel::CreateObject(int type)
{
    switch (type)
    {
    case eActor: { SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Created CActor"); return new CActor(); } break;
    case eEnemy: return new CEnemy(); break;
    default: return new CObject(); break;
    }
}

CLevel::CLevel()
{
    m_objects_pool.clear();
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

    m_objects_pool.resize(MaxObjects(), NULL);

    m_cfg_parser->Init();
    m_cfg_parser->ParseLevelsCfg();
    InitializeObjects();
    InitializeBG();
}

void CLevel::InitializeObjects()
{
    uint8_t id = 0;
    for (; id < g_RawLevels[0].raw_objects.size(); ++id)
    {
        auto raw_obj = &g_RawLevels[0].raw_objects[id];
        CObject* object = CreateObject(raw_obj->eClass);
        object->OnSpawn(raw_obj);
        AddObjectToPool(object);
#if DEBUG
        SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_INFO, "[%u] Spawn object with CLASS [%u]; ID [%u]; HP [%u]", CurrentFrame(), raw_obj->eClass, object->m_ID, object->Health());
#endif
    }
}

void CLevel::InitializeBG()
{
    char bg_texture_buf[BUF_SIZE]; sprintf(bg_texture_buf, "%s%s%s", BG_DIR, g_RawLevels[0].background_texture.c_str(), PNG_EXT);
    m_BGTexture = g_Render->LoadTexture(bg_texture_buf); 
}

void CLevel::Update()
{
    FreeObjectPool();

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

CBullet* CLevel::CreateBullet(CObject* pOwner)
{
    if (!pOwner || !pOwner->is_Alive())
    {
        SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_WARN, "[%u] Can't create Bullet - owner is dead", CurrentFrame());
        return NULL;        
    }

    if (m_objects_pool.size() >= MaxObjects())
    {
        SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_WARN, "[%u] Can't create Bullet - no free memory", CurrentFrame());
        return NULL;
    }

    CBullet *pBullet = new CBullet(pOwner);
    RawObject raw_bullet;
    raw_bullet.iHealth = 1;
    raw_bullet.eClass = eShoot;
    char bullet_name[BUF_SIZE]; sprintf(bullet_name, "bullet%i", pBullet->m_ID);
    raw_bullet.sName = bullet_name;
    raw_bullet.sSprite = BULLET_SPRITE_ALIAS;

    uint8_t owner_direction = pOwner->Direction();
    Fvector start_pos;

    if (owner_direction & CObject::eDirRight)
        start_pos.set(pOwner->Position().x + pOwner->Rect().w, pOwner->Position().y + pOwner->Rect().h / 2);
    else if (owner_direction & CObject::eDirLeft)
        start_pos.set(pOwner->Position().x, pOwner->Position().y + pOwner->Rect().h / 2);
    else if (owner_direction & CObject::eDirUp)
        start_pos.set(pOwner->Position().x + pOwner->Rect().w / 2, pOwner->Position().y);
    else if (owner_direction & CObject::eDirDown)
        start_pos.set(pOwner->Position().x + pOwner->Rect().w / 2, pOwner->Position().y + pOwner->Rect().h);   

    raw_bullet.startPosition = start_pos;
    pBullet->OnSpawn(&raw_bullet);
    AddObjectToPool(pBullet);

    return pBullet;
}

void CLevel::FreeObjectPool()
{
    for (auto I = m_objects_pool.begin(); 
              I < m_objects_pool.end();)
    {
        CObject* pObject = *I;

        if (pObject && pObject->NeedToDestroy())
        {
            m_objects_pool[pObject->ID()] = NULL;
            delete pObject;
        }
        else ++I;
    }
}

void CLevel::AddObjectToPool(CObject *pObj)
{
    for (int i = 0; i < m_objects_pool.size(); ++i)
    {
        if (!m_objects_pool[i])
        {
            pObj->m_ID = i;
            m_objects_pool[i] = pObj;
            break;
        }
    }
}
}