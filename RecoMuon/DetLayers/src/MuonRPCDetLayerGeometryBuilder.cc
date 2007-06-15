#include <RecoMuon/DetLayers/src/MuonRPCDetLayerGeometryBuilder.h>

#include <DataFormats/MuonDetId/interface/RPCDetId.h>
#include <Geometry/CommonDetUnit/interface/GeomDet.h>
#include <RecoMuon/DetLayers/interface/MuRingForwardLayer.h>
#include <RecoMuon/DetLayers/interface/MuRodBarrelLayer.h>
#include <RecoMuon/DetLayers/interface/MuDetRing.h>
#include <RecoMuon/DetLayers/interface/MuDetRod.h>

#include <Utilities/General/interface/precomputed_value_sort.h>
#include <Geometry/CommonDetUnit/interface/DetSorting.h>
#include "Utilities/BinningTools/interface/ClusterizingHistogram.h"

#include <FWCore/MessageLogger/interface/MessageLogger.h>

#include <iostream>

using namespace std;

MuonRPCDetLayerGeometryBuilder::~MuonRPCDetLayerGeometryBuilder() {
}


// Builds the forward (first) and backward (second) layers
pair<vector<DetLayer*>, vector<DetLayer*> > 
MuonRPCDetLayerGeometryBuilder::buildEndcapLayers(const RPCGeometry& geo) {
  
  vector<DetLayer*> result[2];

  for (int endcap = -1; endcap<=1; endcap+=2) {
    int iendcap = (endcap==1) ? 0 : 1; // +1: forward, -1: backward
    
    // ME 1
    int firstStation=1;
        
    // ME 1/1
    for (int layer = RPCDetId::minLayerId; layer <= RPCDetId::maxLayerId; ++layer) { 
      vector<int> rolls;      
      std::vector<int> rings;
      int FirstStationRing = 1; 
      rings.push_back(FirstStationRing);
      for(int roll = RPCDetId::minRollId+1; 
	  roll <= RPCDetId::maxRollId; ++roll) {
	rolls.push_back(roll);
      }
      

      
      MuRingForwardLayer* ringLayer = buildLayer(endcap, rings,
						 firstStation , layer, 
						 rolls, geo);          
      if (ringLayer) result[iendcap].push_back(ringLayer);
      
    }
        
    // ME 1/2 and ME1/3       
    for(int layer = RPCDetId::minLayerId; layer <= RPCDetId::maxLayerId; ++layer) { 
      vector<int> rolls;      
      std::vector<int> rings;
      for(int ring = 2; ring <= 3; ++ring) {
	rings.push_back(ring);
      }
      for(int roll = RPCDetId::minRollId+1; roll <= RPCDetId::maxRollId; 
	  ++roll) {
	rolls.push_back(roll);
      }
                
      MuRingForwardLayer* ringLayer = buildLayer(endcap, rings, firstStation , layer, rolls, geo);          
      if (ringLayer) result[iendcap].push_back(ringLayer);
    }
  

    // ME 2 and ME 3 
    for(int station = 2; station <= RPCDetId::maxStationId; ++station) {
      for(int layer = RPCDetId::minLayerId; layer <= RPCDetId::maxLayerId; ++layer) { 
	vector<int> rolls;      
	std::vector<int> rings;
	for(int ring = RPCDetId::minRingForwardId; ring <= RPCDetId::maxRingForwardId; ++ring) {
	  rings.push_back(ring);
	}
	for(int roll = RPCDetId::minRollId+1; roll <= RPCDetId::maxRollId; ++roll) {
	  rolls.push_back(roll);
	}
                
	MuRingForwardLayer* ringLayer = buildLayer(endcap, rings, station, layer, rolls, geo);          
	if (ringLayer) result[iendcap].push_back(ringLayer);
      }
    }
    
  }
  pair<vector<DetLayer*>, vector<DetLayer*> > res_pair(result[0], result[1]); 
  return res_pair;

}



MuRingForwardLayer* 
MuonRPCDetLayerGeometryBuilder::buildLayer(int endcap,std::vector<int> rings, int station,
					   int layer,
					   vector<int>& rolls,
					   const RPCGeometry& geo) {

  const std::string metname = "Muon|RPC|RecoMuon|RecoMuonDetLayers|MuonRPCDetLayerGeometryBuilder";

  MuRingForwardLayer* result=0;

  vector<const ForwardDetRing*> muDetRings;

  for (std::vector<int>::iterator ring=rings.begin(); ring<rings.end();++ring){ 
    for (vector<int>::iterator roll = rolls.begin(); roll!=rolls.end(); ++roll) {    
      vector<const GeomDet*> geomDets;
      for(int sector = RPCDetId::minSectorForwardId; sector <= RPCDetId::maxSectorForwardId; ++sector) {
	for(int subsector = RPCDetId::minSubSectorForwardId; subsector <= RPCDetId::maxSectorForwardId; ++subsector) {
	  const GeomDet* geomDet = geo.idToDet(RPCDetId(endcap,*ring, station,sector,layer,subsector, (*roll)));

	  if (geomDet) {
	    
	    geomDets.push_back(geomDet);
	    LogTrace(metname) << "get RPC Endcap roll "
			      <<  RPCDetId(endcap,*ring, station,sector,layer,subsector, (*roll))
			      << " at R=" << geomDet->position().perp()
			      << ", phi=" << geomDet->position().phi();
	    
	  }
	}
      }
      if (geomDets.size()!=0) {
	precomputed_value_sort(geomDets.begin(), geomDets.end(), geomsort::DetPhi());
	muDetRings.push_back(new MuDetRing(geomDets));
	LogTrace(metname) << "New ring with " << geomDets.size()
			  << " chambers at z="<< muDetRings.back()->position().z();
      }
    }
  }
  
  if (muDetRings.size()!=0) {
    result = new MuRingForwardLayer(muDetRings);  
    LogTrace(metname) << "New layer with " << muDetRings.size() 
		      << " rolls, at Z " << result->position().z();
  }
  
  return result;
}


