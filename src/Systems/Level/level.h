#pragma once
#include "Objects/object.h"
#include "Systems/Utils/vector.h"
#include <limits>

class CSettingsParser;
class SDL_Texture;
class CBullet;
class CActor;
class CEnemy;

typedef std::vector<CObject*> object_list;

struct AINode
{
    int ID;
    Fvector position;
    bool occupied = false;
    int cost = 0;
    std::vector<int> neighbors_id;
    CObject *object_inside = NULL;
};

class CLevel
{
    public:
    CLevel();
    ~CLevel();

    void Init(int lvl_id);
    CObject* getObject(uint8_t obj_id) {return m_objects_pool[obj_id];};
    uint8_t MaxObjects() {return std::numeric_limits<uint8_t>::max();};
    object_list Objects() {return m_objects_pool;};
    void SetCurrentControlEntity(CObject *obj);
    CObject* CurrentControlEntity();
    void Update();

    std::vector<AINode*> AINodes() {return m_ai_nodes;};
    int MaxAINodes() {return m_MaxNodes;};
    std::vector<CActor*> Actors() {return m_actors;};
    SDL_Texture* BackgroundTexture() {return m_BGTexture;};
    CBullet* CreateBullet(CObject* owner);
    CEnemy* CreateEnemy(Fvector &pos);

    int m_nodesXcount = 0;
    int m_nodesYcount = 0;

    private:
    void InitializeObjects();
    void InitializeBG();
    void GenerateAINodes();
    void AddNeighbors(AINode *pNode);
    void CreateWalls();

    CObject* CreateObject(int type);
    void FreeObjectPool(bool bNeedCheckForDestroy = true);
    void AddObjectToPool(CObject *object);
    CSettingsParser *m_cfg_parser = NULL;

    object_list m_objects_pool;
    std::vector<AINode*> m_ai_nodes;
    CObject *m_current_control_entity = NULL;
    SDL_Texture *m_BGTexture;
    uint8_t m_object_pool_size;

    int m_current_level_id = 0;
    int m_MaxNodes = 0;

    std::vector<CActor*> m_actors;
};