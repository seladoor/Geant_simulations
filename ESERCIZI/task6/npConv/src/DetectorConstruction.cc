// $Id: DetectorConstruction.cc 94 2010-01-26 13:18:30Z adotti $
/**
 * @file
 * @brief Implements mandatory user class DetectorConstruction.
 */

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4NistManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "SensitiveDetector.hh"
#include "G4SDManager.hh"

DetectorConstruction::DetectorConstruction()
  : vacuum(0) 
  , Ar_Mat(0)
  , PE_Mat(0)
  , logicWorld(0)
  , halfWorldLength(0.5*km)
{
  //Create a messanger (defines custom UI commands)
  messenger = new DetectorMessenger(this);
  
  //--------- Material definition ---------
  DefineMaterials();
  
  //            	//--------- Sizes of the principal 
  //              geometrical components (solids)  ---------
  ComputeParameters();
}

DetectorConstruction::~DetectorConstruction()
{
  delete messenger;
  delete Ar_Mat;
  delete PE_Mat;

}

void DetectorConstruction::DefineMaterials() 
{
  //Get Materials from NIST database
  G4NistManager* man = G4NistManager::Instance();
  man->SetVerbose(10);
  
  // define NIST materials
  
  //// 1 - Polyethilene (CH2) /////////////
  
  //Retrieving copper from G4NIST Material

  G4int hydrogenZ = 1;
  G4int carbonZ   = 6;
  
  G4Element* H = man->FindOrBuildElement(hydrogenZ);
  
  if(H) G4cout << "Hydrogen correctly retrieved" << G4endl;

  G4Element* C = man->FindOrBuildElement(carbonZ);
  
  if(C) G4cout << "Carbon correctly retrieved" << G4endl;


  G4cout << "Building PE Material" << G4endl;

  G4double PE_density = 0.935* g/cm3; // mean PE density
  
  G4int PE_comp_Atoms = 2;
  G4int n_H_Atoms = 2;
  G4int n_C_Atoms = 1;
  
  PE_Mat = new G4Material("PEMat", PE_density, PE_comp_Atoms);

  PE_Mat->AddElement(H,n_H_Atoms);
  PE_Mat->AddElement(C,n_C_Atoms);
  
  if (!PE_Mat)   G4cout << "Problems making Cu Material " << G4endl;


  /// 2 - ARGON //////
  
  G4int ArgonZ = 18;
  G4Element* Ar = man->FindOrBuildElement(ArgonZ);

  if(Ar) G4cout << "Argon correctly retrieved" << G4endl;
  
  G4cout << "Building High density germanium Material" << G4endl;

  G4double argon_density = 0.001784* g/cm3;

  G4int Ar_comp_Atoms = 1;

  Ar_Mat = new G4Material("Ar_Mat",argon_density,Ar_comp_Atoms,kStateGas); ///You have to specify that it is a GAS!!!!);
  
  Ar_Mat->AddElement(Ar,Ar_comp_Atoms);

  if (!Ar_Mat)   G4cout << "Problems making Argon Material " << G4endl;

  /// 3 - Retrieving vacuum from G4NIST database

  vacuum  = man->FindOrBuildMaterial("G4_Galactic");
}
 
