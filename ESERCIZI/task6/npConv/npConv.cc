//S/ $Id: task2.cc 26 2010-01-06 08:09:34Z schaelic $
/**
 * @file
 * @brief Main program.
 */

//Main program in order to study the gamma production from the beam dump
//from fast (En = 2.5 MeV) neutrons

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4Version.hh"

#include "G4VisExecutive.hh"
#if  G4VERSION_NUMBER>=930
#include "G4UIExecutive.hh"
#else
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#endif

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
//#include "CopperPhysicsList.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh" /////Physics Lits for low energy neutrons
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "EventAction.hh"
#include "RunAction.hh"
#include "QGSP_BIC_HP.hh" /////Physics Lits for low energy neutrons


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


/*!
\brief Main program

\callgraph

*/
int main(int argc,char** argv)
{
  //choose the Random engine
  //using the same as TRandom3 di root
  // it has a period of 2^19937-1
  CLHEP::HepRandom::setTheEngine(new CLHEP::MTwistEngine());

  // Run manager
  G4RunManager * runManager = new G4RunManager();

  // mandatory Initialization classes 
  G4VUserDetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Local user Physics List
  //G4VUserPhysicsList* physics = new PhysicsList();

  // Reference Physics List from Geant4 kernel 
  //G4VUserPhysicsList* physics = new QGSP_BERT();

  G4cout << "Setting Physics List" << G4endl;
  G4VUserPhysicsList* physics = new QGSP_BERT_HP();  //Declare a physics list using QGSP_BERT_HP 
                                                   //HP is for low energy neutrons and uses G4NDL cross sections

  //G4VUserPhysicsList* physics = new QGSP_BIC_HP();  //Declare a physics list using QGSP_BERT_HP 

  //G4VUserPhysicsList* physics = new CopperPhysicsList();

  // mandatory User class  
  runManager->SetUserInitialization(physics);
   
  // mandatory User class
  G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction();
  runManager->SetUserAction(gen_action);


  //Optional User Action classes

  EventAction* event_action = new EventAction;
  RunAction* run_action = new RunAction(event_action);
  runManager->SetUserAction( event_action );
  runManager->SetUserAction( run_action );

  // Initialize G4 kernel
  runManager->Initialize();
      

  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
     
  // Get the pointer to the User Interface manager
  //
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  

  if (argc!=1) {  // batch mode  
    
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
  }
  else {           // interactive mode : define UI session
     
#if  G4VERSION_NUMBER>=930
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
    if (ui->IsGUI()) 
      UImanager->ApplyCommand("/control/execute visQt.mac");
    else 
      UImanager->ApplyCommand("/control/execute vis.mac");     
#else
  #ifdef G4UI_USE_TCSH
    G4UIsession * ui = new G4UIterminal(new G4UItcsh);      
  #else
    G4UIsession * ui = new G4UIterminal();
  #endif
    UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
    ui->SessionStart();
    delete ui;     
  }

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete runManager;

  return 0;
}
