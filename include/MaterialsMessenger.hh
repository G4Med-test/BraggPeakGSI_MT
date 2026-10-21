//MACG 2012-10-18 : created

#ifndef MaterialsMessenger_h
#define MaterialsMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class Materials;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

// ----------------------------------------------------------------------------

class MaterialsMessenger: public G4UImessenger
{

public:

  MaterialsMessenger( Materials* );
  ~MaterialsMessenger();

  void SetNewValue( G4UIcommand*, G4String );

private:

  Materials*                 theMaterials;
  G4UIdirectory*             theMaterialsDir;
  G4UIdirectory*             theWaterDir;

  G4UIcmdWithADoubleAndUnit* theWaterDensityCmd;
  G4UIcmdWithADoubleAndUnit* theWaterIonPotCmd;
};

// ----------------------------------------------------------------------------

#endif
