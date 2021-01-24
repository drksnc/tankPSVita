#include "Systems/controls.h"
#include "Systems/engine.h"
#include "Systems/Level/level.h"
#include "Systems/Render/render.h"
#include "globals.h"
#include "time.h"

CControls* g_Control;
CLevel* g_Level;
CRender* g_Render;
bool g_bExit = false;
uint32_t g_CurrentFrame = 0;

uint32_t CurrentFrame() {return g_CurrentFrame;};

std::vector<RawLevel> g_RawLevels;

int main(int argc, char *argv[]) 
{
    g_RawLevels.clear();

    g_Control = new CControls();
    g_Level   = new CLevel();
    g_Render  = new CRender();

    g_Control->Init();

    if (g_Render->Init() == -1) 
        return -1;

    g_Level->Init();

    while (!g_bExit)
    {
        g_Control->UpdateInput();
        g_Level->Update();
        g_Render->Render();
        //Snd
        //Network       
    }

    delete g_Render;
    delete g_Level;
    delete g_Control;
	sceKernelExitProcess(0);
    return 0;
}
