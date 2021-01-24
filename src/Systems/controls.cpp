#include "controls.h"
#include "Systems/Level/level.h"
#include "Systems/engine.h"
#include "objects/actor.h"

void CControls::Init()
{
    m_uOldButtons = 0;
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
}

void CControls::UpdateInput()
{
    ReadControlsBufferData();
    PollControls();
}

void CControls::ReadControlsBufferData()
{
    sceCtrlReadBufferPositive(0, &m_ctrldata, 1);
}

void CControls::PollControls()
{
    ProcControls(SCE_CTRL_UP);
    ProcControls(SCE_CTRL_DOWN);
    ProcControls(SCE_CTRL_LEFT);
    ProcControls(SCE_CTRL_RIGHT);
    ProcControls(SCE_CTRL_CROSS);
    ProcControls(SCE_CTRL_SQUARE);
    ProcControls(SCE_CTRL_CIRCLE);
    ProcControls(SCE_CTRL_TRIANGLE);
    ProcControls(SCE_CTRL_L1);
    ProcControls(SCE_CTRL_L2);
    ProcControls(SCE_CTRL_R1);
    ProcControls(SCE_CTRL_R2);
    ProcControls(SCE_CTRL_SELECT);
    ProcControls(SCE_CTRL_START);

    OnLStickMove(m_ctrldata.lx, m_ctrldata.ly);
    OnRStickMove(m_ctrldata.rx, m_ctrldata.ry);

    m_uOldButtons = m_ctrldata.buttons;
}

void CControls::ProcControls(int mask)
{
    if (!g_Level)
      return;

    if (m_ctrldata.buttons & mask)
      m_uOldButtons & mask ?  OnButtonHold(mask) : OnButtonPressed(mask);
    else if (m_uOldButtons & mask)
      OnButtonReleased(mask);
}

void CControls::OnButtonPressed(int button)
{
    if (!Actor())
      return;

    Actor()->OnButtonPressed(button);
}

void CControls::OnButtonReleased(int button)
{
    if (!Actor())
      return;
    
    Actor()->OnButtonReleased(button);
}

void CControls::OnButtonHold(int button)
{
    if (!Actor())
      return;

    Actor()->OnButtonHold(button);
}

void CControls::OnLStickMove(unsigned char x, unsigned char y)
{

}

void CControls::OnRStickMove(unsigned char x, unsigned char y)
{

}

bool CControls::IsButtonPressed(int button)
{
  return m_ctrldata.buttons & button != 0;
}