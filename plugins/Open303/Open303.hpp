// PluginOpen303.hpp
// toneburst (the_voder@yahoo.co.uk)
/*
    SuperCollider plugin wrapper around Open303 TB303 emulator DSP code by Robin
   Schmidt https://github.com/RobinSchmidt/Open303
*/

#pragma once

#include "SC_PlugIn.hpp"

namespace Open303 {

class Open303 : public SCUnit {
public:
  /////////////////
  // Constructor //
  /////////////////

  Open303();

private:

  /////////////////////
  // State Variables //
  /////////////////////

  double m_sRate{};
  bool   m_lastGate{false};
  bool   m_lastNoteAllOff{0};
  int    m_lastNoteNum{60};
  
  const int accentThreshold{100};

  // Param state (initialised in constructor)
  float m_waveform;
  float m_cutoff;
  float m_resonance;
  float m_envmod;
  float m_decay;
  float m_accent;
  float m_volume;
  float m_filtermorph;
  float m_distortion;

  // Input inded enumeration
  enum inputs {
    GATE = 0,
    NOTENUM,
    NOTEVEL,
    NOTEALLOFF,
    WAVEFORM,
    CUTOFF,
    RESONANCE,
    ENVMOD,
    DECAY,
    ACCENT,
    VOLUME,
    FILTERMORPH,
    DISTORTION
  };

  // Calc function
  void next(int nSamples);

  //////////////////////
  // Member Variables //
  //////////////////////
};

} // End namespace Open303
