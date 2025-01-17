/*
 * GarfieldModel.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: dpfeiffe
 */
#include <iostream>
#include "GarfieldG4FastSimulationModel.hh"
#include "G4VPhysicalVolume.hh"
#include "/home/g4user/g4/geant4.10.00.p02/source/persistency/gdml/include/G4GDMLParser.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"

#include "G4SystemOfUnits.hh"

GarfieldG4FastSimulationModel::GarfieldG4FastSimulationModel(G4String modelName,
		G4Region* envelope) :
		G4VFastSimulationModel(modelName, envelope) {

}

GarfieldG4FastSimulationModel::GarfieldG4FastSimulationModel(G4String modelName) :
		G4VFastSimulationModel(modelName) {

}

GarfieldG4FastSimulationModel::~GarfieldG4FastSimulationModel() {
	delete fGarfieldPhysics;
}

void GarfieldG4FastSimulationModel::SetPhysics(GarfieldPhysics* modelPhysics) {
	fGarfieldPhysics = modelPhysics;
	fGarfieldPhysics->InitializePhysics();
}

void GarfieldG4FastSimulationModel::WriteGeometryToGDML(
		G4VPhysicalVolume* physicalVolume) {

	G4GDMLParser* parser = new G4GDMLParser();
	remove("drift.gdml");
	parser->Write("drift.gdml", physicalVolume, false);
	delete parser;
}

G4bool GarfieldG4FastSimulationModel::IsApplicable(
			const G4ParticleDefinition& particleType) {
	G4String particleName = particleType.GetParticleName();
	//G4cout << "\nGarfield model applicable for " << particleName << "?"
	//		<< G4endl;
	if (fGarfieldPhysics->FindParticleName(particleName) ) {
		//G4cout << "YES!\n" << G4endl;
		return true;
	}
	//G4cout << "NO!\n" << G4endl;
	return false;

}

G4bool GarfieldG4FastSimulationModel::ModelTrigger(
		const G4FastTrack& fastTrack) {
	double ekin_keV = fastTrack.GetPrimaryTrack()->GetKineticEnergy() / keV;
	G4String particleName =
			fastTrack.GetPrimaryTrack()->GetParticleDefinition()->GetParticleName();
	if(fastTrack.GetPrimaryTrack()->GetParentID() != 0)
	{
		return fGarfieldPhysics->FindParticleNameEnergy(particleName, ekin_keV);
	}
	else
	{
		return false;
	}

}

void GarfieldG4FastSimulationModel::DoIt(const G4FastTrack& fastTrack,
		G4FastStep& fastStep) {

	G4TouchableHandle theTouchable =
			fastTrack.GetPrimaryTrack()->GetTouchableHandle();
	G4ThreeVector pdirection =
			fastTrack.GetPrimaryTrack()->GetMomentumDirection();
	G4ThreeVector localdir =
			theTouchable->GetHistory()->GetTopTransform().TransformAxis(
					pdirection);

	G4ThreeVector worldPosition = fastTrack.GetPrimaryTrack()->GetPosition();
	G4ThreeVector localPosition =
			theTouchable->GetHistory()->GetTopTransform().TransformPoint(
					worldPosition);

	double ekin_keV = fastTrack.GetPrimaryTrack()->GetKineticEnergy() / keV;
	G4double globalTime = fastTrack.GetPrimaryTrack()->GetGlobalTime();

	G4String particleName =
			fastTrack.GetPrimaryTrack()->GetParticleDefinition()->GetParticleName();

	fastStep.KillPrimaryTrack();
	fGarfieldPhysics->DoIt(particleName, ekin_keV, globalTime,
			localPosition.x() / CLHEP::cm, localPosition.y() / CLHEP::cm,
			localPosition.z() / CLHEP::cm, localdir.x(), localdir.y(),
			localdir.z(), false);

	std::vector<GarfieldElectron*>* secondaryElectrons =
			fGarfieldPhysics->GetSecondaryElectrons();

	if (secondaryElectrons->size() > 0) {
		fastStep.SetNumberOfSecondaryTracks(secondaryElectrons->size());

		G4double totalEnergySecondaries_MeV = 0;

		for (std::vector<GarfieldElectron*>::iterator it =
				secondaryElectrons->begin(); it != secondaryElectrons->end();
				++it) {
			G4double x = (*it)->getX_mm();
			G4double y = (*it)->getY_mm();
			G4double z = (*it)->getZ_mm();
			G4double eKin_MeV = (*it)->getEkin_MeV();
			G4double dx = (*it)->getDX();
			G4double dy = (*it)->getDY();
			G4double dz = (*it)->getDZ();
			G4double time = (*it)->getTime();
			G4cout << "time " << time << " globaltime " << globalTime << G4endl;
			G4ThreeVector momentumDirection(dx, dy, dz);
			G4ThreeVector position(x, y, z);

			G4DynamicParticle electron(G4Electron::ElectronDefinition(),
					momentumDirection, eKin_MeV);

			fastStep.CreateSecondaryTrack(electron, position, globalTime, true);
			totalEnergySecondaries_MeV += eKin_MeV;
		}
	}

}
