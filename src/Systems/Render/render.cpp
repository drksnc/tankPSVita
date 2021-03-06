#include "render.h"
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include "globals.h"
#include "systems/Level/level.h"
#include "Systems/Utils/vector.h"
#include "Systems/engine.h"
#include "Objects/object.h"

using namespace std;

CRender::CRender()
{
    m_texture_pool.clear();
}

CRender::~CRender()
{
    for (auto &it : m_texture_pool)
    {
        DestroyTexture(it.second);
    }

    TTF_CloseFont(m_font);
    TTF_Quit();
    IMG_Quit();

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

    if (TTF_Init() < 0)
        return -1;

    m_font = TTF_OpenFont(GAME_FONT_PATH, GAME_FONT_SIZE);

    if (!m_font)
        return -1;

    return 0;
}

void CRender::Render()
{
    SDL_SetRenderDrawColor(g_Render->m_Renderer, 255, 255, 255, 255);
	SDL_RenderClear(g_Render->m_Renderer);

    if (g_Level)
    {
        g_Render->RenderBackground();

        for (auto &pObject : g_Level->Objects())
        {
            if (!pObject) continue;
            if (pObject->NeedToRender())
                g_Render->QueryTexture(pObject->Texture(), pObject->Rect(), pObject->Position(), pObject->Direction());
#if DEBUG
//            g_Render->RenderDebugInfo(pObject);
#endif
        }
    }

    SDL_RenderPresent(g_Render->m_Renderer);
    SDL_Delay(16);

    g_CurrentFrame++;
}

SDL_Texture* CRender::LoadTexture(std::string filename)
{
	SDL_Texture *pTexture = NULL;

    auto it = m_texture_pool.find(filename);
    if (it != m_texture_pool.end())
    {
        pTexture = (*it).second;
    }
    else
    {
#if DEBUG
	    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading texture %s", filename.c_str());
#endif
	    pTexture = IMG_LoadTexture(m_Renderer, filename.c_str());
        m_texture_pool.insert(std::make_pair(filename, pTexture));
    }

	return pTexture;
}

void CRender::DestroyTexture(SDL_Texture* texture)
{
    SDL_DestroyTexture(texture);
}

void CRender::QueryTexture(SDL_Texture* texture, SDL_Rect& rect, Fvector& pos, uint8_t direction)
{
	rect.x = pos.x;
	rect.y = pos.y;
    double angle = GetSpriteAngleByDirection(direction);
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopyEx(m_Renderer, texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
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

void CRender::SetText(std::string text, Fvector pos, SDL_Color* pColor)
{
    SDL_Color color;
    pColor ? color = *pColor : color = { 0, 0, 255 };
    SDL_Surface* text_surface = TTF_RenderText_Solid(m_font, text.c_str(), color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(m_Renderer, text_surface);
    SDL_Rect dest = { pos.x, pos.y, text_surface->w, text_surface->h };
    SDL_RenderCopy(m_Renderer, text_texture, NULL, &dest);

#if !EMULATOR
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
#endif
}

void CRender::RenderBackground()
{
    if (!g_Level)
        return;

    if (!g_Level->BackgroundTexture())
        return;

    SDL_QueryTexture(g_Level->BackgroundTexture(), NULL, NULL, NULL, NULL);
    SDL_RenderCopy(m_Renderer, g_Level->BackgroundTexture(), NULL, NULL);
}

void CRender::RenderRect(CObject *obj)
{
    SDL_SetRenderDrawColor(g_Render->Renderer(), 0, 0, 255, SDL_ALPHA_OPAQUE);

    SDL_RenderDrawLine(g_Render->Renderer(), obj->Rect().x, obj->Rect().y, obj->Rect().x + obj->Rect().w, obj->Rect().y);
    SDL_RenderDrawLine(g_Render->Renderer(), obj->Rect().x, obj->Rect().y, obj->Rect().x, obj->Rect().y + obj->Rect().h);
    SDL_RenderDrawLine(g_Render->Renderer(), obj->Rect().x + obj->Rect().w, obj->Rect().y + obj->Rect().h, (obj->Rect().x + obj->Rect().w) - obj->Rect().w, obj->Rect().y + obj->Rect().h);
    SDL_RenderDrawLine(g_Render->Renderer(), obj->Rect().x + obj->Rect().w, obj->Rect().y + obj->Rect().h, obj->Rect().x + obj->Rect().w, (obj->Rect().y + obj->Rect().h) - obj->Rect().h);
}

void CRender::RenderDebugInfo(CObject *obj)
{
    // char hp[BUF_SIZE]; sprintf(hp, "Health %i", obj->Health());
    // char pos[BUF_SIZE]; sprintf(pos, "Pos Y %i X %i", obj->Position().x, obj->Position().y);
    // char frame[BUF_SIZE]; sprintf(frame, "Frame %u", CurrentFrame());
    // char id[BUF_SIZE]; sprintf(id, "ID %i", obj->ID());
    //SetText(hp, Fvector().set(obj->Position().x, obj->Position().y - 15));
    //SetText(pos, Fvector().set(obj->Position().x, obj->Position().y - 30));
    //SetText(frame, Fvector().set(obj->Position().x, obj->Position().y - 45));
    //SetText(frame, Fvector().set(obj->Position().x, obj->Position().y - 45));
    RenderRect(obj);
  
    return;
    
    int offsetx = AI_NODE_WIDTH;
    int offsety = AI_NODE_HEIGHT;

    if (g_Level->AINodes().empty())
        return;

    SDL_SetRenderDrawColor(g_Render->Renderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);

    for (auto &it : g_Level->AINodes())
    {
        if (it->occupied)
        {
            SDL_RenderDrawLine(g_Render->Renderer(), it->position.x, it->position.y, it->position.x+offsetx, it->position.y);
            SDL_RenderDrawLine(g_Render->Renderer(), it->position.x, it->position.y, it->position.x, it->position.y+offsety);
            SDL_RenderDrawLine(g_Render->Renderer(), it->position.x+offsetx, it->position.y+offsety, (it->position.x+offsetx) - offsetx, it->position.y+offsety);
            SDL_RenderDrawLine(g_Render->Renderer(), it->position.x+offsetx, it->position.y+offsety, it->position.x+offsetx, (it->position.y+offsety) - offsety);  
        }
        char buf[BUF_SIZE]; sprintf(buf, "%i", it->ID);; 
        SetText(buf, it->position);      
    }

}