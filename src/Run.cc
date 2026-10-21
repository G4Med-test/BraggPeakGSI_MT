//=====================================================================
//
//  (Description)
//    Run Class is for accumulating scored quantities which are
//  scored using G4MutiFunctionalDetector and G4VPrimitiveScorer.
//  Accumulation is done using G4THitsMap objects.
//
//    The constructor Run(const std::vector<G4String>* mfdName)
//  needs a vector filled with MultiFunctionalDetector names which
//  was assigned at instantiation of MultiFunctionalDetector(MFD).
//  Then Run constructor automatically scans primitive scorers
//  in the MFD, and obtains collectionIDs of all collections associated
//  to those primitive scorers. Futhermore, the G4THitsMap objects 
//  for accumulating during a RUN are automatically created too.
//  (*) The Collection Name is same as primitive scorer name.
// 
//    The resultant information is kept inside Run objects as
//  data members.
//  std::vector<G4String> theCollName;            // Collection Name,
//  std::vector<G4int> theCollID;                 // Collection ID,
//  std::vector<G4THitsMap<G4double>*> theRunMap; // HitsMap for RUN.
//
//  The resultant HitsMap objects are obtained using access method,
//  GetHitsMap(..).
//
//=====================================================================


#include "Run.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4MTRunManager.hh"
#include "G4SDManager.hh"
#include "G4VPrimitiveScorer.hh"

#include <map>


//==============================================================================
//
// Constructor. 
Run::Run()
:G4Run()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  /*G4HCtable* */ HCtable = SDman->GetHCtable();
  G4int HCentries = HCtable->entries();   // it gives the number of HC's.

  G4cout << "Registration of G4THitsMap for accumulative magnitudes during run."
	 << G4endl;

  for (G4int ii = 0; ii < HCentries; ii++)  {
    G4String sdName = HCtable->GetSDname(ii);
    G4String hcName = HCtable->GetHCname(ii);

    // The collection name is given by <MFD name>/<Primitive Scorer name>.
    G4String collectionFullName = sdName + "/" + hcName;
    G4int collectionID = SDman->GetCollectionID(collectionFullName);

    if ( collectionID >= 0 )  {
      // Store obtained HitsCollection information into data members.
      // And, create new G4THitsMap for accumulating quantities during RUN.
      theCollName.push_back(collectionFullName);
      theCollID.push_back(collectionID);
      theRunMap.push_back(new G4THitsMap<G4double>(sdName,hcName));
      // And create new G4THitsMap for accumulating errors during RUN.
      theRunErrorMap.push_back(new G4THitsMap<G4double>(sdName, hcName));
      G4cout << "++ "<< collectionFullName << " id " << collectionID
	     << " , in theRunMap[" << theRunMap.size()-1 << "]." << G4endl;
    }
    else {
      G4cout << "** collection " << collectionFullName << " not found. "
	     << G4endl;
    }
  }
}


//==============================================================================
// Destructor
//    clear all data members.
Run::~Run()
{
  //--- Clear HitsMap for RUN
  G4int Nmap = theRunMap.size();
  for ( G4int i = 0; i < Nmap; i++)  {
    if ( theRunMap[i] ) theRunMap[i]->clear();
  }

  //--- Clear HitsMap for accumulated ERRORS
  for (G4int i = 0; i < Nmap; i++)  {
    if ( theRunErrorMap[i] ) theRunErrorMap[i]->clear();
  }

  theCollName.clear();
  theCollID.clear();
  theRunMap.clear();
  theRunErrorMap.clear();
}



//==============================================================================
//  RecordEvent() is a method called at the end of each event.
//  For scoring purpose, the resultant quantity in a event,
//  is accumulated during a Run.
void Run::RecordEvent(const G4Event* aEvent)
{
  numberOfEvent++;  // This line is mandatory (reimplements base class method).

  //------------------------------
  // HitsCollection of This Event
  //------------------------------
  G4HCofThisEvent* HCE = aEvent->GetHCofThisEvent();
  if (!HCE) return;

  //--------------------------------------------------------
  // Sum up HitsMap of this Event into HitsMap of this RUN
  //--------------------------------------------------------

  // Loop over HitsCollection
  G4int Ncol = theCollID.size();  
  for ( G4int i = 0; i < Ncol ; i++ )  {
    G4THitsMap<G4double>* EvtMap = 0;
    
    if ( theCollID[i] >= 0 ) {
      // Collection is attached to HCE
      EvtMap = (G4THitsMap<G4double>*)(HCE->GetHC(theCollID[i]));
    }
    else
      G4cout <<" ERROR: EvtMap Not Found "<< i << G4endl;
    
    if ( EvtMap )  {
      //--- Sum up HitsMap of this event to HitsMap of RUN.
      *theRunMap[i] += *EvtMap;

      //--- Create the EvtErrorMap to score squares.
      G4THitsMap<G4double>* EvtErrorMap = GetEvtErrorMap( EvtMap );
      // And sum up to HitsErrorMap of RUN
      *theRunErrorMap[i] += *EvtErrorMap;
    }
  }
}


