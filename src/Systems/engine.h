#pragma once
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <vector>
#include <string>
#include "Systems/Utils/vector.h"

class CControls;
class CLevel;
class SDL_Window;
class SDL_Renderer;
class CRender;

static const int g_iSpeed = 1;

enum eObjectType
{
    eActor = 0,
    eEnemy,
    eBarrel,
    eShoot,
    ePowerup,
    eSpawn,
    eBase,
    eUnknownObj
};

struct RawObject
{
    std::string sName;
    eObjectType eClass;
    std::string sSprite;
    int iHealth;
    int iCollideOff;
    Fvector startPosition;
};

struct RawLevel
{
    int iLvlId = 0;
    std::string background_texture;
    std::vector<RawObject> raw_objects;
};

uint32_t CurrentFrame();

extern CControls* g_Control;
extern CLevel* g_Level;

extern CRender* g_Render;
extern std::vector<RawLevel> g_RawLevels;
extern uint32_t g_CurrentFrame;
extern bool    g_bExit;