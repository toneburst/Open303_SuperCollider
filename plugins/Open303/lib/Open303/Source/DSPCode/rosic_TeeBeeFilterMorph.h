#ifndef rosic_TeeBeeFilterMorph_h
#define rosic_TeeBeeFilterMorph_h

// standard-library includes:
#include <stdlib.h>             // for the NULL macro

// rosic-indcludes:
#include "rosic_TeeBeeFilter.h"
#include "rosic_NumberManipulations.h"
#include "GlobalDefinitions.h"  // for linearBlend()

namespace rosic
{

  /**

    Crossfaded outpur of two instances of TeeBeeFilter for continuously-variable filter response from low-pass (standard 303 filter) > band-pass > high-pass.

    This is a naive implementation. It would probably be better to morph filter coefficients, but I'm not clear on how I'd do that at this point.

  */

  class TeeBeeFilterMorph
  {

  public:

    /** Enumeration of the available filter modes. */
    enum modes
    {
      FLAT = 0,
      LP_6,
      LP_12,
      LP_18,
      LP_24,
      HP_6,
      HP_12,
      HP_18,
      HP_24,
      BP_12_12,
      BP_6_18,
      BP_18_6,
      BP_6_12,
      BP_12_6,
      BP_6_6,
      TB_303,      // ala mystran & kunn (page 40 in the kvr-thread)

      NUM_MODES
    };

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    TeeBeeFilterMorph();

    /** Destructor. */
    ~TeeBeeFilterMorph();

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate for this filter. */
    void setSampleRate(double newSampleRate);

    /** Sets the cutoff frequency for this filter - the actual coefficient calculation may be 
    supressed by passing 'false' as second parameter, in this case, it should be triggered
    manually later by calling calculateCoefficients. */
    INLINE void setCutoff(double newCutoff, bool updateCoefficients = true);

    /** Sets the resonance in percent where 100% is self oscillation. */
    INLINE void setResonance(double newResonance, bool updateCoefficients = true);

    /** Sets the input drive in decibels. */
    void setDrive(double newDrive);

    /** Sets the cutoff frequency for the highpass filter in the feedback path. */
    void setFeedbackHighpassCutoff(double newCutoff)
    { 
      filter0.setFeedbackHighpassCutoff(newCutoff);
      filter1.setFeedbackHighpassCutoff(newCutoff);
    }

    /** Set filter morph. */
    void setFilterMorph(double newMorphPosition);

    /** Resets the internal state variables. */
    void reset();

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the cutoff frequency of this filter. */
    double getCutoff() const {return filter0.getCutoff(); }

    /** Returns the resonance parameter of this filter. */
    double getResonance() const { return filter0.getResonance(); }

    /** Returns the drive parameter in decibels. */
    double getDrive() const { return filter0.getDrive(); }

    /** Returns filter morph-position */
    double getFilterMorph() { return morphPosition; };

    /** Returns the cutoff frequency for the highpass filter in the feedback path. */
    double getFeedbackHighpassCutoff() const { return filter0.getFeedbackHighpassCutoff(); }

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one output sample at a time. */
    INLINE double getSample(double in);

    //-----------------------------------------------------------------------------------------------
    // embedded objects: 

    TeeBeeFilter              filter0, filter1;

  protected:

    double morphPosition;       // morph position

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE void TeeBeeFilterMorph::setCutoff(double newCutoff, bool updateCoefficients)
  {
    filter0.setCutoff(newCutoff, updateCoefficients);
    filter1.setCutoff(newCutoff, updateCoefficients);
  }

  INLINE void TeeBeeFilterMorph::setResonance(double newResonance, bool updateCoefficients)
  {
    filter0.setResonance(newResonance, updateCoefficients);
    filter1.setResonance(newResonance, updateCoefficients);
  }

  INLINE double TeeBeeFilterMorph::getSample(double in)
  {
    double fracPos, intPos, f0, f1, out;

    fracPos = morphPosition * 2.0;
    fracPos = modf(pos, &intPos);

    f0 = filter0.getSample(in);
    f1 = filter1.getSample(in);

    if(morphPosition <= 0.5) {
      // blend a > b
      out = linearBlend(f0, f1, fracPos);
    } else {
      // blend b > a
      out = linearBlend(f1, f0, fracPos);
    }

    return out;
  }

}

#endif // rosic_TeeBeeFilterMorph_h
