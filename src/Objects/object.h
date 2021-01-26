#pragma once
#include <string>
#include "object_collider.h"
#include "../Systems/Utils/vector.h"
#include "pure_object.h"
#include "systems/Render/render.h"
#include "systems/engine.h"

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
    virtual uint8_t Health() {return m_iHealth;};
    virtual bool is_Alive();
    virtual void Die();

    virtual void OnCollide(CObject* who_collide, CObjectCollider::CollisionSide collision_side);
    virtual void OnHit(CObject* who_hit, int damage);
    virtual void AfterCollide();
    virtual bool IsColliding() {return m_bIsColliding;};
    virtual uint8_t ColliderOffset() {return m_iColliderOff;};

    virtual void Update();
    virtual SDL_Texture* Texture();
    virtual SDL_Rect& Rect();
            void SetTexture(SDL_Texture* texture) {m_texture = texture;};

    virtual bool NeedToRender();
    virtual bool NeedToDestroy() {return m_bNeedToDestroy;};

    virtual Fvector& Position();
    virtual uint8_t Direction();


    protected:
    CObjectCollider* m_object_collider;
    uint8_t m_iHealth;
    uint8_t m_iColliderOff = 0;
    bool m_bIsColliding = false;    
    void SetDirection(eDirection dir);

    uint8_t m_ID;
    std::string m_sName;
    SDL_Texture* m_texture;
    SDL_Rect m_Rect;
    Fvector m_position;
    uint8_t m_direction_mask;
    uint32_t m_uTimeBeforeDestroy;
    uint32_t m_uDeathTime;
    bool m_bNeedToDestroy = false;

};