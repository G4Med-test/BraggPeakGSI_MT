//MACG 2018-09-01: BraggPeakGSI_MT messenger class for regression tests.


#ifndef PhysicsListMessenger_h
#define PhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class PhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;


//-------1---------2---------3---------4---------5---------6---------7---------8

class PhysicsListMessenger: public G4UImessenger
{
  public:
  
    PhysicsListMessenger(PhysicsList* );
   ~PhysicsListMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
  
  PhysicsList* thePL;
    
  G4UIdirectory*             physDir;
  G4UIcmdWithADoubleAndUnit* gammaCutCmd;
  G4UIcmdWithADoubleAndUnit* electCutCmd;
  G4UIcmdWithADoubleAndUnit* positCutCmd;    
  G4UIcmdWithADoubleAndUnit* gammaCutROICmd;
  G4UIcmdWithADoubleAndUnit* electCutROICmd;
  G4UIcmdWithADoubleAndUnit* positCutROICmd;
  G4UIcmdWithAString*        physicsListCmd;
};

#endif

