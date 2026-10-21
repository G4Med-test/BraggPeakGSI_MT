//MACG 2012-11-02: class based on G4SDParticleFilter
//                 Adding a method to select only primaries.
//MACG 2012-11-06: Method added to select ions by atomic number (Z)


#include "SDParticleFilter.hh"
#include "G4Step.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

////////////////////////////////////////////////////////////////////////////////
// class description:
//
//  This is the class of a filter to be associated with a
// sensitive detector. 
//  This class filters steps by partilce definition.
//
// Created: 2005-11-14  Tsukasa ASO.
// 
///////////////////////////////////////////////////////////////////////////////


SDParticleFilter::SDParticleFilter(G4String name)
  :G4VSDFilter(name)
{
  thePdef.clear();
  theIonZ.clear();
  theIonA.clear();
  theIonOnlyZ.clear();
  thePrimaries = false;
}


SDParticleFilter::SDParticleFilter(G4String name,
				   const G4String& particleName)
  :G4VSDFilter(name)
{
  thePdef.clear();
  theIonZ.clear();
  theIonA.clear();
  theIonOnlyZ.clear();
  thePrimaries = false;

  G4ParticleDefinition* pd = 
    G4ParticleTable::GetParticleTable()->FindParticle(particleName);

  if(!pd)
    {
      G4String msg = "Particle <";
      msg += particleName;
      msg += "> not found.";
      G4Exception("SDParticleFilter::SDParticleFilter",
		  "DetPS0101",FatalException,msg);
    }

  thePdef.push_back(pd);
}


SDParticleFilter::SDParticleFilter(G4String name, 
				   const std::vector<G4String>& particleNames)
  :G4VSDFilter(name)
{
  thePdef.clear();
  theIonZ.clear();
  theIonA.clear();
  theIonOnlyZ.clear();
  thePrimaries = false;

  for ( size_t i = 0; i < particleNames.size(); i++)
    {
      G4ParticleDefinition* pd = 
	G4ParticleTable::GetParticleTable()->FindParticle(particleNames[i]);

      if(!pd)
	{
	  G4String msg = "Particle <";
	  msg += particleNames[i];
	  msg += "> not found.";
	  G4Exception("SDParticleFilter::SDParticleFilter",
		      "DetPS0102",FatalException,msg);
	}
      thePdef.push_back(pd);
    }
}


SDParticleFilter::SDParticleFilter(G4String name, 
			  const std::vector<G4ParticleDefinition*>& particleDef)
  :G4VSDFilter(name), thePdef(particleDef)
{
  theIonZ.clear();
  theIonA.clear();
  theIonOnlyZ.clear();
  thePrimaries = false;

  for ( size_t i = 0; i < particleDef.size(); i++) {
    if(!particleDef[i]) 
      G4Exception("SDParticleFilter::SDParticleFilter",
		  "DetPS0103",FatalException,
		  "NULL pointer is found in the given particleDef vector.");
  }

}


SDParticleFilter::~SDParticleFilter()
{ 
  thePdef.clear();
  theIonZ.clear();
  theIonA.clear();
  theIonOnlyZ.clear();
}


G4bool SDParticleFilter::Accept(const G4Step* aStep) const
{
  const G4Track* aTrack = aStep->GetTrack();

  if (thePrimaries) {
    if (aTrack->GetParentID() == 0) return TRUE;
  }
 
  for ( size_t i = 0; i < thePdef.size(); i++) {
    if ( thePdef[i] == aTrack->GetDefinition() ) return TRUE;
  }

  // Ions by Z,A
  for ( size_t i = 0; i < theIonZ.size(); i++) {
    if ( theIonZ[i] == aTrack->GetDefinition()->GetAtomicNumber() 
	 && theIonA[i] == aTrack->GetDefinition()->GetAtomicMass() )
      {
	return TRUE;
      }
  }

  // Ions only by Z
  for ( size_t i = 0; i < theIonOnlyZ.size(); i++) {
    if ( theIonOnlyZ[i] == aTrack->GetDefinition()->GetAtomicNumber() )
      {
	return TRUE;
      }
  }

  return FALSE;
}


void SDParticleFilter::Add(const G4String& particleName)
{
  G4ParticleDefinition* pd = 
    G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  if (!pd)
    {
      G4String msg = "Particle <";
      msg += particleName;
      msg += "> not found.";
      G4Exception("SDParticleFilter::add()",
		  "DetPS0104",FatalException,msg);
    }
  for ( size_t i = 0; i < thePdef.size(); i++){
    if ( thePdef[i] == pd ) return;
  }
  thePdef.push_back(pd);
}


void SDParticleFilter::AddIon(G4int Z, G4int A)
{
  for ( size_t i = 0; i < theIonZ.size(); i++) {
    if ( theIonZ[i] == Z && theIonA[i] == A ) {
      G4cout << "SDParticleFilter:: Ion has been already registered."<<G4endl;
      return;
    }
  }
  theIonZ.push_back(Z);
  theIonA.push_back(A);
}


void SDParticleFilter::AddIonByZ(G4int Z)
{
  for ( size_t i = 0; i < theIonOnlyZ.size(); i++) {
    if ( theIonOnlyZ[i] == Z ) {
      G4cout << "SDParticleFilter::AddIonByZ(" << Z << ")" << G4endl;
      G4cout << "SDParticleFilter:: Ion has been already registered." << G4endl;
      return;
    }
  }

  theIonOnlyZ.push_back(Z);
}


void SDParticleFilter::SetOnlyPrimaries(G4bool flag)
{
  // This method resets the acceptance criterium so that if:
  // True: Only primary particles (i.e. those with parentID == 0) are filtered
  // False: the particles filtered can be primaries or not.

  if (flag) {
    thePdef.clear();
    theIonZ.clear();
    theIonA.clear();
  }

  thePrimaries = flag;
}


void SDParticleFilter::Show()
{
  G4cout << "----G4SDParticleFileter particle list------"<<G4endl;
  for ( size_t i = 0; i < thePdef.size(); i++){
    G4cout << thePdef[i]->GetParticleName() << G4endl;
  }
  for ( size_t i = 0; i < theIonZ.size(); i++){
    G4cout << " Ion PrtclDef (" << theIonZ[i]<<","<<theIonA[i]<<")"
	   << G4endl;
  }
  G4cout << "-------------------------------------------"<<G4endl;
}
