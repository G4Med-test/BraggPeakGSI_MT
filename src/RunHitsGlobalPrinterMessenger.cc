//MACG 2018-09-16: Created.


#include "globals.hh"

#include "RunHitsGlobalPrinterMessenger.hh"
#include "RunHitsGlobalPrinter.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"


//-------1---------2---------3---------4---------5---------6---------7---------8
// ----------------------------------------------------------------------------

RunHitsGlobalPrinterMessenger::
RunHitsGlobalPrinterMessenger( RunHitsGlobalPrinter* myOutput )
  : theOutput( myOutput )
{ 
  theOutputDir = new G4UIdirectory( "/output/" );
  theOutputDir->SetGuidance("Control of output printer.");

  theFileNameCmd = new G4UIcmdWithAString("/output/runHitsFileName", this);
  theFileNameCmd->SetGuidance("Common name of the files containing run hits.");
  theFileNameCmd->SetParameterName("commonName", false);
  theFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

// ----------------------------------------------------------------------------

RunHitsGlobalPrinterMessenger::~RunHitsGlobalPrinterMessenger()
{
  delete theOutputDir;
  delete theFileNameCmd;
}

// ----------------------------------------------------------------------------

void RunHitsGlobalPrinterMessenger::SetNewValue(G4UIcommand* command,
						G4String newValue)
{ 
  if (command == theFileNameCmd)
    { theOutput->SetFileName(newValue); }
}
