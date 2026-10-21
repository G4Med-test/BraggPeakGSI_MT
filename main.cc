
#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"

#include "G4RunManagerFactory.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }
     
  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager. Pick the proper run 
  // manager depending if the multi-threading option is 
  // active or not.
  //
  auto* runManager = G4RunManagerFactory::CreateRunManager("Serial");


  // Set mandatory initialization classes
  //
  runManager->SetUserInitialization(new DetectorConstruction);
  //
  runManager->SetUserInitialization(new PhysicsList);
    
  // Set user action initialization
  //
  runManager->SetUserInitialization(new ActionInitialization());  

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else { 
    // interactive mode
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  delete visManager;
  delete runManager;

}
