#pragma once
#include <vector>
#include <fstream>
#include <iostream>

class CSettingsParser
{
    public:
    ~CSettingsParser();
    int ParseLevelsCfg(); //returns numbers of lvls
    void Init();

    enum PARSE_TYPE
    {
        eInt = 0,
        eStr,
        eDummy
    };

    private:
    std::ifstream m_cfg_game;
    int ParseLevelsNumber();
    void* ParseConfigSection(std::ifstream& config, std::string section, std::string param, PARSE_TYPE type, void* default_value = 0);
    int AssignClassForObject(std::string& str_type);
    int m_iNumLvls = 0;
};