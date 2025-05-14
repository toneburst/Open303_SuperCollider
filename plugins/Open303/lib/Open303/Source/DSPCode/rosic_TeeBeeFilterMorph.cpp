// rosic-includes:
#include "rosic_TeeBeeFilterMorph.h"
#include "rosic_NumberManipulations.h"

using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

TeeBeeFilterMorph::TeeBeeFilterMorph()
{
  morphPosition = 0.0;
  filter0.setMode(15);  // TB_303 mode initially
  filter1.setMode(9);   // BP_12_12 bandpass mode (remains in this mode)
}

TeeBeeFilterMorph::~TeeBeeFilterMorph()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void TeeBeeFilterMorph::setSampleRate(double newSampleRate)
{
  filter0.setSampleRate(newSampleRate);
  filter1.setSampleRate(newSampleRate);
}

void TeeBeeFilterMorph::setDrive(double newDrive)
{
  filter0.setDrive(newDrive);
  filter1.setDrive(newDrive);
}

void TeeBeeFilterMorph::setFilterMorph(double newMorphPosition)
{
  morphPosition = newMorphPosition;
  // Change filter0 filter mode
  // filter1 stays in band-pass mode. This way, filter 1 only switches mode when it's silent (avoiding clicks).
  if(morphPosition <= 0.5) {
    filter0.setMode(15);  // TB_303 mode
  } else {
    filter0.setMode(6);   // HP_12 high-pass mode
  }
}

//-------------------------------------------------------------------------------------------------
// others:

void TeeBeeFilterMorph::reset()
{
  filter0.reset();
  filter1.reset();
}

void TeeBeeFilterMorph::getFilterState() 
{
  std::cout << "Filter 0 state: \n";
  filter0.getFilterState();
  std::cout << "Filter 1 state: \n";
  filter1.getFilterState(); 
}
