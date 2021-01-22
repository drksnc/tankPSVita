#include "level.h"
#include "globals.h"
#include "Systems/Render/render.h"
#include "Systems/engine.h"
#include "Systems/Utils/config_parser.h"
#include "objects/actor.h"
#include "Objects/object.h"
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
}

void CLevel::InitializeObjects()
{
    for (uint8_t id = 0; id < std::numeric_limits<uint8_t>::max(); ++id)
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