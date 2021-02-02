#pragma once
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <vector>
#include <string>
#include "Systems/Utils/vector.h"
#include "Objects/object_collider.h"

class CControls;
class CLevel;
class SDL_Window;
class SDL_Renderer;
class CRender;
class CObjectCollider;
class CMainMenu;
class CNetwork;

typedef CObjectCollider::CollisionSide collision_side;
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
    eWall,
    eUnknownObj
};

struct RawObject
{
    std::string sName;
    eObjectType eClass;
    std::string sSprite;
    int iHealth;
    bool bNeedUpdateNodes = false;
    bool bBreakable = true;
    Fvector startPosition;
};

struct RawLevel
{
    int iLvlId = 0;
    std::string background_texture;
    std::vector<RawObject> raw_objects;
};

uint32_t CurrentFrame();

extern CControls *g_Control;
extern CLevel *g_Level;
extern CMainMenu *g_MainMenu;
extern CNetwork *g_Network;
extern CRender *g_Render;

extern std::vector<RawLevel> g_RawLevels;
extern uint32_t g_CurrentFrame;
extern bool    g_bExit;

extern void LoadLevel(int lvl_id);