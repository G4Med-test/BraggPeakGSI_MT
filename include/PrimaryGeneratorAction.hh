//MACG 2018-09-05 : PrimaryGeneratorAction class for regression tests.
//                 This class assumes that the beam propagates along +Z axis.


#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include <string>
#include <iostream>

class G4Event;
class G4GeneralParticleSource;
//class G4ParticleGun;

class PrimaryGeneratorMessenger;


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();    
  ~PrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  
private:
  void SetDefaultParameters();
  void SetDefaultPrimaryParticle();
  void SetEneDist(G4int eneID);
  void SetPosDist(G4int posID);
  void SetAngDist(G4int angID);

public:

  // =================
  // MODIFIER METHODS
  // =================

  // Method to set the type of beam used
  void SetSource(G4int source);

  // Methods to change the parameters of the default radiation source
  // interactively
  void SetParticle(G4String);
  void SetIon(G4int, G4int, G4double );  // only nucleus properties

  inline void SetMeanKineticEnergy(G4double val) { meanKineticEnergy = val; }
  inline void SetSigmaEnergy(G4double val) { sigmaEnergy = val; }

  inline void SetX0(G4double val) { X0 = val; }
  inline void SetY0(G4double val) { Y0 = val; }
  inline void SetZ0(G4double val) { Z0 = val; }
  inline void SetSigmaRadius(G4double val) { sigmaRadiusX = sigmaRadiusY = val;}
  inline void SetSigmaRadiusX(G4double val) { sigmaRadiusX = val; }
  inline void SetSigmaRadiusY(G4double val) { sigmaRadiusY = val; }
  inline void SetRadius(G4double val) { radiusX = radiusY = val; }
  inline void SetRadiusX(G4double val) { radiusX = val; }
  inline void SetRadiusY(G4double val) { radiusY = val; }
  inline void SetSide(G4double side) { dX = dY = side; }
  inline void SetDX(G4double val) { dX = val; }
  inline void SetDY(G4double val) { dY = val; }

  inline void SetSigmaMomentumX(G4double val) { sigmaMomentumX = val; }
  inline void SetSigmaMomentumY(G4double val) { sigmaMomentumY = val; }

  // ===============
  // ACCESS METHODS
  // ===============

  inline G4String GetSourceID() const     {return std::to_string(theSourceID);}


private:

  // 3-digit integer to choose the radiation source. The values are:
  // - 1st digit (hundreds): Energy Distribution.
  //    0 = mono or gaussian.
  // - 2nd digit (tens): Position Distribution.
  //    0 = point or 2D Gaussian distribution.
  //    1 = uniform circular/elliptical distribution.
  //    2 = plane/rectangular surface.
  // - 3rd digit (units): Angular Distribution.
  //    0 = fixed momentum direction or 2D Gaussian random aperture.
  // Examples:
  // - Default value = 000;


  G4int theSourceID;
  G4int theEneID;   // These are obtained from theSourceID
  G4int thePosID;   // automatically.
  G4int theAngID;   //

  // General particle source
  G4GeneralParticleSource* theGPS;
  //  G4ParticleGun* particleGun;



  // Data members for each kind of distribution
  //
  // Energy
  G4double meanKineticEnergy;
  G4double sigmaEnergy;
  // Position
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double sigmaRadiusX;
  G4double sigmaRadiusY;
  G4double radiusX;
  G4double radiusY;
  G4double dX;
  G4double dY;
  // Direction ( default is (0., 0., 1.) )
  G4double sigmaMomentumX;
  G4double sigmaMomentumY;

  // Messenger class
  PrimaryGeneratorMessenger* theMessenger;

};

#endif
