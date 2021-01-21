#pragma once
#include <string>

class SDL_Texture;
class Fvector;

class IPureObject
{
    public:
    virtual ~IPureObject() {};

    virtual void Update() = 0;
    virtual std::string Name() = 0;
    virtual uint8_t ID() = 0;
    virtual bool is_Alive() = 0;

    virtual SDL_Texture* Texture() = 0;
    virtual bool NeedToRender() = 0;
    virtual Fvector& Position() = 0;
    virtual uint8_t Direction() = 0;

    virtual void OnSpawn() = 0;
};