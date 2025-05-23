#ifndef rosic_Open303_h
#define rosic_Open303_h

#include "rosic_MidiNoteEvent.h"
#include "rosic_BlendOscillator.h"
#include "rosic_BiquadFilter.h"
//#include "rosic_TeeBeeFilter.h"
#include "tbrst_TeeBeeFilterMorph.h"
#include "rosic_AnalogEnvelope.h"
#include "rosic_DecayEnvelope.h"
#include "rosic_LeakyIntegrator.h"
#include "rosic_EllipticQuarterBandFilter.h"
#include "GlobalDefinitions.h"  // for linearBlend()

#include <list>
using namespace std; // for the noteList

namespace rosic
{

  /**

  This is a monophonic bass-synth that aims to emulate the sound of the famous Roland TB 303 and
  goes a bit beyond.

  */

  class Open303
  {

  public:

    //-----------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    Open303();

    /** Destructor. */
    ~Open303();

    //-----------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate (in Hz). */
    void setSampleRate(double newSampleRate);

    /** Sets up the waveform continuously between saw and square - the input should be in the range 
    0...1 where 0 means pure saw and 1 means pure square. */
    void setWaveform(double newWaveform) { oscillator.setBlendFactor(newWaveform); }

    /** Sets the master tuning frequency for note A4 (usually 440 Hz). */
    void setTuning(double newTuning) { tuning = newTuning; }

    /** Sets the filter's nominal cutoff frequency (in Hz). */
    void setCutoff(double newCutoff); 

    /** Sets the resonance amount for the filter. */
    void setResonance(double newResonance) { filter.setResonance(newResonance); }

    /** Filter morph (low/band/high-pass) */
    void setFilterMorph(double newFilterMorphPosition) { filter.setFilterMorph(newFilterMorphPosition); }

    /** Sets the modulation depth of the filter's cutoff frequency by the filter-envelope generator 
    (in percent). */
    void setEnvMod(double newEnvMod);

    /** Sets the main envelope's decay time for non-accented notes (in milliseconds). 
    Devil Fish provides range of 30...3000 ms for this parameter. On the normal 303, this 
    parameter had a range of 200...2000 ms.  */
    void setDecay(double newDecay) { normalDecay = newDecay; }

    /** Sets the accent (in percent).  */
    void setAccent(double newAccent);

    /** Sets the master volume level (in dB). */
    void setVolume(double newVolume);   

    //  from here: parameter settings which were not available to the user in the 303:

    /** Set filter overdrive */
    void setFilterDrive(double newFilterDrive);

    /** Sets the amplitudes envelope's sustain level in decibels. Devil Fish uses the second half 
    of the range of the (amplitude) decay pot for this and lets the user adjust it between 0 
    and 100% of the full volume. In the normal 303, this parameter was fixed to zero. */
    void setAmpSustain(double newAmpSustain) { ampEnv.setSustainInDecibels(newAmpSustain); }

    /** Sets the drive (in dB) for the tanh-shaper for 303-square waveform - internal parameter, to 
    be scrapped eventually. */
    void setTanhShaperDrive(double newDrive) 
    { waveTable2.setTanhShaperDriveFor303Square(newDrive); }

    /** Sets the offset (as raw value for the tanh-shaper for 303-square waveform - internal 
    parameter, to be scrapped eventually. */
    void setTanhShaperOffset(double newOffset) 
    { waveTable2.setTanhShaperOffsetFor303Square(newOffset); }

    /** Sets the cutoff frequency for the highpass before the main filter. */
    void setPreFilterHighpass(double newCutoff) { highpass1.setCutoff(newCutoff); }

    /** Sets the cutoff frequency for the highpass inside the feedback loop of the main filter. */
    void setFeedbackHighpass(double newCutoff) { filter.setFeedbackHighpassCutoff(newCutoff); }

    /** Sets the cutoff frequency for the highpass after the main filter. */
    void setPostFilterHighpass(double newCutoff) { highpass2.setCutoff(newCutoff); }

