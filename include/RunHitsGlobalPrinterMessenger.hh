//MACG 2018-09-16: Created.


#ifndef RunHitsGlobalPrinterMessenger_h
#define RunHitsGlobalPrinterMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class RunHitsGlobalPrinter;
class G4UIdirectory;
class G4UIcmdWithAString;

// ----------------------------------------------------------------------------

class RunHitsGlobalPrinterMessenger: public G4UImessenger
{

public:

  RunHitsGlobalPrinterMessenger( RunHitsGlobalPrinter* );
  ~RunHitsGlobalPrinterMessenger();

  void SetNewValue( G4UIcommand*, G4String );

private:

  RunHitsGlobalPrinter*      theOutput;
  G4UIdirectory*             theOutputDir;

  G4UIcmdWithAString*        theFileNameCmd;
};

// ----------------------------------------------------------------------------

#endif
