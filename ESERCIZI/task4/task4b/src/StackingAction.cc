#include "StackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"
#include "Analysis.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
//#include "G4TrackStatus.hh"
//#include "G4ParticleDefinition.hh"
//#include "G4ParticleTypes.hh"
//#include "G4TouchableHistory.hh"
//#include "G4VPhysicalVolume.hh"
//#include "G4VProcess.hh"
//#include <iostream>
//#include <string>
//#include <cmath>


StackingAction::StackingAction()
{
}


StackingAction::~StackingAction() {
}


G4ClassificationOfNewTrack 
StackingAction::ClassifyNewTrack( const G4Track * aTrack ) {

  G4ClassificationOfNewTrack result( fUrgent );
  Analysis* analysis = Analysis::GetInstance();
  if ( aTrack->GetParentID() > 0 )//This is a secondary
  {
		analysis->AddSecondary(1);
  }

  //This method returns the definition of the partycle type
  G4ParticleDefinition * particleType = aTrack->GetDefinition();

  //Count particles above threshold.
  G4double thresh = 30*MeV;

  //-------------
  // Exercise 4b 1
  //-------------
  //You can find what particle is the track comparing its particleType variable with the
  //static definition of Gamma, Nuetron etc:
  // if ( particleType == G4Gamma::GammaDefinition() )
  // Check that the particle is a Gamma and that its kinetic energy is above threshold:
  // to get track kinetic energy use:
  // aTrack->GetKineticEnergy()
  // To increase the counter for gammas of 1 use:
  // analysis->AddGammas(1);
  // do the same for Neutrons using:
  // G4Neutron::NeutronDefinition()
  // and 		  analysis->AddNeutrons(1);

  //--------------
  // Exercise 4b 2
  //--------------
  // To kill a track you need to change the value of result
  // to status fKill:
  // result = fKill;
  // Check if the particle is a gamma and in case kill it.
  return result;

}