    /** Sets the phase shift of tanh-shaped square wave with respect to the saw-wave (in degrees)
    - this is important when the two are mixed. */
    void setSquarePhaseShift(double newShift) { waveTable2.set303SquarePhaseShift(newShift); }

    /** Sets the slide-time (in ms). The TB-303 had a slide time of 60 ms. */
    void setSlideTime(double newSlideTime);

    /** Sets the filter envelope's attack time for non-accented notes (in milliseconds). 
    Devil Fish provides range of 0.3...30 ms for this parameter. */
    void setNormalAttack(double newNormalAttack) 
    { 
      normalAttack = newNormalAttack; 
      rc1.setTimeConstant(normalAttack);
    }

    /** Sets the filter envelope's attack time for accented notes (in milliseconds). In the 
    Devil Fish, accented notes have a fixed attack time of 3 ms.  */
    void setAccentAttack(double newAccentAttack) 
    { 
      accentAttack = newAccentAttack; 
      rc2.setTimeConstant(accentAttack);
    }

    /** Sets the filter envelope's decay time for accented notes (in milliseconds). 
    Devil Fish provides range of 30...3000 ms for this parameter. On the normal 303, this 
    parameter was fixed to 200 ms.  */
    void setAccentDecay(double newAccentDecay) { accentDecay = newAccentDecay; }

    /** Sets the amplitudes envelope's decay time (in milliseconds). Devil Fish provides range of 
    16...3000 ms for this parameter. On the normal 303, this parameter was fixed to 
    approximately 3-4 seconds.  */
    void setAmpDecay(double newAmpDecay) { ampEnv.setDecay(newAmpDecay); }

    /** Sets the amplitudes envelope's release time (in milliseconds). On the normal 303, this 
    parameter was fixed to .....  */
    void setAmpRelease(double newAmpRelease) 
    { 
      normalAmpRelease = newAmpRelease;
      ampEnv.setRelease(newAmpRelease); 
    }

    /** Sets external input mix-level and audio sample values */
    void setExtIn(double newExtInMix, double newExtInSample)
    {
      extInMix = newExtInMix;
      extInSample = newExtInSample * extInTrim;
    }

    //-----------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the waveform as a continuous value between 0...1 where 0 means pure saw and 1 means 
    pure square. */
    double getWaveform() const { return oscillator.getBlendFactor(); }

    /** Sets the master tuning frequency for note A4 (usually 440 Hz). */
    double getTuning() const { return tuning; }

    /** Returns the filter's nominal cutoff frequency (in Hz). */
    double getCutoff() const { return cutoff; }

    /** Returns the filter's resonance amount (in percent) */
    double getResonance() const { return filter.getResonance(); }

    /** Returns the morphing filter morph position */
    double getFilterMorph() { return filter.getFilterMorph(); }

    /** Returns the modulation depth of the filter's cutoff frequency by the filter-envelope 
    generator (in percent). */
    double getEnvMod() const { return envMod; }

    /** Returns the filter envelope's decay time for non-accented notes (in milliseconds). */
    double getDecay() const { return normalDecay; }

    /** Returns the accent (in percent). */
    double getAccent() const { return 100.0 * accent; }

    /** Returns the master volume level (in dB). */
    double getVolume() const { return level; }

    //  from here: parameters which were not available to the user in the 303:

    /** Returns the amplitudes envelope's sustain level (in dB). */
    double getAmpSustain() const { return amp2dB(ampEnv.getSustain()); }

    /** Returns the drive (in dB) for the tanh-shaper for 303-square waveform - internal parameter, 
    to be scrapped eventually. */
    double getTanhShaperDrive() const 
    { return waveTable2.getTanhShaperDriveFor303Square(); }

    /** Returns the offset (as raw value for the tanh-shaper for 303-square waveform - internal 
    parameter, to be scrapped eventually. */   
    double getTanhShaperOffset() const 
    { return waveTable2.getTanhShaperOffsetFor303Square(); }

    /** Returns the cutoff frequency for the highpass before the main filter. */
    double getPreFilterHighpass() const { return highpass1.getCutoff(); }

