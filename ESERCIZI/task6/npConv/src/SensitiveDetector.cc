// $Id: SensitiveDetector.cc 100 2010-01-26 16:12:59Z adotti $
/**
 * @file   SensitiveDetector.cc
 * @brief  Implements sensitive part of simulation.
 *
 * @date   10 Dec 2009
 * @author Andreas Schaelicke
 */

#include "SensitiveDetector.hh"

#include "G4Step.hh"
#include "Randomize.hh"

#include "G4HCofThisEvent.hh"

#include "G4HCtable.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

#include "G4TouchableHistory.hh"

#include "TrackParticle.hh" 
#include "TrackParentParticle.hh"

//#include "TrackingAction.hh"
//#include "TrackInformation.hh"



SensitiveDetector::SensitiveDetector(G4String SDname)
  : G4VSensitiveDetector(SDname)
{
  G4cout << "Creating SD with name: " << SDname << G4endl;
  // 'collectionName' is a protected data member of base class G4VSensitiveDetector.
  // Here we declare the name of the collection we will be using.
  G4String myCollectionName = "GasHitCollection";
 
  // Note that we may add as many collection names we would wish: ie
  // a sensitive detector can have many collections.
  
  // Add myCollectionName to the vector of names called collectionName variable
  // Use insert method of std::vector<>
  
  collectionName.insert(myCollectionName);

  


}

SensitiveDetector::~SensitiveDetector()
{}