vector<DetLayer*> 
MuonRPCDetLayerGeometryBuilder::buildBarrelLayers(const RPCGeometry& geo) {
  const std::string metname = "Muon|RPC|RecoMuon|RecoMuonDetLayers|MuonRPCDetLayerGeometryBuilder";

  vector<DetLayer*> detlayers;
  vector<MuRodBarrelLayer*> result;
  int region =0;

  for(int station = RPCDetId::minStationId; station <= RPCDetId::maxStationId; station++) {
    for(int layer=RPCDetId::minLayerId; layer<= RPCDetId::maxLayerId;++layer){
      
      vector<const DetRod*> muDetRods;
      for(int sector = RPCDetId::minSectorId; sector <= RPCDetId::maxSectorId; sector++) {
	vector<const GeomDet*> geomDets;
	int nSubSectors = 0;
	int lastSubSector = -1; 
	for(int subsector = RPCDetId::minSubSectorId; subsector <= RPCDetId::maxSubSectorId; subsector++) {
	  for(int wheel = RPCDetId::minRingBarrelId; wheel <= RPCDetId::maxRingBarrelId; wheel++) {
	    for(int roll=RPCDetId::minRollId+1; roll <= RPCDetId::maxRollId; roll++){         
	      const GeomDet* geomDet = geo.idToDet(RPCDetId(region,wheel,station,sector,layer,subsector,roll));
	      if (geomDet) {
		if (lastSubSector!=subsector) {
		  lastSubSector=subsector;
		  nSubSectors++;
		}
		geomDets.push_back(geomDet);
		LogTrace(metname) << "get RPC Barrel roll " <<  RPCDetId(region,wheel,station,sector,layer,subsector,roll)
				  << " at R=" << geomDet->position().perp()
				  << ", phi=" << geomDet->position().phi() ;
	      }
	    }
	  }
	}
	
	// ------>
	//FIXME: find subsectors. As for 130, DetId number for subsectors is inconsistent.
	
	if (geomDets.size()==0) continue;

	if (nSubSectors == 1) {
	  muDetRods.push_back(new MuDetRod(geomDets));
	  LogTrace(metname) << "  New MuDetRod with " << geomDets.size()
			    << " rolls at R=" << geomDets.front()->position().perp()
			    << ", phi=" << geomDets.front()->position().phi();
	} else {
	  //Sort in phi
	  precomputed_value_sort(geomDets.begin(), geomDets.end(),geomsort::DetPhi());
	  
	  // Clusterize in phi - phi0
	  float resolution(0.01); // rad
	  float phi0 = float(geomDets.front()->position().phi());
	  float phiMin = - float(resolution);
	  float phiMax = float(geomDets.back()->position().phi()) - phi0 + resolution;

	  ClusterizingHistogram hisPhi( int((phiMax-phiMin)/resolution) + 1,
					phiMin, phiMax);

	  vector<const GeomDet*>::iterator first = geomDets.begin();
	  vector<const GeomDet*>::iterator last = geomDets.end();

	  for (vector<const GeomDet*>::iterator i=first; i!=last; i++){
	    hisPhi.fill(float((*i)->position().phi())-phi0);
	    LogTrace(metname) << "C " << float((*i)->position().phi())-phi0;
	  }
	  vector<float> phiClust = hisPhi.clusterize(resolution);

	  // LogTrace(metname) << "     Found " << phiClust.size() << " clusters in Phi, ";
	
	  vector<const GeomDet*>::iterator rodStart = first;
	  vector<const GeomDet*>::iterator separ = first;
    
	  for (unsigned int i=0; i<phiClust.size(); i++) {
	    float phiSepar;
	    if (i<phiClust.size()-1) {
	      phiSepar = (phiClust[i] + phiClust[i+1])/2.f;
	    } else {
	      phiSepar = phiMax;
	    }

	    // LogTrace(metname) << "       cluster " << i
	    // << " phisepar " << phiSepar <<endl;
	    while (separ < last && float((*separ)->position().phi())-phi0 < phiSepar ) {
	      // LogTrace(metname) << "         roll at dphi:  " << float((*separ)->position().phi())-phi0;
	      separ++;
	    }

	    if (int(separ-rodStart) > 0) {
	      muDetRods.push_back(new MuDetRod(rodStart,separ));
	      LogTrace(metname) << "  New MuDetRod with " << int(separ-rodStart)
				<< " rolls at R=" << (*rodStart)->position().perp()
				<< ", phi=" << float((*rodStart)->position().phi());
	    }
	    rodStart = separ; 
	  }
	}
	/// <------------

      }
      if (muDetRods.size()!=0) {
	result.push_back(new MuRodBarrelLayer(muDetRods));  
	LogTrace(metname) << "    New MuRodBarrelLayer with " << muDetRods.size()
			  << " rods, at R " << result.back()->specificSurface().radius();
      }
    }
  }
  
  for(vector<MuRodBarrelLayer*>::const_iterator it = result.begin(); it != result.end(); it++)
    detlayers.push_back((DetLayer*)(*it));

  return detlayers;
}
