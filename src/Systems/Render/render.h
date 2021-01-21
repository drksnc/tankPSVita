#pragma once
#include <psp2/kernel/processmgr.h>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class SDL_Rect;
class Fvector;

class CRender
{
    public:
    ~CRender();
    int Init();

    SDL_Window* Window() {return m_Window;};
    SDL_Renderer* Renderer() {return m_Renderer;};

    SDL_Texture* LoadTexture(char* filename);
    void DestroyTexture(SDL_Texture* texture);

    void Render();

    private:
    void QueryTexture(SDL_Texture* texture, int x, int y, uint8_t direction);
    double GetSpriteAngleByDirection(uint8_t direction);

    SDL_Window* m_Window   = NULL;
    SDL_Renderer* m_Renderer = NULL;
};