//==============================================================================
// Get the squares of all the quantities stored
// in the EvtMap in order to keep information
// about the uncertainties during all the run.
G4THitsMap<G4double>* 
Run::GetEvtErrorMap( G4THitsMap<G4double>* evtHitsMap ) const
{
  G4THitsMap<G4double>* evtErrorMap = evtHitsMap;
  std::map<G4int, G4double*>* evtMap = evtHitsMap->GetMap();

  std::map<G4int, G4double*>::iterator it;
  for(it = evtMap->begin(); it != evtMap->end(); it++)  {
    G4int key = it->first;
    G4double evtX2 = (*(it->second))*(*(it->second));
    evtErrorMap->set(key, evtX2);
  }

  return evtErrorMap;
}



//==============================================================================
//  Access methods for HitsMap of the RUN
//
//-----
// Access HitsMap.
//  By  MultiFunctionalDetector name and Collection Name.
G4THitsMap<G4double>* Run::GetHitsMap(const G4String& detName,
				      const G4String& colName) const
{
  G4String fullName = detName+"/"+colName;
  return GetHitsMap(fullName);
}

//-----
// Access HitsMap.
//  By full description of collection name, that is
//    <MultiFunctional Detector Name>/<Primitive Scorer Name>
G4THitsMap<G4double>* Run::GetHitsMap(const G4String& fullName) const
{
  G4int Ncol = theCollName.size();
  for ( G4int i = 0; i < Ncol; i++)  {
    if ( theCollName[i] == fullName )
      return theRunMap[i];
  }

  return NULL;
}


//==================================================================
//  Access methods for HitsMap of the ACCUMULATED ERRORS DURING RUN
//
//-----
// Access HitsMap.
//  By  MultiFunctionalDetector name and Collection Name.
G4THitsMap<G4double>* Run::GetHitsErrorMap(const G4String& detName,
					   const G4String& colName) const
{
  G4String fullName = detName+"/"+colName;
  return GetHitsErrorMap(fullName);
}

//-----
// Access HitsMap.
//  By full description of collection name, that is
//    <MultiFunctional Detector Name>/<Primitive Scorer Name>
G4THitsMap<G4double>* Run::GetHitsErrorMap(const G4String& fullName) const
{
  G4int Ncol = theCollName.size();
  for ( G4int i = 0; i < Ncol; i++) {
    if ( theCollName[i] == fullName )
      return theRunErrorMap[i];
  }

  return NULL;
}


//============================================================================
// Method to be overwritten by the user for merging local Run object to the
// global Run object.
void Run::Merge(const G4Run* aRun) 
{
  // MACG 2018-10-15: Merging is being done from output files!
  // This method will be used in the future (following standards!!)

  // const Run* localRun = reinterpret_cast<const Run*>(aRun);

  // // - Number of HitsMap in THIS RUN.
  // G4int nHitMaps = localRun->GetNumberOfHitsMap();
  // // - Number of Events to calculate relative error
  // G4int nEvt = localRun->GetNumberOfEvent();

  // G4cout << "nHitMaps = " << nHitMaps << G4endl;
  // G4cout << "nEvt = " << nEvt << G4endl;

  // // - GetHitsMap and dump values.
  // for ( G4int ii = 0; ii < nHitMaps ; ii++ )  {
  //   G4THitsMap<G4double>* RunMap = localRun->GetHitsMap(ii);
  //   G4THitsMap<G4double>* errorMap = localRun->GetHitsErrorMap(ii);
  //   if ( RunMap )  {
  //     G4cout << " PrimitiveScorer RUN " 
  // 	     << RunMap->GetSDname() <<","<< RunMap->GetName() << G4endl;
  //     G4cout << " Number of entries " << RunMap->entries() << G4endl;

  //     std::map<G4int,G4double*>::iterator itr;
  //     for(itr = RunMap->GetMap()->begin();
  // 	  itr != RunMap->GetMap()->end(); itr++)  {
  // 	G4int key = itr->first;
  // 	G4double sumW = *(itr->second);
  // 	G4double sumW2 = *((*errorMap)[key]);
  // 	G4double absError = std::sqrt((sumW2*nEvt-sumW*sumW)/(nEvt-1))/nEvt;
	  
  // 	G4cout << "  copy no.: " << key
  // 	       << "  Run Value : " << sumW/nEvt
  // 	       << "  Rel. Error Value: " << absError/(sumW/nEvt)
  // 	       << G4endl;
  //     }
  //   }
  // }

  G4Run::Merge(aRun);

}
