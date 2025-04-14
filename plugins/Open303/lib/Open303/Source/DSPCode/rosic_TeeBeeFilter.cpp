#include <iostream> // For cout

#include "rosic_TeeBeeFilter.h"

using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

TeeBeeFilter::TeeBeeFilter()
{
  cutoff              =  1000.0;
  drive               =     0.0;
  driveFactor         =     1.0;
  resonanceRaw        =     0.0;
  resonanceSkewed     =     0.0;
  mode                =  TB_303;
  oldMode             =    mode;
  g                   =     1.0;
  sampleRate          = 44100.0;
  twoPiOverSampleRate = 2.0*PI/sampleRate;

  feedbackHighpass.setMode(OnePoleFilter::HIGHPASS);
  feedbackHighpass.setCutoff(150.0);

  setMode(mode);
  calculateCoefficientsExact();
  
  // DEBUG: print current filter coefficients
  std::cout << "PLUGIN Filter init (calculateCoefficientsExact):\n";
  getFilterCoefficients();

  reset();
}

TeeBeeFilter::~TeeBeeFilter()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void TeeBeeFilter::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;
  twoPiOverSampleRate = 2.0*PI/sampleRate;
  feedbackHighpass.setSampleRate(newSampleRate);
  calculateCoefficientsExact();
}

void TeeBeeFilter::setDrive(double newDrive)
{
  drive       = newDrive;
  driveFactor = dB2amp(drive);
}

void TeeBeeFilter::setMode(int newMode)
{
  if( (newMode >= 0) && (newMode < NUM_MODES) && (newMode != oldMode))
  {
    mode = newMode;
    switch(mode)
    {
      case FLAT:      c0 =  1.0; c1 =  0.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  break;
      case LP_6:      c0 =  0.0; c1 =  1.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  break;
      case LP_12:     c0 =  0.0; c1 =  0.0; c2 =  1.0; c3 =  0.0; c4 =  0.0;  break;
      case LP_18:     c0 =  0.0; c1 =  0.0; c2 =  0.0; c3 =  1.0; c4 =  0.0;  break;
      case LP_24:     c0 =  0.0; c1 =  0.0; c2 =  0.0; c3 =  0.0; c4 =  1.0;  break;
      case HP_6:      c0 =  1.0; c1 = -1.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  break;
      case HP_12:     c0 =  1.0; c1 = -2.0; c2 =  1.0; c3 =  0.0; c4 =  0.0;  break;
      case HP_18:     c0 =  1.0; c1 = -3.0; c2 =  3.0; c3 = -1.0; c4 =  0.0;  break;
      case HP_24:     c0 =  1.0; c1 = -4.0; c2 =  6.0; c3 = -4.0; c4 =  1.0;  break;
      case BP_12_12:  c0 =  0.0; c1 =  0.0; c2 =  1.0; c3 = -2.0; c4 =  1.0;  break;
      case BP_6_18:   c0 =  0.0; c1 =  0.0; c2 =  0.0; c3 =  1.0; c4 = -1.0;  break;
      case BP_18_6:   c0 =  0.0; c1 =  1.0; c2 = -3.0; c3 =  3.0; c4 = -1.0;  break;
      case BP_6_12:   c0 =  0.0; c1 =  0.0; c2 =  1.0; c3 = -1.0; c4 =  0.0;  break;
      case BP_12_6:   c0 =  0.0; c1 =  1.0; c2 = -2.0; c3 =  1.0; c4 =  0.0;  break;
      case BP_6_6:    c0 =  0.0; c1 =  1.0; c2 = -1.0; c3 =  0.0; c4 =  0.0;  break;
      default:        c0 =  1.0; c1 =  0.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  // flat
    }
    calculateCoefficientsApprox4();
    reset();
  }
  oldMode = newMode;
}

//-------------------------------------------------------------------------------------------------
// others:

void TeeBeeFilter::reset()
{
  feedbackHighpass.reset();
  y1 = 0.0;
  y2 = 0.0;
  y3 = 0.0;
  y4 = 0.0;
}

// DEBUG: Print current filter coefficients
void TeeBeeFilter::getFilterCoefficients()
{
  std::cout << "b0: " << b0 << "\n";
  std::cout << "a1: " << a1 << "\n";
  std::cout << "y1: " << y1 << "\n";
  std::cout << "y2: " << y2 << "\n";
  std::cout << "y3: " << y3 << "\n";
  std::cout << "y4: " << y4 << "\n";
  std::cout << "c0: " << c0 << "\n";
  std::cout << "c1: " << c1 << "\n";
  std::cout << "c2: " << c2 << "\n";
  std::cout << "c3: " << c3 << "\n";
  std::cout << "c4: " << c4 << "\n";
  std::cout << "k: "  << k  << "\n";
  std::cout << "PLUGIN End filter coefficients\n";
}
