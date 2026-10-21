////////////////////////////////////////////////////////////////////////////////
// class description:
//
//  This is the class of a filter to be associated with a
// sensitive detector. 
//  This class filters steps by partilce definition.
// The particles are given at constructor or add() method.
//
// Created: 2005-11-14  Tsukasa ASO.
// 2010-07-22 T.Aso Filter for Ions
// 
///////////////////////////////////////////////////////////////////////////////

//MACG 2012-11-02: class based on G4SDParticleFilter
//                 Adding a method to select only primaries.
//MACG 2012-11-06: Method added to select ions by atomic number (Z)


#ifndef SDParticleFilter_h
#define SDParticleFilter_h 1

class G4Step;
class G4ParticleDefinition;
#include "globals.hh"
#include "G4VSDFilter.hh"

#include <vector>


class SDParticleFilter : public G4VSDFilter 
{

public: // with description
  SDParticleFilter(G4String name);
  SDParticleFilter(G4String name, const G4String& particleName);
  SDParticleFilter(G4String name,
		   const std::vector<G4String>&  particleNames);
  SDParticleFilter(G4String name,
		   const std::vector<G4ParticleDefinition*>&  particleDef);
  // Constructors. Filter name and particle's name.
  //

  virtual ~SDParticleFilter();

public: // with description
  virtual G4bool Accept(const G4Step*) const;

  void Add(const G4String& particleName);
  // set method for acceptable particle name.
  //
  void AddIon(G4int Z, G4int A);
  void AddIonByZ(G4int Z);
  void Show();

  void SetOnlyPrimaries(G4bool ); // method to accept only primaries

private:
  std::vector<G4ParticleDefinition*> thePdef;
  std::vector<G4int> theIonZ;
  std::vector<G4int> theIonA;
  std::vector<G4int> theIonOnlyZ;
  G4bool thePrimaries;
};

#endif
