// PluginOpen303.cpp
// toneburst (the_voder@yahoo.co.uk)

#include <iostream>

#include "SC_PlugIn.hpp"

// Include Open303 DSP header
#include "lib/Open303/Source/DSPCode/rosic_Open303.h"

// Global functions (for parameter scaling)
#include "lib/Open303/Source/DSPCode/GlobalFunctions.h"

// Shruthi-1 note-stack (thanks as ever @Pichenettes)
#include "lib/pichenettes/shruthi_noteStack.hpp"

#include "Open303.hpp"

static InterfaceTable* ft;

namespace Open303 {

    // Instantiate Open303 object
    rosic::Open303 o303;

    // Instantiate Shruthi-1 note-stack
    shruthi::NoteStack noteStack;

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
        srate = fullSampleRate();

        // Set Open303 sample-rate
        o303.setSampleRate(srate);

        // Init note-stack
        noteStack.Init();

        mCalcFunc = make_calc_function<Open303, &Open303::next>();
        next(1);
    
    } // End Open303 constructor

    // Control-rate loop
    void Open303::next(int nSamples) {
        // Control parameters
        // (at Audio rate, the input values are supplied in a block of nSamples values (as floats).
        // "in0" is the first value in the block (although all other values are probably the same))

        // Note parameters. Synth expects ints
        const int noteevent    = static_cast<int>(in0(NOTEEVENT));
        const int notenum      = static_cast<int>(in0(NOTENUM));
        const int notevel      = static_cast<int>(in0(NOTEVEL));
        const int notealloff   = static_cast<int>(in0(NOTEALLOFF));
        const bool accent      = (notevel >= 100) ? true : false;

        // Interpolated synth control parameters. Synth expects doubles, inputs are floats.
        // Conversion functions from Globalfunctions.h
        // Conversion function args: <function>(in, inMin, inMax, outMin, outMax);
        // Param conversion values copied from Open303VST.cpp
        // All params 0.0 - 1.0 range
        const float waveformParam   = in0(WAVEFORM);    // No scaling required (already in 0-1 range)
        const float cutoffParam     = linToExp(in0(CUTOFF),    0.0, 1.0, 314.0, 2394.0);        
        const float resonanceParam  = linToLin(in0(RESONANCE), 0.0, 1.0,   0.0,  100.0);
        const float envmodParam     = linToLin(in0(ENVMOD),    0.0, 1.0,   0.0,  100.0);
        const float decayParam      = linToExp(in0(DECAY),     0.0, 1.0, 200.0, 2000.0);
        const float accentParam     = linToLin(in0(ACCENT),    0.0, 1.0,   0.0,  100.0);
        const float volumeParam     = linToLin(in0(VOLUME),    0.0, 1.0, -60.0,    0.0);
        
        // Create interpolation slopes
        SlopeSignal<float> slopedWaveform   = makeSlope(waveformParam,  m_waveform);
        SlopeSignal<float> slopedCutoff     = makeSlope(cutoffParam,    m_cutoff);
        SlopeSignal<float> slopedResonance  = makeSlope(resonanceParam, m_resonance);
        SlopeSignal<float> slopedEnvmod     = makeSlope(envmodParam,    m_envmod);
        SlopeSignal<float> slopedDecay      = makeSlope(decayParam,     m_decay);
        SlopeSignal<float> slopedAccent     = makeSlope(accentParam,    m_accent);
        SlopeSignal<float> slopedVolume     = makeSlope(volumeParam,    m_volume);

        ///////////////////
        // Note-Handling //
        ///////////////////

        // Process note on/off messages
        // Use Shruthi-1 note-stack
        //if(noteevent == 1 && lastnoteevent == 0) {
        if(noteevent == 1) {
            // Check note velocity to determine if this is a note-on or note-off event
            if(notevel > 0) {
                // NOTE-ON             
                // Check note-stack size. If 0, play note
                if(noteStack.size() == 0) {
                    // No note in stack, we can play the incoming note
                    o303.triggerNote(notenum, accent);
                    cout << "PLUGIN TRIGGER NOTE " << notenum << "\n";
                } else {
                    // There's at least one note in the stack (a note is already playing). We need to slide to the incoming note
                    o303.slideToNote(notenum, accent);
                    cout << "PLUGIN SLIDETO NOTE " << notenum << "\n";
                }
                // add note to note-stack
                noteStack.NoteOn(notenum, notevel);      
            } else {
                // NOTE-OFF (note-on event with velocity 0)
                // Remove note from stack
                noteStack.NoteOff(notenum);
                // Check new note-stack size. If 0, release note
                if(noteStack.size() == 0) {
                    // No notes currently playing, we can release the current note
                    o303.releaseNote(notenum);
                    cout << "PLUGIN RELEASE NOTE " << notenum << "\n";
                } else {
                    // Slide to the most recent note remaining in the stack
                    o303.slideToNote(noteStack.most_recent_note().note, false);
                    cout << "PLUGIN SLIDETO NOTE " << notenum << "\n";
                }
            }
        }

        // Update previous note-event
        lastnoteevent = noteevent;

        // Detect all-notes-off trigger
        if(notealloff == 1 && lastnotealloff == 0) {
            o303.allNotesOff();
        }
        // Update previous all-note-off
        lastnotealloff = notealloff;

        //////////////////
        // Audio Render //
        //////////////////

        // Create output buffer
        float* outbuf = out(0);

        // Fill output buffer with nSamples Open303 synth samples
        for (int i = 0; i < nSamples; ++i) {
            // Update synth params with interpolated value
            // Cast floats to doubles
            o303.setWaveform(   static_cast<double>(slopedWaveform.consume()));
            o303.setCutoff(     static_cast<double>(slopedCutoff.consume()));
            o303.setResonance(  static_cast<double>(slopedResonance.consume()));
            o303.setEnvMod(     static_cast<double>(slopedEnvmod.consume()));
            o303.setDecay(      static_cast<double>(slopedDecay.consume()));
            o303.setAccent(     static_cast<double>(slopedAccent.consume()));
            o303.setVolume(     static_cast<double>(slopedVolume.consume()));

            // Call Open303 render function
            outbuf[i] = o303.getSample();
        }

        ////////////////////////
        // Update Param State //
        ////////////////////////

        m_waveform  = slopedWaveform.value;
        m_cutoff    = slopedCutoff.value;
        m_resonance = slopedResonance.value;
        m_envmod    = slopedEnvmod.value;
        m_decay     = slopedDecay.value;
        m_accent    = slopedAccent.value;
        m_volume    = slopedVolume.value;
    
    } // End Open303::next()

} // End of namespace Open303

PluginLoad(Open303UGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<Open303::Open303>(ft, "Open303", false);
}