    /** Retruns the cutoff frequency for the highpass inside the feedback loop of the main 
    filter. */
    double getFeedbackHighpass() const { return filter.getFeedbackHighpassCutoff(); }

    /** Returns the cutoff frequency for the highpass after the main filter. */
    double getPostFilterHighpass() const { return highpass2.getCutoff(); }

    /** Returns the phase shift of tanh-shaped square wave with respect to the saw-wave (in degrees)
    - this is important when the two are mixed. */
    double getSquarePhaseShift() const { return waveTable2.get303SquarePhaseShift(); }

    /** Returns the slide-time (in ms). */
    double getSlideTime() const { return slideTime; }

    /** Returns the filter envelope's attack time for non-accented notes (in milliseconds). */
    double getNormalAttack() const { return normalAttack; }

    /** Returns the filter envelope's attack time for non-accented notes (in milliseconds). */
    double getAccentAttack() const { return accentAttack; }

    /** Returns the filter envelope's decay time for non-accented notes (in milliseconds). */
    double getAccentDecay() const { return accentDecay; }

    /** Returns the amplitudes envelope's decay time (in milliseconds). */
    double getAmpDecay() const { return ampEnv.getDecay(); }

    /** Returns the amplitudes envelope's release time (in milliseconds). */
    double getAmpRelease() const { return normalAmpRelease; }

    /** Returns external input mixlevel */
    double getExtInMix() const { return extInMix; }

    /** Returns the state all filter-related variables */
    void  getFilterState() { filter.getFilterState(); };


    //-----------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one output sample at a time. */
    INLINE double getSample(); 

    //-----------------------------------------------------------------------------------------------
    // event handling:

    /** Accepts note-on events (note offs are also handled here as note ons with velocity zero). */ 
    void noteOn(int noteNumber, int velocity, double detune);

    /** Turns all possibly running notes off. */
    void allNotesOff();

    /** Sets the pitchbend value in semitones. */ 
    void setPitchBend(double newPitchBend);

     /* Triggers a note (called either directly in noteOn or in getSample when the sequencer is 
    used). */
    void triggerNote(int noteNumber, bool hasAccent);

    /** Slides to a note (called either directly in noteOn or in getSample when the sequencer is 
    used). */
    void slideToNote(int noteNumber, bool hasAccent);

    /** Releases a note (called either directly in noteOn or in getSample when the sequencer is 
    used). */
    void releaseNote(int noteNumber);

    //-----------------------------------------------------------------------------------------------
    // embedded objects: 

    MipMappedWaveTable        waveTable1, waveTable2;
    BlendOscillator           oscillator;
    //TeeBeeFilter              filter;  // standard filter
    TeeBeeFilterMorph         filter; // morphing filter
    AnalogEnvelope            ampEnv; 
    DecayEnvelope             mainEnv;
    LeakyIntegrator           pitchSlewLimiter;
    BiquadFilter              ampDeClicker;
    LeakyIntegrator           rc1, rc2;
    OnePoleFilter             highpass1, highpass2, allpass; 
    BiquadFilter              notch;
    EllipticQuarterBandFilter antiAliasFilter;

  protected:

    /** Sets the decay-time of the main envelope and updates the normalizers n1, n2 accordingly. */
    void setMainEnvDecay(double newDecay);

    void calculateEnvModScalerAndOffset();

    /** Updates the normalizer n1 according to the time-constant of rc1 and the decay-time of the
    main envelope generator. */
    void updateNormalizer1();

    /** Updates the normalizer n2 according to the time-constant of rc2 and the decay-time of the
    main envelope generator. */
    void updateNormalizer2();

    static const int oversampling = 4;

