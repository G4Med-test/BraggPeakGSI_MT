
#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcommand;

class PrimaryGeneratorMessenger: public G4UImessenger
{
public:
  PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
  ~PrimaryGeneratorMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);

private:
  void IonCommand(G4String);

 private:
  PrimaryGeneratorAction* PrimaryAction;
  // Directories
  G4UIdirectory* beamDir;
  G4UIdirectory* eneDir;
  G4UIdirectory* posDir;
  G4UIdirectory* angDir;
  // General commands
  G4UIcmdWithAnInteger* sourceCmd;
  G4UIcmdWithAString*   particleCmd;
  G4UIcommand*          ionCmd;
  // Energy commands
  G4UIcmdWithADoubleAndUnit* meanKineticEnergyCmd;
  G4UIcmdWithADoubleAndUnit* sigmaEnergyCmd;
  // Position commands
  G4UIcmdWithADoubleAndUnit* XposCmd;
  G4UIcmdWithADoubleAndUnit* YposCmd;
  G4UIcmdWithADoubleAndUnit* ZposCmd;
  G4UIcmdWithADoubleAndUnit* sigmaRadiusCmd;
  G4UIcmdWithADoubleAndUnit* sigmaRadiusXCmd;
  G4UIcmdWithADoubleAndUnit* sigmaRadiusYCmd;
  G4UIcmdWithADoubleAndUnit* radiusCmd;
  G4UIcmdWithADoubleAndUnit* radiusXCmd;
  G4UIcmdWithADoubleAndUnit* radiusYCmd;
  G4UIcmdWithADoubleAndUnit* sideCmd;
  G4UIcmdWithADoubleAndUnit* deltaXCmd;
  G4UIcmdWithADoubleAndUnit* deltaYCmd;
  // Angular commands
  G4UIcmdWithADouble*        sigmaMomentumXCmd;
  G4UIcmdWithADouble*        sigmaMomentumYCmd;
};

#endif
