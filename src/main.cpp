#include "Systems/controls.h"
#include "Systems/engine.h"
#include "Systems/Level/level.h"
#include "Systems/Render/render.h"
#include "Systems/MainMenu/main_menu.h"
#include "Systems/Network/network.h"
#include "psp2/net/net.h"
#include "psp2/net/netctl.h"
#include "psp2/kernel/threadmgr.h"
#include "globals.h"

CControls* g_Control = NULL;
CLevel* g_Level = NULL;
CRender* g_Render = NULL;
CMainMenu* g_MainMenu = NULL;
CNetwork* g_Network = NULL;
bool g_bExit = false;
uint32_t g_CurrentFrame = 0;

uint32_t CurrentFrame() {return g_CurrentFrame;};

std::vector<RawLevel> g_RawLevels;

static int Update1(SceSize args, void *arg)
{
    while (!g_bExit)
    {
        //SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_INFO, "2");
        //g_Control->UpdateInput();
        //g_MainMenu->Update();
       // if (g_Level) g_Level->Update();
       // g_Control->UpdateInput();
    }

    return 1;
}

void LoadLevel(int lvl_id)
{
    if (g_Level) delete g_Level;
    g_Level = new CLevel();
    g_Level->Init(lvl_id);
}

int main(int argc, char *argv[]) 
{
    g_RawLevels.clear();

    g_Control = new CControls();
    g_MainMenu = new CMainMenu();
    g_Render  = new CRender();
  //  g_Network = new CNetwork();

    g_Control->Init();

    if (g_Render->Init() == -1) 
        return -1;

    g_MainMenu->Init();
   // g_Network->Init();
    
    while (!g_bExit)
    {
        g_Control->UpdateInput();
        g_MainMenu->Update();
        if (g_Level) g_Level->Update();
        g_Render->Render();
        
        //g_Network->Listen();
        //Snd
        //Network       
    }

    delete g_Render;
    delete g_Level;
    delete g_Control;
    delete g_Network;
	sceKernelExitProcess(0);
    return 0;
}
