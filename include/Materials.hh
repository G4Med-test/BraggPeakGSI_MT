//MACG 2012-10-02 : Materials class

#ifndef Materials_H
#define Materials_H 1

#include "globals.hh"

class G4Material;

class MaterialsMessenger;


class Materials
{ 
public:
  Materials();
  ~Materials();

public:
  void  DefineMaterials();
  G4Material* GetMat(G4String); // Returns the material

  void SetWaterDensity(G4double dens)      {theWaterDensity = dens;}
  void SetWaterIonPotential(G4double ipot) {theWaterIonPotential = ipot;}
  G4double GetWaterDensity()      {return theWaterDensity;}
  G4double GetWaterIonPotential() {return theWaterIonPotential;}


private:
  MaterialsMessenger* theMessenger;
  G4double theWaterDensity;
  G4double theWaterIonPotential;

};
#endif
