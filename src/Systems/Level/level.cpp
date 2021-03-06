#include "level.h"
#include "globals.h"
#include "Systems/Render/render.h"
#include "Systems/engine.h"
#include "Systems/Utils/config_parser.h"
#include "objects/actor.h"
#include "Objects/object.h"
#include "Objects/bullet.h"
#include "Objects/enemy.h"
#include "Objects/spawner.h"
#include <sdl2/SDL.h>
#include <cstdio>
#include <memory>
#include <algorithm>

CObject* CLevel::CreateObject(int type)
{
    switch (type)
    {
    case eActor: {
            CActor* pActor = new CActor(); 
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Created CActor"); 
            m_actors.push_back(pActor);
            return pActor; 
        } break;
    case eEnemy: return new CEnemy(); break;
    case eSpawn: return new CSpawner(); break;
    default: return new CObject(); break;
    }
}

CLevel::CLevel()
{
    m_object_pool_size = 0;
    m_objects_pool.clear();
    m_ai_nodes.clear();
    m_actors.clear();
}

CLevel::~CLevel()
{
    FreeObjectPool(false);

#if !EMULATOR
    g_Render->DestroyTexture(m_BGTexture);
#endif

    for (auto &node : AINodes()) 
        delete node;

    AINodes().clear();

    m_current_control_entity = NULL;
    delete m_cfg_parser;
    m_cfg_parser = NULL;
}

void CLevel::Init(int lvl_id)
{
    m_current_level_id = lvl_id;

    if (!m_cfg_parser)
        m_cfg_parser = new CSettingsParser();

    m_objects_pool.resize(MaxObjects(), NULL);
    m_cfg_parser->Init();
    m_cfg_parser->ParseLevelsCfg();
    GenerateAINodes();
    InitializeObjects();
    InitializeBG();

    CreateWalls();
}

void CLevel::InitializeObjects()
{
    uint8_t id = 0;
    for (; id < g_RawLevels[m_current_level_id].raw_objects.size(); ++id)
    {
        auto raw_obj = &g_RawLevels[m_current_level_id].raw_objects[id];
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
    char bg_texture_buf[BUF_SIZE]; sprintf(bg_texture_buf, "%s%s%s", BG_DIR, g_RawLevels[m_current_level_id].background_texture.c_str(), PNG_EXT);
    m_BGTexture = g_Render->LoadTexture(bg_texture_buf); 
}

CEnemy* CLevel::CreateEnemy(Fvector &pos)
{
    RawObject raw_obj;
    raw_obj.bNeedUpdateNodes = true;
    raw_obj.eClass = eEnemy;
    raw_obj.iHealth = 25;
    raw_obj.sName = "enemy";
    raw_obj.sSprite = "tank0";
    raw_obj.startPosition = pos;
    
    CObject *pObj = CreateObject(raw_obj.eClass);
    pObj->OnSpawn(&raw_obj);
    AddObjectToPool(pObj);

    return dynamic_cast<CEnemy*>(pObj);
}

void CLevel::Update()
{
    FreeObjectPool();

    for (auto &it : m_objects_pool)
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

    if (m_object_pool_size >= MaxObjects())
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

void CLevel::FreeObjectPool(bool bNeedCheckForDestroy)
{
    for (auto I = m_objects_pool.begin(); 
              I < m_objects_pool.end(); ++I)
    {
        CObject* pObject = *I;

        if (pObject)
        {
            if (bNeedCheckForDestroy && !pObject->NeedToDestroy())
                continue;
            
            if (pObject->Type() == eObjectType::eActor)
                m_actors.erase(std::find(m_actors.begin(), m_actors.end(), dynamic_cast<CActor*>(pObject)));

            m_objects_pool[pObject->ID()] = NULL;
            delete pObject;
            m_object_pool_size--;
        }
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
            m_object_pool_size++;
            break;
        }
    }
}

void CLevel::GenerateAINodes()
{
    int nodeX = 0;
    int nodeY = 0;
    int nodeWidth = AI_NODE_WIDTH;
    int nodeHeight = AI_NODE_HEIGHT;

    //Have to do this because of rounding
    float screenWidth = SCREEN_WIDTH;
    float screenHeight = SCREEN_HEIGHT;

    int nodeCount = 0;

    m_nodesXcount = screenWidth / nodeWidth + 0.5f;
    m_nodesYcount = screenHeight / nodeHeight + 0.5f;

    m_MaxNodes = m_nodesXcount * m_nodesYcount - 1;

    for (int j = 0; j < m_nodesYcount; ++j)
    {  
        nodeX = 0;
        nodeWidth = 0; 

        for (int i = 0; i < m_nodesXcount; ++i)
        {
            AINode* node = new AINode();
            node->ID = nodeCount;
            node->position.set(nodeX, nodeY);

            AddNeighbors(node);

            m_ai_nodes.push_back(node);
            nodeWidth += AI_NODE_WIDTH;
            nodeX = nodeWidth; 
            nodeCount++;          
        }

        nodeY = nodeHeight;
        nodeHeight += AI_NODE_HEIGHT;   
    }    
}

void CLevel::AddNeighbors(AINode *pNode)
{
    pNode->neighbors_id.clear();

    int TopNeighbor = pNode->ID - m_nodesXcount;
    int BottomNeighbor = pNode->ID + m_nodesXcount;
    int LeftNeighbor = pNode->ID - 1;
    int RightNeighbor = pNode->ID + 1;

    if (RightNeighbor % m_nodesXcount != 0 && RightNeighbor < m_MaxNodes) pNode->neighbors_id.push_back(RightNeighbor);
    if (LeftNeighbor % m_nodesXcount != 0 && LeftNeighbor > 0) pNode->neighbors_id.push_back(LeftNeighbor);
    if (TopNeighbor > 0) pNode->neighbors_id.push_back(TopNeighbor);
    if (BottomNeighbor < m_MaxNodes) pNode->neighbors_id.push_back(BottomNeighbor);
}

void CLevel::CreateWalls()
{
    RawObject raw_wall;
    raw_wall.eClass = eWall;
    raw_wall.iHealth = 90;
    raw_wall.sName = "wall_name";
    raw_wall.sSprite = "wall";
    raw_wall.startPosition = (AINodes()[90]->position);
    raw_wall.bBreakable = false;

    auto pObj = CreateObject(eWall);
    pObj->OnSpawn(&raw_wall);
    AddObjectToPool(pObj);        
}