void DetectorConstruction::ComputeParameters() 
{
  //This function defines the defaults
  //of the geometry construction
  
  // ** world **
  halfWorldLength = 500*m;
  
  // ** PE_Conv **
  
  halfPEConv_x = 10* cm;
  halfPEConv_y = 10* cm;
  halfPEConv_z = 50* um;
    
  //Argon Detector

  halfArDet_x = 10* cm;
  halfArDet_y = 10* cm;
  halfArDet_z =  3* mm;
  
  ArDet_z_displacement = 1*cm;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	//This function is called by G4 when the detector has to be created
	//--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------


  ///------WORLD-------------------------------------///
  
  
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(2.*halfWorldLength);
  G4cout << "Computed tolerance = "
	 << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
	 << " mm" << G4endl;

  
  //Define Box for the world
  G4Box * solidWorld= new G4Box("world",
				halfWorldLength,  //half x dimension
				halfWorldLength,  //half y dimension
				halfWorldLength); //half z dimension
  
  
  logicWorld= new G4LogicalVolume(solidWorld, //corresponding G4Solid 
				  vacuum,      //corresponding G4Material: 
				               //              I set vacuum
				  "World",    //Name
				  0,          //FieldMamanger
				  0,          //Sensitive detector
				  0,          //User limits
				  0);         //Optimise
  
  
  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  G4VPhysicalVolume* physiWorld 
    = new G4PVPlacement(0,               // no rotation
			G4ThreeVector(), // at (0,0,0)
			logicWorld,      // its logical volume
			"World",         // its name
			0,               // its mother  volume
			false,           // no boolean operations
			0);              // copy number
  
  
    
  //--------- Visualization attributes -------------------------------
  
  G4Color
    green(0.0,1.0,0.0),
    blue(0.0,0.0,1.0),
    brown(0.4,0.4,0.1),
    white(1.0,1.0,1.0);
  
  logicWorld -> SetVisAttributes(new G4VisAttributes(white));
  logicWorld -> SetVisAttributes(G4VisAttributes::Invisible);
  
  
    
  //always return the physical World!!!!!!!!!!!!!!!!!1
  //the typedef of this method is G4VPhysicalVolume
  
  //Call the method to construct all the rest
  
  Construct_PEConv_ArDet();
    
  
  return physiWorld;
}

G4VPhysicalVolume* DetectorConstruction::Construct_PEConv_ArDet()
    {
     
      G4Color
	green(0.0,1.0,0.0),
 	blue(0.0,0.0,1.0),
 	brown(0.4,0.4,0.1),
 	white(1.0,1.0,1.0);
      
      

      //*****************Create the PE Converter****************************//
                 
      G4Box * solidPEConv = new G4Box("solid_BigArC02Block",
					halfPEConv_x,
					halfPEConv_y,
					halfPEConv_z);
      
      logicPEConv = 
	new G4LogicalVolume(solidPEConv,        // its solid
			    PE_Mat,	        //its material --> PE Converter
			    "logic_PEConv");	//its name
      
      G4int PEConv_copynum = 0;

      physiPEConv = new G4PVPlacement(0,	//no rotation
					G4ThreeVector(), //put at 0,0,0
					logicPEConv,//its logical volume
					"physical__PEConv",   //its name
					logicWorld,	    //its mother  volume
					false,	    //no boolean operation
					PEConv_copynum);		    //copy number
      
      
      logicPEConv -> SetVisAttributes(new G4VisAttributes(green));

      //**************************************************************//

      //*****************Construct Argon Detector************************//
      
      //Define the solid/

      G4Box * solidArDet = 
       new G4Box("solid_ArDet",
		 halfArDet_x,
		 halfArDet_y,
		 halfArDet_z);

      //Define logic volume for Argon detector and its color
   
     logicArDet = 
       new G4LogicalVolume(solidArDet, // its solid
			   Ar_Mat,	//its material 
			   "logic_ArDet");	//its name
   
     logicArDet -> SetVisAttributes(new G4VisAttributes(blue));


     G4int ArDet_copynum = 1;

     physiArDet = 
       new G4PVPlacement(0,
			 G4ThreeVector(0,0,halfPEConv_z+ArDet_z_displacement+halfArDet_z),
			 logicArDet,
			 "physical_ArDet",
			 logicWorld,
			 false,
			 ArDet_copynum);

     //Define The HD Ge detector as Sensitive Detector

     static SensitiveDetector* sensitive = 0;
     if ( !sensitive) 
       {
	 G4cout << "DECLARING SENSITIVE DETECTOR" << G4endl;
	 sensitive = new SensitiveDetector("/myDet/ArCO2");
	 //We register now the SD with the manager
	 G4SDManager::GetSDMpointer()->AddNewDetector(sensitive);
	 G4cout << "SENSITIVE DETECTOR ADDED" << G4endl;
       }
  
     
     logicArDet->SetSensitiveDetector(sensitive);
     
     return physiArDet ;
   }

#include "G4RunManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

void DetectorConstruction::UpdateGeometry()
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());


}
