#include "spawner.h"
#include "systems/Level/level.h"

CSpawner::CSpawner()
{
    m_cooldown = 200 + std::rand() % 351;
    m_last_time_spawn = m_cooldown;
}

void CSpawner::Update()
{
    if (CurrentFrame() - m_last_time_spawn > m_cooldown)
    {
        g_Level->CreateEnemy(Position());
        m_last_time_spawn = CurrentFrame();
    }
}