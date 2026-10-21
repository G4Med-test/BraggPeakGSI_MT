#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include <map>
#include <vector>


class RunHitsGlobalPrinter;

class G4Run;


class RunAction : public G4UserRunAction
{
public:
RunAction();
~RunAction();

public:
// virtual methods from G4UserRunAction

G4Run* GenerateRun();
// A derived G4Run is needed to score quantities during the run

void BeginOfRunAction(const G4Run*);
void EndOfRunAction(const G4Run* );

//Methods for the global output file
void MergeGlobalFile();
void AddMaps(std::vector< std::map<G4int, G4double>* >& newMaps,
	     std::vector< std::map<G4int, G4double>* >& appendMaps);

//Method for the conversion of the outputfile data in terms 
//of energy depostion
void GetRunMap_energy(G4String fileName);


private:  

  // Data members
RunHitsGlobalPrinter* theRunHitsGlobalPrinter;
std::vector<G4String> theFileNames;

};
#endif
