#pragma once
#include "Systems/Render/render.h"
#include "systems/engine.h"
#include "systems/controls.h"
#include <string>
#include <vector>

struct MElement;

class CMainMenu
{
    public:
    CMainMenu();
    ~CMainMenu();

    void Init();
    void Update();

    void OnButtonPressed(int button);

    private:
    enum EMenuOptions
    {
        eoptStart = 0,
        eoptConnect,
        eoptQuit
    };

    struct MElement
    {
        int ID;
        std::string Text;
        SDL_Texture* Texture;
    };

    void AddElement(std::string text);
    void ChooseOption();

    std::vector<MElement*> m_elements;
    int m_current_element = 0;
    int m_cooldown_after_death = 100;
};