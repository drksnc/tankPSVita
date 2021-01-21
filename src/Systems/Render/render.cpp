#include "render.h"
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include "SDL2/SDL_ttf.h"
#include "globals.h"
#include "systems/Level/level.h"
#include "Systems/Utils/vector.h"
#include "Systems/engine.h"

using namespace std;

CRender::~CRender()
{
    //Destroy all object's texture first
    for (uint8_t it = 0; it < std::numeric_limits<uint8_t>::max(); ++it)
    {
        CObject* object = g_Level->getObject(it);
        if (object) DestroyTexture(object->Texture());
    }

    SDL_DestroyRenderer(Renderer());
    SDL_DestroyWindow(Window());

    m_Renderer = NULL;
    m_Window = NULL;

    SDL_Quit();
}

int CRender::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return -1;

    if ((m_Window = SDL_CreateWindow("TankVita", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL)
        return -1;
    
    if ((m_Renderer = SDL_CreateRenderer(m_Window, -1, 0)) == NULL)
        return -1;

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
        return -1;

    return 0;
}

void CRender::Render()
{
    SDL_SetRenderDrawColor(m_Renderer, 107, 219, 197, 255);
	SDL_RenderClear(m_Renderer);

    for (uint8_t it = 0; it < std::numeric_limits<uint8_t>::max(); ++it)
    {
        CObject* object = g_Level->getObject(it);

        if (!object)
            continue;
            
        if (object->NeedToRender())
            QueryTexture(object->Texture(), object->Position().x, object->Position().y, object->Direction());
    }

    SDL_RenderPresent(m_Renderer);

    SDL_Delay(16);
}

SDL_Texture* CRender::LoadTexture(char* filename)
{
	SDL_Texture *texture;

#if DEBUG
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
#endif
	texture = IMG_LoadTexture(m_Renderer, filename);
    
	return texture;
}

void CRender::DestroyTexture(SDL_Texture* texture)
{
    SDL_DestroyTexture(texture);
}

void CRender::QueryTexture(SDL_Texture* texture, int x, int y, uint8_t direction)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
    double angle = GetSpriteAngleByDirection(direction);

	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopyEx(m_Renderer, texture, NULL, &dest, angle, NULL, SDL_FLIP_NONE);
}

double CRender::GetSpriteAngleByDirection(uint8_t direction)
{
    if (direction & CObject::eDirRight)
        return 90;
    else if (direction & CObject::eDirLeft)
        return -90;
    else if (direction & CObject::eDirUp)
        return 0;
    else if (direction & CObject::eDirDown)
        return 180;

    return 0;
}