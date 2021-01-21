#pragma once
#include "Objects/object.h"
#include "Systems/Utils/vector.h"
#include <limits>

class CSettingsParser;

class CLevel
{
    public:
    CLevel();
    ~CLevel();

    void Init();
    CObject* getObject(uint8_t obj_id) {return m_objects_pool[obj_id];};
    CObject* CurrentControlEntity();
    void Update();

    private:
    void InitializeObjects();

    CObject* CreateObject(int type);
    CSettingsParser* m_cfg_parser = NULL;

    CObject* m_objects_pool[std::numeric_limits<uint8_t>::max()]; //reserve objects pool (npc, doors, chests, etc)
    CObject* m_current_control_entity = NULL;
    void SetCurrentControlEntity(CObject* obj);
};