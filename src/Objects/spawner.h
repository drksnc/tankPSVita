#pragma once
#include "object.h"

class CSpawner : public CObject
{
    public:
    CSpawner();
    virtual ~CSpawner() {};

    virtual void Update();

    private:

    uint32_t m_cooldown;
    uint32_t m_last_time_spawn;

    int m_MaxObjects;
};