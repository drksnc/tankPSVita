#pragma once
#include <string>
#include "object_collider.h"
#include "../Systems/Utils/vector.h"
#include "pure_object.h"
#include "systems/Render/render.h"

struct RawObject;

class CObject
{
    public:
    friend class CLevel;
    CObject();
    virtual ~CObject();

    enum eDirection
    {
        eDirUp = 0x00000001,
        eDirDown = 0x00000002,
        eDirLeft = 0x00000004,
        eDirRight = 0x00000008,
        eDirDummy = 0x00000100
    };

    virtual void OnSpawn(RawObject* raw_object);
    virtual std::string Name();
    virtual uint8_t ID();
    virtual bool is_Alive();

    virtual void Update();
    virtual SDL_Texture* Texture();
            void SetTexture(SDL_Texture* texture) {m_texture = texture;};

    virtual bool NeedToRender();

    virtual Fvector& Position();
    virtual uint8_t Direction();

    protected:
    void SetDirection(eDirection dir);

    private:
    uint8_t m_iHealth;
    uint8_t m_ID;
    std::string m_sName;
    SDL_Texture* m_texture;
    Fvector m_position;
    uint8_t m_direction_mask;
    

    friend class CLevel;

};