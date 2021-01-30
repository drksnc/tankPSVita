#include "config_parser.h"
#include "globals.h"
#include <sdl2/SDL.h>
#include <string>
#include <limits>
#include <cstdlib>
#include <map>
#include <psp2/kernel/processmgr.h>
#include "Systems/Utils/vector.h"
#include "Systems/engine.h"
#include <algorithm>

using namespace std;

#define SETTINGS_SECT "settings"

CSettingsParser::~CSettingsParser()
{
    m_cfg_game.close();
}

void CSettingsParser::Init()
{
    m_cfg_game.open(GAME_CONFIG_PATH);
}

int CSettingsParser::ParseLevelsCfg()
{
    int num_lvls = ParseLevelsNumber();
    char buf[BUF_SIZE];
    int i, j = 0;

    g_RawLevels.resize(num_lvls);

    for (i = 0; i < num_lvls; ++i)
    {
        snprintf(buf, BUF_SIZE, "%s%s%d%s", LEVELS_DIR, "level", i, CFG_EXT);
        ifstream cfg_lvl(buf);
        
        int num_obj = (int)ParseConfigSection(cfg_lvl, SETTINGS_SECT, "objects", eInt);
        char default_bg[4] = "bg0";
        string bg_texture = (char*)ParseConfigSection(cfg_lvl, SETTINGS_SECT, "background", eStr, default_bg);
        g_RawLevels[i].raw_objects.resize(num_obj);

        g_RawLevels[i].iLvlId = i;
        g_RawLevels[i].background_texture = bg_texture;

        for (j = 0; j < num_obj; j++)
        {
            snprintf(buf, BUF_SIZE, "%s%d", "object", j);
            
            string sClass  = (char*)ParseConfigSection(cfg_lvl, buf, "class", eStr);
            string sSprite = (char*)ParseConfigSection(cfg_lvl, buf, "sprite", eStr);
            string sPos    = (char*)ParseConfigSection(cfg_lvl, buf, "start_pos", eStr);
            int hp         = (int)ParseConfigSection(cfg_lvl, buf, "hp", eInt);
            int posX, posY; 
            sscanf(sPos.c_str(), "%d, %d", &posX, &posY);

            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Parsed object with CLASS [%s]; SPRITE [%s]; POS [%d %d]; HP [%d] for LEVEL [%d]", 
                sClass.c_str(), sSprite.c_str(), posX, posY, hp, i);

            RawObject* raw_obj = &g_RawLevels[i].raw_objects[j];
            snprintf(buf, BUF_SIZE, "%s%d", sClass.c_str(), j);
            raw_obj->sName = buf; //enemy0, actor1, etc
            raw_obj->eClass = (eObjectType)AssignClassForObject(sClass);
            raw_obj->bNeedUpdateNodes = (raw_obj->eClass == eActor || raw_obj->eClass == eEnemy);
            raw_obj->sSprite = sSprite;
            raw_obj->iHealth = hp;
            raw_obj->startPosition.set(posX, posY);
        }

        cfg_lvl.close();

        //we have to reverse vector because we need to render Actor above all objects
        reverse(g_RawLevels[i].raw_objects.begin(), g_RawLevels[i].raw_objects.end());
    }

    return num_lvls;
}

int CSettingsParser::ParseLevelsNumber()
{
    int num = (int)ParseConfigSection(m_cfg_game, "game", "levels", eInt);
    return num;
}

void* CSettingsParser::ParseConfigSection(ifstream& config, string section, string param, PARSE_TYPE parse_type, void* default_value)
{
    string cfg_str; 
    bool sect_found = false, param_found = false;

    while (getline(config, cfg_str))
    {      
        if (sect_found)
        {
            if (!param_found)
            {
                if (cfg_str.find(param.c_str()) != string::npos)
                    param_found = true;
                else
                    continue; 
            }

            //parse all parametres here
            size_t lexem_pos = cfg_str.find('=');
            if (lexem_pos != string::npos)
            {
                if (strstr(cfg_str.substr(0, lexem_pos-1).c_str(), param.c_str()))
                {
                    config.clear();
                    config.seekg(0, ios::beg);
                    string cfg_var = cfg_str.substr(lexem_pos+2, cfg_str.length()); // = and space

                    switch (parse_type)
                    {
                    case eInt: return (void*)stoi(cfg_var.erase(cfg_var.length()-1)); break;
                    case eStr: return (void*)cfg_var.erase(cfg_var.length()-1).c_str(); break;
                    }
                } 
            }
        } 

        if (cfg_str.find(section.c_str()) != string::npos)
            sect_found = true;
    }
    config.clear();
    config.seekg(0, ios::beg);

    return default_value;
}

int CSettingsParser::AssignClassForObject(string& str_type)
{
    if (strstr(str_type.c_str(), "actor"))
        return eActor;
    else if (strstr(str_type.c_str(), "enemy"))
        return eEnemy;
    else if(strstr(str_type.c_str(), "barrel"))
        return eBarrel;
    else if (strstr(str_type.c_str(), "powerup"))
        return ePowerup;
    else if (strstr(str_type.c_str(), "spawn"))
        return eSpawn;
    else if (strstr(str_type.c_str(), "base"))
        return eBase;
    else 
        return eUnknownObj;
}