#pragma once
#include "Objects/object.h"
#include "Systems/Utils/vector.h"
#include <limits>

class CSettingsParser;
class SDL_Texture;

class CLevel
{
    public:
    CLevel();
    ~CLevel();

    void Init();
    CObject* getObject(uint8_t obj_id) {return m_objects_pool[obj_id];};
    uint8_t MaxObjects() {return std::numeric_limits<uint8_t>::max();};
    void SetCurrentControlEntity(CObject* obj);
    CObject* CurrentControlEntity();
    void Update();

    SDL_Texture* BackgroundTexture() {return m_BGTexture;};

    private:
    void InitializeObjects();
    void InitializeBG();

    CObject* CreateObject(int type);
    CSettingsParser* m_cfg_parser = NULL;

    CObject* m_objects_pool[std::numeric_limits<uint8_t>::max()]; //reserve objects pool (npc, doors, chests, etc)
    CObject* m_current_control_entity = NULL;
    SDL_Texture* m_BGTexture;
};