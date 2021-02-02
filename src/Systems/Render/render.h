#pragma once
#include <psp2/kernel/processmgr.h>
#include <map>
#include <vector>
#include <string>
#include "SDL2/SDL_ttf.h"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class SDL_Rect;
class Fvector;
class CObject;

class CRender
{
    public:
    CRender();
    ~CRender();
    int Init();

    SDL_Window* Window() {return m_Window;};
    SDL_Renderer* Renderer() {return m_Renderer;};

    SDL_Texture* LoadTexture(std::string filename);
    void DestroyTexture(SDL_Texture* texture);

    static void Render();

    void RenderRect(CObject* obj);
    void RenderDebugInfo(CObject* obj);

    void SetText(std::string text, Fvector pos, SDL_Color* color = NULL);

    private:
    void RenderBackground();
    void QueryTexture(SDL_Texture* texture, SDL_Rect& rect, Fvector& pos, uint8_t direction);
    double GetSpriteAngleByDirection(uint8_t direction);

    std::map<std::string, SDL_Texture*> m_texture_pool;

    TTF_Font* m_font = NULL;
    SDL_Window* m_Window = NULL;
    SDL_Renderer* m_Renderer = NULL;
};