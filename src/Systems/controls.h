#pragma once
#include <psp2/ctrl.h>

class CControls
{
public:
    void Init();
    virtual void UpdateInput();

    virtual void OnButtonPressed(int button);
    virtual void OnButtonReleased(int button);
    virtual void OnButtonHold(int button);

    virtual void OnLStickMove(unsigned char x, unsigned char y);
    virtual void OnRStickMove(unsigned char x, unsigned char y);

            bool IsButtonPressed(int button);

    SceCtrlData ControlData() {return m_ctrldata;};

private:
    void ReadControlsBufferData();
    void PollControls();
    void ProcControls(int mask);

    SceCtrlData m_ctrldata;
    unsigned int m_uOldButtons;
};