    double tuning;           // master tuning for A4 in Hz
    double ampScaler;        // final volume as raw factor
    double oscFreq;          // frequecy of the oscillator (without pitchbend)
    double sampleRate;       // the (non-oversampled) sample rate
    double level;            // master volume level (in dB)
    double levelByVel;       // velocity dependence of the level (in dB)
    double accent;           // scales all "byVel" parameters
    double slideTime;        // the time to slide from one note to another (in ms)
    double cutoff;           // nominal cutoff frequency of the filter
    double envMod;           // strength of the envelope modulation in percent
    double envUpFraction;    // fraction of the envelope that goes upward
    double envOffset;        // offset for the normalized envelope ('bipolarity' parameter)
    double envScaler;        // scale-factor for the normalized envelope (derived from envMod)
    double normalAttack;     // attack time for the filter envelope on non-accented notes
    double accentAttack;     // attack time for the filter envelope on accented notes
    double normalDecay;      // decay time for the filter envelope on non-accented notes
    double accentDecay;      // decay time for the filter envelope on accented notes
    double normalAmpRelease; // amp-env release time for non-accented notes
    double accentAmpRelease; // amp-env release time for accented notes
    double accentGain;       // between 0.0...1.0 - to scale the 3rd amp-envelope on accents
    double filterDrive;      // filter overdrive
    double extInMix;         // mix of the external input (0.0...1.0)
    double extInSample;      // external input to the filter
    double extInTrim;        // level trim for the external input

    double pitchWheelFactor; // scale factor for oscillator frequency from pitch-wheel
    double n1, n2;           // normalizers for the RCs that are driven by the MEG
    int    currentNote;      // note which is currently played (-1 if none)
    int    currentVel;       // velocity of currently played note
    bool   idle;             // flag to indicate that we have currently nothing to do in getSample

    // MIDI notes list
    list<MidiNoteEvent> noteList;

  };

  //-------------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE double Open303::getSample()
  {
    //if( sequencer.getSequencerMode() == AcidSequencer::OFF && ampEnv.endIsReached() )
    //  return 0.0;
    if( idle )
      return 0.0;

    // calculate instantaneous oscillator frequency and set up the oscillator:
    double instFreq = pitchSlewLimiter.getSample(oscFreq);
    oscillator.setFrequency(instFreq*pitchWheelFactor);
    oscillator.calculateIncrement();

    // calculate instantaneous cutoff frequency from the nominal cutoff and all its modifiers and 
    // set up the filter:
    double mainEnvOut = mainEnv.getSample();
    double tmp1       = n1 * rc1.getSample(mainEnvOut);
    double tmp2       = 0.0;
    if( accentGain > 0.0 )
      tmp2 = mainEnvOut;
    tmp2 = n2 * rc2.getSample(tmp2);  
    tmp1 = envScaler * ( tmp1 - envOffset );  // seems not to work yet
    tmp2 = accentGain*tmp2;
    double instCutoff = cutoff * pow(2.0, tmp1+tmp2);
    filter.setCutoff(instCutoff);

    double ampEnvOut = ampEnv.getSample();
    //ampEnvOut += 0.45*filterEnvOut + accentGain*6.8*filterEnvOut; 
    if( ampEnv.isNoteOn() )
      ampEnvOut += 0.45*mainEnvOut + accentGain*4.0*mainEnvOut; 
    ampEnvOut = ampDeClicker.getSample(ampEnvOut);

    // oversampled calculations:
    double tmp;
    for(int i=1; i<=oversampling; i++)
    {
      tmp  = -oscillator.getSample();                 // the raw oscillator signal
      //tmp  = linearBlend(tmp, extInSample, extInMix); // external input mixed in (linear crossfade bewtween osc and external input)
      tmp  = highpass1.getSample(tmp);                // pre-filter highpass
      tmp  = linearBlend(tmp, extInSample, extInMix); // external input mixed in (linear crossfade bewtween osc and external input)
      tmp  = filter.getSample(tmp);                   // now it's filtered with 303 filter
      tmp  = antiAliasFilter.getSample(tmp);          // anti-aliasing filtered
    }

    // these filters may actually operate without oversampling (but only if we reset them in
    // triggerNote - avoid clicks)
    tmp  = allpass.getSample(tmp);
    tmp  = highpass2.getSample(tmp);        
    tmp  = notch.getSample(tmp);
    tmp *= ampEnvOut;   // amplified
    tmp *= ampScaler;

    // find out whether we may switch ourselves off for the next call:
    idle = false;

    return tmp;
  }

} // End namespace rosic

#endif 
