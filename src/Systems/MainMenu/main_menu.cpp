#include "main_menu.h"
#include "globals.h"
#include "Systems/Level/level.h"
#include "Systems/Network/network.h"
#include "objects/actor.h"

CMainMenu::CMainMenu()
{
    m_elements.clear();
}

CMainMenu::~CMainMenu()
{
    for (auto &element : m_elements)
        delete element;
}

void CMainMenu::Init()
{
    AddElement("New Game");
    AddElement("Connect");
    AddElement("Quit");
}

void CMainMenu::AddElement(std::string element)
{
    MElement *pElement = new MElement();

    pElement->ID = m_elements.size();
    pElement->Text = element;

    m_elements.push_back(pElement);
}

void CMainMenu::OnButtonPressed(int button)
{
    if (g_Level)
        return;

    switch (button)
    {
    case SCE_CTRL_UP: 
    {
        if (m_elements.size() - m_current_element == m_elements.size())
        {
            m_current_element = m_elements.size()-1;
            break;
        }
        m_current_element--; break;
    }
    case SCE_CTRL_DOWN:
    {
        if (m_elements.size()-1 - m_current_element == 0)
        {
            m_current_element = 0;
            break;
        } 
        m_current_element++; break;
    }
    case SCE_CTRL_CROSS: ChooseOption(); break;
    default:
        break;
    }
}

void CMainMenu::Update()
{
    if (g_Level)
    {
        if (!Actor())
            LoadLevel(0);

        return;
    }
    
    int offset = 0;
    for (auto &element : m_elements)
    {
        SDL_Color color;
        if (m_current_element == element->ID)
            color = {255, 0, 0, 255};
        else
            color = {0, 0, 0, 255};

        g_Render->SetText(element->Text, Fvector().set(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + offset), &color);
        offset += 25;
    }
}

void CMainMenu::ChooseOption()
{
    switch (m_current_element)
    {
    case eoptStart: if (g_Network) g_Network->CreateServer(); LoadLevel(0); break;
    case eoptConnect: g_Network->Connect("192.168.0.77"); break;
    case eoptQuit: g_bExit = true; break;
    default:
        break;
    }
}