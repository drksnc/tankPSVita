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
    virtual bool Breakable() {return m_bBreakable;};

    virtual void OnCollide(CObject* who_collide, CObjectCollider::CollisionSide collision_side);
    virtual void OnHit(CObject* who_hit, int damage);
    virtual void AfterCollide();
    virtual bool IsColliding() {return m_bIsColliding;};

    virtual void Update();
    virtual SDL_Texture* Texture();
    virtual SDL_Rect& Rect();
            void SetTexture(SDL_Texture* texture) {m_texture = texture;};

    virtual bool NeedToRender();
    virtual bool NeedToDestroy() {return m_bNeedToDestroy;};

    virtual Fvector& Position();
    virtual Fvector& PositionCenter();
    virtual uint8_t Direction();
            int Type() {return m_ObjectType;};
            bool Static() {return m_bNeedUpdateAINodes;};

    protected:
    CObjectCollider* m_object_collider;
    uint8_t m_iHealth;
    bool m_bIsColliding = false;    
    void SetDirection(eDirection dir);
    static bool NodeFree(int NodeID);
    static int GetNodeByPosition(Fvector& pos);

    uint8_t m_ID;
    std::string m_sName;
    SDL_Texture* m_texture;
    SDL_Rect m_Rect;
    Fvector m_position;
    uint8_t m_direction_mask;
    uint32_t m_uTimeBeforeDestroy;
    uint32_t m_uDeathTime;
    bool m_bBreakable = true;
    bool m_bNeedToDestroy = false;
    bool m_bNeedUpdateAINodes = false;
    std::vector<int> m_nodes;

    int m_ObjectType;

    private:
    void UpdateAINodes();
    void SetNodeOccupied(int nodeID, bool occupied);
    void DisableNodes();

};