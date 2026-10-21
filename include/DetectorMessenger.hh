//MACG 2012-07-25: my template class
//MACG 2012-10-19: Geometry option command added
//                  Cell commands added.
//MACG 2012-10-29: Command to register ROIs added.
//MACG 2012-14-12: Removed cellFirstZpos and nCells commands.
//MACG 2013-06-10: Adding commands to define the detailed scoring region.
//MACG 2013-06-11: Command added to set the spacing of cells out of detailed
//                  scoring region.


#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

// ----------------------------------------------------------------------------

class DetectorMessenger: public G4UImessenger
{

public:

  DetectorMessenger( DetectorConstruction* );
  ~DetectorMessenger();

  void SetNewValue( G4UIcommand*, G4String );

private:

  DetectorConstruction*      theDet;

  G4UIdirectory*             detectorDir;
  G4UIcmdWithAString*        sensitiveLVsCmd;
  G4UIcmdWithAString*        roiCmd;
  G4UIcmdWithAnInteger*      geomOptionCmd;

  G4UIcmdWithADoubleAndUnit* waterTankXYCmd;
  G4UIcmdWithADoubleAndUnit* waterTankZCmd;
  G4UIdirectory*             cellDir;
  G4UIcmdWithADoubleAndUnit* cellRadiusCmd;
  G4UIcmdWithADoubleAndUnit* cellHeightCmd;
  G4UIcmdWithADoubleAndUnit* detailLowZCmd;
  G4UIcmdWithADoubleAndUnit* detailHighZCmd;
  G4UIcmdWithAnInteger*      cellSpacingCmd;

};

// ----------------------------------------------------------------------------

#endif
