//---------------------------------------------------------------------
// (Purpose) 
//    Example implementation for multi-functional-detector and 
//   primitive scorer.
//    This Run class has collections which accumulate
//   an event information into a run information.
//
//---------------------------------------------------------------------

#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "G4Event.hh"

#include "G4THitsMap.hh"
#include <vector>


class Run : public G4Run
{
public:
  // constructor and destructor.
  Run();
  virtual ~Run();

public:
  // virtual method from G4Run. 
  // The method is overriden in this class for scoring.
  virtual void RecordEvent(const G4Event*);

  //New method for the MT migration 13/07/2016
  virtual void Merge(const G4Run*);


  // Access methods for scoring information.
  // - Number of HitsMap for this RUN. 
  //   This is equal to number of collections.
  G4int GetNumberOfHitsMap() const {return theRunMap.size();}

  // - Get HitsMap of this RUN.
  //   by sequential number, by multifucntional name and collection name,
  //   and by collection name with full path.
  G4THitsMap<G4double>* GetHitsMap(G4int i) const {return theRunMap[i];}
  G4THitsMap<G4double>* GetHitsMap(const G4String& detName, 
				   const G4String& colName) const;
  G4THitsMap<G4double>* GetHitsMap(const G4String& fullName) const;

  // - Get HitsMap of this RUN.
  //   by sequential number, by multifucntional name and collection name,
  //   and by collection name with full path.
  G4THitsMap<G4double>* GetHitsErrorMap(G4int i) const
  {return theRunErrorMap[i];}

  G4THitsMap<G4double>* GetHitsErrorMap(const G4String& detName, 
					const G4String& colName) const;
  G4THitsMap<G4double>* GetHitsErrorMap(const G4String& fullName) const;

  // - Dump All HitsMap of this RUN.
  //   This method calls G4THisMap::PrintAll() for individual HitsMap.
  //void DumpAllScorer() const;

private:
  G4THitsMap<G4double>* GetEvtErrorMap(G4THitsMap<G4double>*) const;

private:
  std::vector<G4String> theCollName;
  std::vector<G4int> theCollID;
  std::vector<G4THitsMap<G4double>*> theRunMap;
  std::vector<G4THitsMap<G4double>*> theRunErrorMap;  // to score errors
};


#endif
