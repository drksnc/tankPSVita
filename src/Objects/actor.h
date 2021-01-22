#pragma once
#include "object.h"

struct RawObject;

class CActor : virtual public CObject
{
    typedef CObject inherited;

    public:
    CActor();
    virtual ~CActor();
    
    virtual void OnSpawn(RawObject* raw_object);
    virtual void Update();
    virtual bool NeedToRender();

    void OnButtonPressed(int button);
    void OnButtonHold(int button);
    void OnButtonReleased(int button);

    int Velocity() {return m_iVelocity;};

    private:
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

    protected:
    int m_iVelocity = 3;
    void SetVelocity(int value) {m_iVelocity = value;};
};

CActor* Actor();