// PluginOpen303.cpp
// toneburst (the_voder@yahoo.co.uk)

#include <iostream> // For cout

#include "SC_PlugIn.hpp"

// Include Open303 DSP header
#include "lib/Open303/Source/DSPCode/rosic_Open303.h"

// Global functions (for parameter scaling)
#include "lib/Open303/Source/DSPCode/GlobalFunctions.h"

#include "Open303.hpp"

static InterfaceTable* ft;

namespace Open303 {

    // Instantiate Open303 object
    rosic::Open303 o303;

    // CONSTRUCTOR
    Open303::Open303() {

        // Initialize the state of member variables that depend on input aruguments
        m_waveform  = in0(WAVEFORM);
        m_cutoff    = in0(CUTOFF);
        m_resonance = in0(RESONANCE);
        m_envmod    = in0(ENVMOD);
        m_decay     = in0(DECAY);
        m_accent    = in0(ACCENT);
        m_volume    = in0(VOLUME);

        // Get Sample-Rate
        m_sRate = fullSampleRate();

        // Set Open303 sample-rate
        o303.setSampleRate(m_sRate);

        mCalcFunc = make_calc_function<Open303, &Open303::next>();
        next(1);
    
    } // End Open303 constructor

    // Control-rate loop
    void Open303::next(int nSamples) {
        
        ////////////////////////
        // Control Parameters //
        ////////////////////////

        // (at Audio rate, the input values are supplied in a block of nSamples values (as floats).
        // "in0" is the first value in the block (although all other values are probably the same))

        // Most-recent note parameters. Synth expects ints
        const bool gate                      = static_cast<bool>(in0(GATE));
        const int  noteNum                   = static_cast<int>(in0(NOTENUM));
        const int  noteVel                   = static_cast<int>(in0(NOTEVEL));
        const bool noteAllOff                = static_cast<bool>(in0(NOTEALLOFF));    
        const bool accent                    = (noteVel >= accentThreshold);

        // Interpolated parameters. Synth expects doubles, inputs are floats.
        // Conversion functions from Open303 Globalfunctions.h
        // Conversion function args: <function>(in, inMin, inMax, outMin, outMax);
        // Param conversion values copied from Open303VST.cpp
        // All param inputs 0.0 - 1.0 range
        const float waveformParam            = in0(WAVEFORM);    // No scaling required (already in 0-1 range)
        const float cutoffParam              = linToExp(in0(CUTOFF),      0.0, 1.0, 200.0,  2394.0);        
        const float resonanceParam           = linToLin(in0(RESONANCE),   0.0, 1.0,   0.0,   100.0);
        const float envmodParam              = linToLin(in0(ENVMOD),      0.0, 1.0,   0.0,   100.0);
        const float decayParam               = linToExp(in0(DECAY),       0.0, 1.0, 200.0,  2000.0);
        const float accentParam              = linToLin(in0(ACCENT),      0.0, 1.0,   0.0,   100.0);
        const float volumeParam              = linToLin(in0(VOLUME),      0.0, 1.0, -60.0,    -2.0);
        const float filterMorphParam         = linToLin(in0(FILTERMORPH), 0.0, 1.0,   0.0, 0.99999);    // Must not reach exactly 1.0
        const float filterDriveParam         = linToLin(in0(FILTERDRIVE), 0.0, 1.0,   0.0,    60.0);    // Not sure of correct range here. Gain is in dB, apparently...
        
        // Create interpolation slopes
        // The slope signal is used to interpolate between the last value and the new value within the audio render loop
        SlopeSignal<float> slopedWaveform    = makeSlope(waveformParam,    m_waveform);
        SlopeSignal<float> slopedCutoff      = makeSlope(cutoffParam,      m_cutoff);
        SlopeSignal<float> slopedResonance   = makeSlope(resonanceParam,   m_resonance);
        SlopeSignal<float> slopedEnvmod      = makeSlope(envmodParam,      m_envmod);
        SlopeSignal<float> slopedDecay       = makeSlope(decayParam,       m_decay);
        SlopeSignal<float> slopedAccent      = makeSlope(accentParam,      m_accent);
        SlopeSignal<float> slopedVolume      = makeSlope(volumeParam,      m_volume);
        SlopeSignal<float> slopedFilterMorph = makeSlope(filterMorphParam, m_filtermorph);
        SlopeSignal<float> slopedFilterDrive = makeSlope(filterDriveParam, m_filterdrive);

        ///////////////////
        // Note-Handling //
        ///////////////////

        // New gate
        if(gate && !m_lastGate) {
            //cout << "PLUGIN NOTEON " << noteNum << "\n";
            o303.triggerNote(noteNum, accent);
        }
        // Gate still high but note changed. Slide to new note
        if((noteNum != m_lastNoteNum) && (gate && m_lastGate)) {
            //cout << "PLUGIN SLIDETO " << noteNum << "\n";
            o303.slideToNote(noteNum, accent);
        }
        // Last note off
        if(m_lastGate && !gate) {
            //cout << "PLUGIN LAST NOTE OFF " << noteNum << "\n";
            //o303.releaseNote(noteNum);
            o303.allNotesOff();
        }
        // Detect all-notes-off trigger
        if(noteAllOff && !m_lastNoteAllOff) {
            // Trigger synth all-notes-off
            //cout << "PLUGIN ALL NOTES OFF\n"
            o303.allNotesOff();
        }

        //////////////////
        // Audio Render //
        //////////////////

        // Create output buffer
        float* outbuf = out(0);

        // Fill output buffer with nSamples Open303 synth samples
        for (int i = 0; i < nSamples; ++i) {
            
            // Update synth params with interpolated value
            // Cast floats to doubles
            o303.setWaveform(    static_cast<double>(slopedWaveform.consume()));
            o303.setCutoff(      static_cast<double>(slopedCutoff.consume()));
            o303.setResonance(   static_cast<double>(slopedResonance.consume()));
            o303.setEnvMod(      static_cast<double>(slopedEnvmod.consume()));
            o303.setDecay(       static_cast<double>(slopedDecay.consume()));
            o303.setAccent(      static_cast<double>(slopedAccent.consume()));
            o303.setVolume(      static_cast<double>(slopedVolume.consume()));
            o303.setFilterMorph( static_cast<double>(slopedFilterMorph.consume()));
            o303.setFilterDrive( static_cast<double>(slopedFilterDrive.consume()));

            // Call Open303 render function
            outbuf[i] = o303.getSample();
        }

        ////////////////////////
        // Update Param State //
        ////////////////////////

        m_waveform      = slopedWaveform.value;
        m_cutoff        = slopedCutoff.value;
        m_resonance     = slopedResonance.value;
        m_envmod        = slopedEnvmod.value;
        m_decay         = slopedDecay.value;
        m_accent        = slopedAccent.value;
        m_volume        = slopedVolume.value;
        m_filtermorph   = slopedFilterMorph.value;
        m_filterdrive   = slopedFilterDrive.value;

        ///////////////////////////////
        // Update Previous Gate/Note //
        ///////////////////////////////

        m_lastGate        = gate;
        m_lastNoteNum     = noteNum;
        m_lastNoteAllOff  = noteAllOff;
    
    } // End Open303::next()

} // End of namespace Open303

PluginLoad(Open303UGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<Open303::Open303>(ft, "Open303", false);
}