G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *)
{
  
  
  //This method is called every time a G4Step is performed in the logical volume
  //to which this SD is attached: the HDGe Det!!!!

  //In this case using the Touchable is not needed since no copy number has
  //to be retrievied

  //The copynumber of the HDGe is the only one existing

  //G4cout << "<<<<<<<<<<<<<<<<<<<<<<PROCESSING HITS>>>>>>>>>>>>>>>>>>>>>>" << G4endl;

  //I need the layerindex (in this case the gas volume index) in order to say that an hit collection 
  //has already been declared for this
  G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
  G4int copyNo = touchable->GetVolume(0)->GetCopyNo();
  G4int HDGeindex = copyNo; //It is defined as 1
 
  TrackParticle *Tparticle = new TrackParticle();


  G4Track* thistrack   = step->GetTrack();
  
  
  
  /////TrackInformation* info = (TrackInformation*)( thistrack->GetUserInformation() );
  /////
  /////G4cout << "<<<<<<<<<<<<<<<<<<<<<<TRACKINFORMATION DECLARED>>>>>>>>>>>>>>>>>>>>>>" << G4endl;
  /////
  /////G4double parentPos = (info->GetOriginalPosition()).getZ();
  /////
  /////G4cout << "<<<<<<<<<<<<<<<<<<<<<<AFTER GETORIGINALZPOSITION>>>>>>>>>>>>>>>>>>>>>>" << G4endl;


  G4double trackenergy = thistrack->GetDynamicParticle()->GetKineticEnergy();

    //Define the key for the trackMap_t
  G4int    thistrackID = thistrack->GetTrackID();
  
  G4String trackname   = thistrack->GetDefinition()->GetParticleName();
  
  G4int    parentID    = thistrack->GetParentID();
  
  //G4cout << " Track " << thistrackID << " Total Energy  = " 
  //  << G4BestUnit(trackenergy,"Energy") << " Type " << trackname << " Mum " 
  //  << parentID << G4endl;
  
  //G4cout << "\n" << G4endl;

  G4int trackPDGencod = thistrack->GetDefinition()->GetPDGEncoding();

  G4double zstartpos = ( thistrack->GetVertexPosition() ).getZ();

  G4double starttime = thistrack->GetGlobalTime();

  //G4cout << "This track time is: " << G4BestUnit(starttime,"Time") << G4endl;
  
  //G4ThreeVector stoppos = step->GetPostStepPoint()->GetPosition();


  //****************************************************************************//
  //Filling the members of TrackParticle   
  
  //Mother Track ID
  Tparticle->SetMothPart_ID(parentID);

  // energy deposit in this step 
  G4double edep = step->GetTotalEnergyDeposit();

  //G4cout << " Total Step Energy Deposit Edep = " << G4BestUnit(edep,"Energy") << G4endl;
  
  //Total EnergyDeposited
  //Tparticle->TrackAddEDep(edep);
  
  //Particle Type via PDG_Encoding

  Tparticle->SetPart_Type(trackPDGencod);
  
  //Set Particle Name (string)
  
  Tparticle->SetPart_name(trackname);

  //Set Particle Start Position

  Tparticle->SetZStart_Pos(zstartpos);

  Tparticle->SetStart_Time(starttime);
  
  //
  //Set Particle Stop Position (not used)
  //
  //Tparticle->SetStop_Pos(stoppos);
  //

  if( (edep)/MeV > 2)
    {
      G4cout << "1 -- ERROR IN EDEP: In the Iterator Edep " << 
      	G4BestUnit(edep,"Energy") << 
      	" from particle: name " << trackname << " ID " << thistrackID 
      	     << " with mum " << parentID << " produced at " 
	     << G4BestUnit(zstartpos,"Length")<< G4endl;
      
      G4cout << "\n";
    }


  

  //**************************************************************************//  



  
  //*****************************************************************************//
  //HITS PART!!!!!!!!!!!!!!!****************************************************//

  //We need to know if the hit for a specific layer has already been created or not.
  //We use the mapHit map to verify this.
  //We could have done searching the hit in the hitCollection,
  //but a std::map is more efficient in searching.
  //Since this is called several times for each event, this is a place of the code
  //to be optimized
  //This is the part for the Hits///
  //I leave it/
  //****************************************************************//
 

  
    
  
  hitMap_t::iterator it = hitMap.find(HDGeindex);
  GasHit* aHit = 0;
  if ( it != hitMap.end() )
    {
      //Hit for this layer already exists
      //remember the hit pointer
      aHit = it->second;
    }
  else
    {
      //Hit for this layer does not exists,
      //we create it
      

//            TFile *out_root_f = new TFile("histoEnrergy.root","RECREATE");
//            TH1F *hene =  new TH1F("Energy Deposition","Energy Deposition",10000,0,1000);    
//            hene->GetXaxis()->SetTitle("Deposited Energy (keV)");

      //Modified the constructor to give also the histogram
      aHit = new GasHit(HDGeindex);
      hitMap.insert( std::make_pair(HDGeindex,aHit) );
      hitCollection->insert(aHit);
    }
  
  aHit->AddEdep( edep ); //This method is defined in GasHit.hh
  
  //******************************************************************************//


  //This is the part where the TrackMap is filled//


  //Look if an entry of the Track Map is already filled
  trackMap_t::iterator itt = trackMap.find(thistrackID);
  
  
  
  if ( itt != trackMap.end() )
    {
      //track for this ID already exists
      //we update it
      //remember the hit pointer
      if(HDGeindex ==1)
	{
	  Tparticle = itt->second;

	  //G4cout << "Prompt2 Edep = " << G4BestUnit(edep,"Energy") << G4endl;

	  (itt->second)->TrackAddEDep(edep);
	}
      //  G4cout << "In the Iterator Edep " << 
      //G4BestUnit((itt->second)->GetTot_Edep(),"Energy") << 
      //" from particle " << (itt->second)->GetPart_Type() << G4endl;

      if( ( (itt->second) -> GetTot_Edep() )/MeV > 2 )
	{
	  G4cout << "2 -- ERROR IN EDEP: In the Iterator Edep " << 
	    G4BestUnit((itt->second)->GetTot_Edep(),"Energy") << 
	    " from particle " << (itt->second)->GetPart_Type() << 
	    " name " << (itt->second)->GetPart_name() << " with mum " 
	  	 << (itt->second)->GetMoth_Part_ID() << " with ID " << thistrackID 
		 << " produced at " << G4BestUnit(zstartpos,"Length") << G4endl;
	  
	  G4cout << "\n";
        }
    }
  
  else
    {
      //Track from this particle does not exist
      //we create it
      if (HDGeindex==1) 
	{
	  Tparticle->SetEdep(edep);
	}
      else 
	{
	  Tparticle->SetEdep(0);
	}
      trackMap.insert(std::make_pair(thistrackID,Tparticle) );
      
    }

  
  
  return true;

  //*********************************************************//

}

void SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
//        // ------------------------------
//        // -- Creation of the collection
//        // ------------------------------
//        // -- collectionName[0] is "SiHitCollection", as declared in constructor
  hitCollection = new GasHitCollection(GetName(), collectionName[0]);
//      
//        // ----------------------------------------------------------------------------
//        // -- and attachment of this collection to the "Hits Collection of this Event":
//        // ----------------------------------------------------------------------------
//        // -- To insert the collection, we need to get an index for it. This index
//        // -- is unique to the collection. It is provided by the GetCollectionID(...)
//        // -- method (which calls what is needed in the kernel to get this index).
  static G4int HCID = -1;
  if (HCID<0) HCID = GetCollectionID(0); // <<-- this is to get an ID for collectionName[0]
  HCE->AddHitsCollection(HCID, hitCollection);
  
  
  hitMap.clear();
  trackMap.clear();
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
	/*
  // test output of hits
  G4cout << "EndOfEvent method of SD `" << GetName() << "' called." << G4endl;
  for (size_t i = 0; i < hitCollection->GetSize(); ++i ) {
    G4cout<<i<<G4endl;
	(*hitCollection[i])->Print();
  }
*/
  
  hitCollection->PrintAllHits();
  
  // -- we could have attached the collection to the G4HCofThisEvent in this
  // -- method as well (instead of Initialize).
}

