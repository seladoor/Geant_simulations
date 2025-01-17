//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file NeutronGEMDetectorConstruction.cc
/// \brief Implementation of the NeutronGEMDetectorConstruction class

#include "NeutronGEMDetectorConstruction.hh"
#include "NeutronGEMDataManager.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4UnitsTable.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "/home/g4user/g4/geant4.10.00.p02/source/persistency/gdml/include/G4GDMLParser.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ProductionCuts.hh"
#include <TGeoManager.h>
#include <TCanvas.h>

#include "G4ElectricField.hh"
#include "G4EqMagElectricField.hh"
#include "G4UniformElectricField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4ClassicalRK4.hh"

#include "GarfieldG4FastSimulationModel.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NeutronGEMDetectorConstruction::NeutronGEMDetectorConstruction() :
		G4VUserDetectorConstruction(), fCheckOverlaps(true), fWorldSize(0), fCathodeThickness(
				0), fConverterThickness(0), fSEEThickness(0), fDriftThickness(
				0), fDriftField(0) {
	fDataManager = NeutronGEMDataManager::GetInstance();

	DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NeutronGEMDetectorConstruction::~NeutronGEMDetectorConstruction() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NeutronGEMDetectorConstruction::DefineMaterials() {

	G4NistManager* nistManager = G4NistManager::Instance();

	G4bool isotopes = false;
	G4String name, symbol;
	G4int z, n, ncomponents, natoms;
	G4double a, density, abundance;
	G4double fractionmass;

	G4Element* C = nistManager->FindOrBuildElement("C", isotopes);
	G4Element* O = nistManager->FindOrBuildElement("O", isotopes);
	G4Element* Ar = nistManager->FindOrBuildElement("Ar", isotopes);
	G4Element* GdElement = nistManager->FindOrBuildElement("Gd", true);

	fWorldMaterial = nistManager->FindOrBuildMaterial("G4_AIR");
	G4Material* Al = nistManager->FindOrBuildMaterial("G4_Al");
	G4Material* Cu = nistManager->FindOrBuildMaterial("G4_Cu");
	G4Material* CsI = nistManager->FindOrBuildMaterial("G4_CESIUM_IODIDE");
	G4Material* Gd2O2S = nistManager->FindOrBuildMaterial(
			"G4_GADOLINIUM_OXYSULFIDE");
	G4Material* Gd = nistManager->FindOrBuildMaterial("G4_Gd");

	G4Material* Al2O3 = nistManager->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");

	G4Isotope* B10 = new G4Isotope(name = "B10", z = 5, n = 10,
			a = 10.0129370 * CLHEP::g / CLHEP::mole);

	G4Element* elB10 = new G4Element(name = "elB10", symbol = "B", ncomponents =
			1);
	elB10->AddIsotope(B10, abundance = 100. * CLHEP::perCent);

	density = 2.51 * CLHEP::g / CLHEP::cm3;
	G4Material * B4C = new G4Material(name = "10B4C", density, ncomponents = 2);
	B4C->AddElement(elB10, natoms = 4);
	B4C->AddElement(C, natoms = 1);

	G4Isotope* Gd157 = new G4Isotope(name = "Gd157", z = 64, n = 157,
			a = 156.9239601 * CLHEP::g / CLHEP::mole);

	G4Element* elGd157 = new G4Element(name = "elGd157", symbol = "Gd",
			ncomponents = 1);
	elGd157->AddIsotope(Gd157, abundance = 100. * CLHEP::perCent);

	density = 7.9 * CLHEP::g / CLHEP::cm3;
	G4Material* enrichedGd = new G4Material(name = "enrichedGd", density,
			ncomponents = 1);
	enrichedGd->AddElement(elGd157, natoms = 1);

	G4Isotope* Gd155 = new G4Isotope(name = "Gd155", z = 64, n = 155,
			a = 154.9239601 * CLHEP::g / CLHEP::mole);
	//a = 156.9239601 * CLHEP::g / CLHEP::mole);

	G4Element* elGd155 = new G4Element(name = "elGd155", symbol = "Gd",
			ncomponents = 1);
	elGd155->AddIsotope(Gd155, abundance = 100. * CLHEP::perCent);

	density = 7.9 * CLHEP::g / CLHEP::cm3;
	G4Material* enrichedGd155 = new G4Material(name = "enrichedGd155", density,
			ncomponents = 1);
	enrichedGd155->AddElement(elGd155, natoms = 1);

	G4Element* elGd155_157 = new G4Element(name = "elGd155_157", symbol = "Gd",
			ncomponents = 2);
	elGd155_157->AddIsotope(Gd155, abundance = 50. * CLHEP::perCent);
	elGd155_157->AddIsotope(Gd157, abundance = 50. * CLHEP::perCent);

	density = 7.9 * CLHEP::g / CLHEP::cm3;
	G4Material* enrichedGd155_157 = new G4Material(name = "enrichedGd155_157",
			density, ncomponents = 1);
	enrichedGd155_157->AddElement(elGd155_157, 1);

	density = 0.25 * 7.6 * CLHEP::g / CLHEP::cm3;
	G4Material* Gd2O3 = new G4Material(name = "Gd2O3", density, ncomponents =
			2);
	Gd2O3->AddElement(GdElement, natoms = 2);
	Gd2O3->AddElement(O, natoms = 3);

	density = 7.6 * CLHEP::g / CLHEP::cm3;
	G4Material* enrichedGd2O3 = new G4Material(name = "enrichedGd2O3", density,
			ncomponents = 2);
	enrichedGd2O3->AddElement(elGd157, natoms = 2);
	enrichedGd2O3->AddElement(O, natoms = 3);

	G4Material* CO2 = new G4Material("CO2", density = 1.977 * CLHEP::mg / CLHEP::cm3,
			ncomponents = 2);
	CO2->AddElement(C, natoms = 1);
	CO2->AddElement(O, natoms = 2);

	G4Material* ArCO2 = new G4Material("ArCO2", density = 1.8223 * CLHEP::mg / CLHEP::cm3,
			ncomponents = 2);
	ArCO2->AddElement(Ar, fractionmass = 0.70);
	ArCO2->AddMaterial(CO2, fractionmass = 0.30);

	fDriftMaterial = ArCO2;

	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
	G4String cathodeMaterial = fDataManager->getCathodeMaterial();

	if (cathodeMaterial == "Al") {
		fCathodeMaterial = Al;
	} else if (cathodeMaterial == "Cu") {
		fCathodeMaterial = Cu;
	}

	G4String converterMaterial = fDataManager->getConverterMaterial();
	if (converterMaterial == "Gd") {
		fConverterMaterial = Gd;
	} else if (converterMaterial == "enrichedGd") {
		fConverterMaterial = enrichedGd;
	} else if (converterMaterial == "enrichedGd155") {
		fConverterMaterial = enrichedGd155;

	} else if (converterMaterial == "enrichedGd155_157") {
		fConverterMaterial = enrichedGd155_157;
	} else if (converterMaterial == "Gd2O3") {
		fConverterMaterial = Gd2O3;
	} else if (converterMaterial == "enrichedGd2O3") {
		fConverterMaterial = enrichedGd2O3;
	} else if (converterMaterial == "Gd2O2S") {
		fConverterMaterial = Gd2O2S;
	} else if (converterMaterial == "B4C") {
		fConverterMaterial = B4C;
	}

	G4String SEEMaterial = fDataManager->getSEEMaterial();
	if (SEEMaterial == "CsI") {
		fSEEMaterial = CsI;
	} else if (SEEMaterial == "Al2O3") {
		fSEEMaterial = Al2O3;
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* NeutronGEMDetectorConstruction::Construct() {
	fCathodeThickness = fDataManager->getCathodeThickness() * CLHEP::um;

	fConverterThickness = fDataManager->getConverterThickness() * CLHEP::um;

	fSEEThickness = fDataManager->getSEEThickness() * CLHEP::nm;
	fDriftThickness = fDataManager->getDriftThickness() * CLHEP::mm;

	fWorldSize = 10000 * CLHEP::mm;

	fDetectorSize = 100 * CLHEP::mm;

	fDriftField = fDataManager->getDriftField();

	G4ElectricField* fEMfield;
	G4EqMagElectricField* fEquation;
	G4MagIntegratorStepper* fStepper;
	G4double fMinStep;
	G4ChordFinder* fChordFinder;
	G4MagInt_Driver* fIntgrDriver;
	G4FieldManager* fFieldMgr;

	if (fDriftField > 0) {
		fEMfield = new G4UniformElectricField(
				G4ThreeVector(0.0, 0.0,
						-fDriftField * CLHEP::volt / CLHEP::cm));

		fEquation = new G4EqMagElectricField(fEMfield);

		G4int nvar = 8;
		fStepper = new G4ClassicalRK4(fEquation, nvar);

		fMinStep = 10 * CLHEP::nm;

		fIntgrDriver = new G4MagInt_Driver(fMinStep, fStepper,
				fStepper->GetNumberOfVariables());
		fChordFinder = new G4ChordFinder(fIntgrDriver);
	}

	fSolidWorld = new G4Box("world", 0.5 * fWorldSize, 0.5 * fWorldSize,
			0.5 * fWorldSize);
	fLogicalWorld = new G4LogicalVolume(fSolidWorld, fWorldMaterial, "World", 0,
			0, 0);
	fPhysicalWorld = new G4PVPlacement(0,              // no rotation
			G4ThreeVector(), // at (0,0,0)
			fLogicalWorld,     // its logical volume
			"World",         // its name
			0,               // its mother  volume
			false,           // no boolean operations
			0, true);              // no field specific to volume

	G4ThreeVector positionConverter = G4ThreeVector(0, 0,
			0.5 * fConverterThickness);
	fSolidConverter = new G4Box("SolidConverter", 0.5 * fDetectorSize,
			0.5 * fDetectorSize, 0.5 * fConverterThickness);
	fLogicalConverter = new G4LogicalVolume(fSolidConverter, fConverterMaterial,
			"LogicalConverter", 0, 0, 0);
	fPhysicalConverter = new G4PVPlacement(0, positionConverter,
			fLogicalConverter, "Converter", fLogicalWorld, false, 0, true);

	G4ThreeVector positionCathode = G4ThreeVector(0, 0,
			fConverterThickness + 0.5 * fCathodeThickness);
	fSolidCathode = new G4Box("SolidCathode", 0.5 * fDetectorSize,
			0.5 * fDetectorSize, fCathodeThickness * 0.5);
	fLogicalCathode = new G4LogicalVolume(fSolidCathode, fCathodeMaterial,
			"LogicalCathode", 0, 0, 0);
	fPhysicalCathode = new G4PVPlacement(0, positionCathode, fLogicalCathode,
			"Cathode", fLogicalWorld, false, 0, true);

	if (fSEEThickness > 0) {
		G4ThreeVector positionSEE = G4ThreeVector(0, 0,
				fCathodeThickness + fConverterThickness + fSEEThickness * 0.5);
		fSolidSEE = new G4Box("SolidSEE", 0.5 * fDetectorSize,
				0.5 * fDetectorSize, fSEEThickness * 0.5);
		fLogicalSEE = new G4LogicalVolume(fSolidSEE, fSEEMaterial, "LogicalSEE",
				0, 0, 0);
		fPhysicalSEE = new G4PVPlacement(0, positionSEE, fLogicalSEE, "SEE",
				fLogicalWorld, false, 0, true);
		G4VisAttributes* SEEVisAtt = new G4VisAttributes(
				G4Colour(0.0, 1.0, 1.0));
		fLogicalSEE->SetVisAttributes(SEEVisAtt);
	}

	G4ThreeVector positionDrift = G4ThreeVector(0, 0,
			fConverterThickness + fCathodeThickness + fSEEThickness
					+ fDriftThickness * 0.5);
	fSolidDrift = new G4Box("SolidDrift", 0.5 * fDetectorSize,
			0.5 * fDetectorSize, fDriftThickness * 0.5);
	fLogicalDrift = new G4LogicalVolume(fSolidDrift, fDriftMaterial,
			"LogicalDrift", 0, 0, 0);
	fPhysicalDrift = new G4PVPlacement(0, positionDrift, fLogicalDrift, "Drift",
			fLogicalWorld, false, 0, true);

	if (fDriftField > 0) {
		fFieldMgr = new G4FieldManager(fEMfield);
		fFieldMgr->SetDetectorField(fEMfield);
		fFieldMgr->SetChordFinder(fChordFinder);
		fLogicalDrift->SetFieldManager(fFieldMgr, true);
	}

	//--------- Visualization attributes -------------------------------
	fLogicalWorld->SetVisAttributes(G4VisAttributes::Invisible);

	G4VisAttributes* CathodeVisAtt = new G4VisAttributes(
			G4Colour(1.0, 0.0, 0.0));
	fLogicalCathode->SetVisAttributes(CathodeVisAtt);

	G4VisAttributes* ConverterVisAtt = new G4VisAttributes(
			G4Colour(0.0, 1.0, .0));
	fLogicalConverter->SetVisAttributes(ConverterVisAtt);

	G4VisAttributes* DriftVisAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
	fLogicalDrift->SetVisAttributes(DriftVisAtt);

	G4cout << "fWorldSize " << fWorldSize << G4endl;
	G4cout << "fCathodeThickness " << fCathodeThickness << G4endl;
	G4cout << "fConverterThickness " << fConverterThickness << G4endl;
	G4cout << "fSEEThickness " << fSEEThickness << G4endl;
	G4cout << "fDriftThickness " << fDriftThickness << G4endl;

	G4Region* regionGarfield = new G4Region("RegionGarfield");
	regionGarfield->AddRootLogicalVolume(fLogicalDrift);
	fGarfieldG4FastSimulationModel = new GarfieldG4FastSimulationModel(
			"GarfieldG4FastSimulationModel", regionGarfield);

	G4double cutValue = 0.01 * CLHEP::mm;

	G4ProductionCuts* cuts = new G4ProductionCuts();
	cuts->SetProductionCut(cutValue, G4ProductionCuts::GetIndex("gamma"));
	cuts->SetProductionCut(cutValue, G4ProductionCuts::GetIndex("e-"));
	cuts->SetProductionCut(cutValue, G4ProductionCuts::GetIndex("e+"));
	regionGarfield->SetProductionCuts(cuts);

	fGarfieldG4FastSimulationModel->WriteGeometryToGDML(fPhysicalDrift);

	return fPhysicalWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GarfieldG4FastSimulationModel* NeutronGEMDetectorConstruction::GetGarfieldG4FastSimulationModel() {
	return fGarfieldG4FastSimulationModel;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

