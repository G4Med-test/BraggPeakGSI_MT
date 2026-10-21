
#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4Tokenizer.hh"


PrimaryGeneratorMessenger::
PrimaryGeneratorMessenger(PrimaryGeneratorAction* primaryGen)
:PrimaryAction(primaryGen)
{ 
  //
  // Definition of the interactive commands to modify the parameters of the
  // generation of primary particles
  // 
 beamDir = new G4UIdirectory("/beam/");
 beamDir->SetGuidance("Set parameters of the beam.");

 eneDir = new G4UIdirectory("/beam/energy/");  
 eneDir->SetGuidance("Set kinetic energy parameters of the beam.");  

 posDir = new G4UIdirectory("/beam/position/");  
 posDir->SetGuidance("Position commands of the beam.");  

 angDir = new G4UIdirectory("/beam/momentum/");  
 angDir->SetGuidance("Momentum direction commands of the beam.");

 // =================
 // GENERAL COMMANDS
 // =================

 sourceCmd = new G4UIcmdWithAnInteger("/beam/source", this);
 sourceCmd->SetGuidance("Set the type of radiation source based on a ");
 sourceCmd->SetGuidance("3-digit number:");
 sourceCmd->SetGuidance(" - 1st digit (hundreds): Energy Distribution.");
 sourceCmd->SetGuidance("    0 = mono or gaussian.");
 sourceCmd->SetGuidance(" - 2nd digit (tens): Position Distribution.");
 sourceCmd->SetGuidance("    0 = point or 2D Gaussian distribution.");
 sourceCmd->SetGuidance("    1 = uniform circular/elliptical distribution.");
 sourceCmd->SetGuidance("    2 = plane/rectangular surface.");
 sourceCmd->SetGuidance(" - 3rd digit (units): Angular Distribution.");
 sourceCmd->SetGuidance("    0 = fixed direction or 2D Gaussian aperture.");
 sourceCmd->SetGuidance(" Examples:");
 sourceCmd->SetGuidance(" - Default value = 000");
 sourceCmd->SetParameterName("choice", false);
 sourceCmd->AvailableForStates(G4State_PreInit, G4State_Idle);


 particleCmd = new G4UIcmdWithAString("/beam/particle", this);
 particleCmd -> SetGuidance("Type of primary particle");
 particleCmd -> SetParameterName("choice", false);
 particleCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

 ionCmd = new G4UIcommand("/beam/ion",this);
 ionCmd->SetGuidance("Set an ion as beam particle.");
 ionCmd->SetGuidance("[usage] /gps/ion Z A E");
 ionCmd->SetGuidance("        Z:(int) AtomicNumber");
 ionCmd->SetGuidance("        A:(int) AtomicMass");
 ionCmd->SetGuidance("        E:(double) Excitation energy (in keV)");
  
 G4UIparameter* param;
 param = new G4UIparameter("Z",'i',false);
 param->SetDefaultValue("1");
 ionCmd->SetParameter(param);
 param = new G4UIparameter("A",'i',false);
 param->SetDefaultValue("1");
 ionCmd->SetParameter(param);
 param = new G4UIparameter("E",'d',true);
 param->SetDefaultValue("0.0");
 ionCmd->SetParameter(param);

 //=================
 // ENERGY COMMANDS
 //=================

 meanKineticEnergyCmd =
   new G4UIcmdWithADoubleAndUnit("/beam/energy/mean",this);
 meanKineticEnergyCmd->SetGuidance("Set mean kinetic energy.");
 meanKineticEnergyCmd->SetParameterName("meanEne",false);
 meanKineticEnergyCmd->SetDefaultUnit("MeV");
 meanKineticEnergyCmd->SetUnitCandidates("eV keV MeV GeV TeV");
 meanKineticEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   
 
 sigmaEnergyCmd = 
   new G4UIcmdWithADoubleAndUnit("/beam/energy/sigma",this);
 sigmaEnergyCmd->SetGuidance("Set sigma of the energy gaussian distribution.");
 sigmaEnergyCmd->SetParameterName("sigmaEne",false);
 sigmaEnergyCmd->SetDefaultUnit("keV");
 sigmaEnergyCmd->SetUnitCandidates("eV keV MeV GeV TeV");
 sigmaEnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 //===================
 // POSITION COMMANDS
 //===================

 XposCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/centralX",this);
 XposCmd->SetGuidance("Set central X coordinate of the beam.");
 XposCmd->SetParameterName("position",false);
 XposCmd->SetDefaultUnit("mm");
 XposCmd->SetUnitCandidates("mm cm m");
 XposCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   

 YposCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/centralY",this);
 YposCmd->SetGuidance("Set central Y coordinate of the beam.");
 YposCmd->SetParameterName("position",false);
 YposCmd->SetDefaultUnit("mm");
 YposCmd->SetUnitCandidates("mm cm m");
 YposCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   

 ZposCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/centralZ",this);
 ZposCmd->SetGuidance("Set central Z coordinate of the beam.");
 ZposCmd->SetParameterName("position",false);
 ZposCmd->SetDefaultUnit("mm");
 ZposCmd->SetUnitCandidates("mm cm m");
 ZposCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   

 sigmaRadiusCmd 
   = new G4UIcmdWithADoubleAndUnit("/beam/position/sigma",this);
 sigmaRadiusCmd->SetGuidance("Set radial sigma of a 2D circular Gaussian.");
 sigmaRadiusCmd->SetParameterName("sigmaR",false);
 sigmaRadiusCmd->SetDefaultUnit("mm");
 sigmaRadiusCmd->SetUnitCandidates("mm cm m");
 sigmaRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 sigmaRadiusXCmd 
   = new G4UIcmdWithADoubleAndUnit("/beam/position/sigmaX",this);
 sigmaRadiusXCmd->SetGuidance("Set X sigma of a 2D elliptical Gaussian dist.");
 sigmaRadiusXCmd->SetParameterName("sigmaX",false);
 sigmaRadiusXCmd->SetDefaultUnit("mm");
 sigmaRadiusXCmd->SetUnitCandidates("mm cm m");
 sigmaRadiusXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 sigmaRadiusYCmd 
   = new G4UIcmdWithADoubleAndUnit("/beam/position/sigmaY",this);
 sigmaRadiusYCmd->SetGuidance("Set Y sigma of a 2D elliptical Gaussian dist.");
 sigmaRadiusYCmd->SetParameterName("sigmaY",false);
 sigmaRadiusYCmd->SetDefaultUnit("mm");
 sigmaRadiusYCmd->SetUnitCandidates("mm cm m");
 sigmaRadiusYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 radiusCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/radius",this);
 radiusCmd->SetGuidance("Set radius of a uniform circular beam.");
 radiusCmd->SetParameterName("radius",false);
 radiusCmd->SetDefaultUnit("mm");
 radiusCmd->SetUnitCandidates("mm cm m");
 radiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 radiusXCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/radiusX",this);
 radiusXCmd->SetGuidance("Set X radius of a uniform elliptical beam.");
 radiusXCmd->SetParameterName("radiusX",false);
 radiusXCmd->SetDefaultUnit("mm");
 radiusXCmd->SetUnitCandidates("mm cm m");
 radiusXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 radiusYCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/radiusY",this);
 radiusYCmd->SetGuidance("Set Y radius of a uniform elliptical beam.");
 radiusYCmd->SetParameterName("radiusY",false);
 radiusYCmd->SetDefaultUnit("mm");
 radiusYCmd->SetUnitCandidates("mm cm m");
 radiusYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 sideCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/side",this);
 sideCmd->SetGuidance("Set the side of a uniform square beam");
 sideCmd->SetParameterName("side",false);
 sideCmd->SetDefaultUnit("mm");
 sideCmd->SetUnitCandidates("mm cm m");
 sideCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 deltaXCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/deltaX",this);
 deltaXCmd->SetGuidance("Set the side X of a uniform square beam.");
 deltaXCmd->SetParameterName("deltaX",false);
 deltaXCmd->SetDefaultUnit("mm");
 deltaXCmd->SetUnitCandidates("mm cm m");
 deltaXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 deltaYCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/deltaY",this);
 deltaYCmd->SetGuidance("Set the side Y of a uniform square beam.");
 deltaYCmd->SetParameterName("deltaY",false);
 deltaYCmd->SetDefaultUnit("mm");
 deltaYCmd->SetUnitCandidates("mm cm m");
 deltaYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 //==================
 // ANGULAR COMMANDS
 //==================

 sigmaMomentumXCmd = new G4UIcmdWithADouble("/beam/momentum/sigmaX",this);
 sigmaMomentumXCmd -> SetGuidance("Set sigma momentum X.");
 sigmaMomentumXCmd -> SetParameterName("sigmaX",false);
 sigmaMomentumXCmd -> AvailableForStates(G4State_PreInit,G4State_Idle);   

 sigmaMomentumYCmd = new G4UIcmdWithADouble("/beam/momentum/sigmaY",this);
 sigmaMomentumYCmd -> SetGuidance("Set sigma momentum Y.");
 sigmaMomentumYCmd -> SetParameterName("sigmaY",false);
 sigmaMomentumYCmd -> AvailableForStates(G4State_PreInit,G4State_Idle);   

}


PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  // Directories
  delete beamDir;
  delete eneDir;
  delete posDir;
  delete angDir;
  // General
  delete sourceCmd;
  delete particleCmd;
  delete ionCmd;
  // Energy
  delete meanKineticEnergyCmd;  
  delete sigmaEnergyCmd;
  // Position
  delete XposCmd; 
  delete YposCmd; 
  delete ZposCmd; 
  delete sigmaRadiusCmd;
  delete sigmaRadiusXCmd;
  delete sigmaRadiusYCmd;
  delete radiusCmd;
  delete radiusXCmd;
  delete radiusYCmd;
  delete sideCmd;
  delete deltaXCmd;
  delete deltaYCmd;
  // Angular
  delete sigmaMomentumXCmd; 
  delete sigmaMomentumYCmd; 
}  


void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* cmd,
					    G4String newValue)
{
  //=================
  // GENERAL COMMANDS
  //=================

  if ( cmd == sourceCmd )
    { PrimaryAction->SetSource(sourceCmd->GetNewIntValue(newValue)); }

  else if ( cmd == particleCmd )
    { PrimaryAction->SetParticle(newValue); }

  else if ( cmd == ionCmd )
    { IonCommand(newValue); }

  //=================
  // ENERGY COMMANDS
  //=================

  else if ( cmd == meanKineticEnergyCmd ) {
    PrimaryAction->SetMeanKineticEnergy(meanKineticEnergyCmd->GetNewDoubleValue(newValue));
  }

  else if ( cmd == sigmaEnergyCmd ) {
    PrimaryAction->SetSigmaEnergy(sigmaEnergyCmd->GetNewDoubleValue(newValue));
  }

  //===================
  // POSITION COMMANDS
  //===================

  else if ( cmd == XposCmd )
    { PrimaryAction->SetX0(XposCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == YposCmd )
    { PrimaryAction->SetY0(YposCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == ZposCmd )
    { PrimaryAction->SetZ0(ZposCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == sigmaRadiusCmd ) {
    PrimaryAction->SetSigmaRadius(sigmaRadiusCmd->GetNewDoubleValue(newValue));
  }

  else if ( cmd == sigmaRadiusXCmd ) {
    PrimaryAction->SetSigmaRadiusX(sigmaRadiusXCmd->GetNewDoubleValue(newValue));
  }

  else if ( cmd == sigmaRadiusYCmd ) {
    PrimaryAction->SetSigmaRadiusY(sigmaRadiusYCmd->GetNewDoubleValue(newValue));
  }

  else if ( cmd == radiusCmd )
    { PrimaryAction->SetRadius(radiusCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == radiusXCmd )
    { PrimaryAction->SetRadiusX(radiusXCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == radiusYCmd )
    { PrimaryAction->SetRadiusY(radiusYCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == sideCmd )
    { PrimaryAction->SetSide(sideCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == deltaXCmd )
    { PrimaryAction->SetDX(deltaXCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == deltaYCmd )
    { PrimaryAction->SetDY(deltaYCmd->GetNewDoubleValue(newValue)); }

  //==================
  // ANGULAR COMMANDS
  //==================

  else if ( cmd == sigmaMomentumXCmd )
    { PrimaryAction->SetSigmaMomentumX(sigmaMomentumXCmd->GetNewDoubleValue(newValue)); }

  else if ( cmd == sigmaMomentumYCmd )
    { PrimaryAction->SetSigmaMomentumY(sigmaMomentumYCmd->GetNewDoubleValue(newValue)); }

}



void PrimaryGeneratorMessenger::IonCommand(G4String newValues)
{
  // Read Command
  // -------------

  G4Tokenizer next( newValues );
  // check argument
  G4int Z = StoI(next());
  G4int A = StoI(next());
  G4double eE = 0.0;

  G4String sE = next();
 // if (!(sE.isNull()))
    if (!(sE.empty()))
    eE = StoD(sE) * keV;

  // Call the suitable method
  // -------------------------
  PrimaryAction->SetIon(Z, A, eE);

}
