// Gold storage
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _GOLD_STORAGE_H_
#define _GOLD_STORAGE_H_

// Game engine interface
#include <Renderers/IRenderer.h>
#include <Renderers/IRenderNode.h>

#include "Location.h"

// Namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Renderers;

// Forward declaration of the dwarf class
class Dwarf;

class GoldStorage {
private:
	Location* location;
	int amountPerGain;
	int amountPerLose;
	int gold;
	int capacity;
	IRenderNode* storageRenderNode;
	
	class GoldStorageRenderNode : public IRenderNode {
	private:
		GoldStorage* storage;
		
	public:
		GoldStorageRenderNode(GoldStorage* storage) : storage(storage) {}
		
		virtual void Apply(IRenderingView* renderingView) {
			storage->GetLocation()->GetRenderNode()->Apply(renderingView);
			
		    IRenderer* renderer = renderingView->GetRenderer();
		    Location* location = storage->GetLocation();
		    Vector<3,float> locationPosition = location->GetPosition();
		    float margin = 5.0;
		    float locationSize = location->GetSize() - margin * 2;
		    Vector<3,float> color = location->GetColor();
		    int width = 3;
		    int capacity = storage->GetCapacity();
		    int goldPerLine = (int)sqrt(capacity);
		    Vector<3,float> startPos = locationPosition - Vector<3,float>(locationSize/2, 0.0, locationSize/2);
		    
		    float gold = storage->GetGoldAmount();
		    for (int i = 0; i < gold; i++) {
			  	Vector<3,float> goldDisplacement = (locationSize/goldPerLine)*Vector<3,float>((float)(i % goldPerLine), 0.0, (float)((int)(i / goldPerLine))) ;
				renderer->DrawPoint(startPos + goldDisplacement, color, width);
			}
		}
    };

public:
    GoldStorage(Location* loc, int gold = 100, int capacity = 100, int amountPerGain = 1, int amountPerLose = 1) {
		location = loc;
		this->gold = gold;
		this->amountPerGain = amountPerGain;
		this->amountPerLose = amountPerLose;
		this->capacity = capacity;
		storageRenderNode = new GoldStorageRenderNode(this);
    }

    int GetGoldAmount() {
		return gold;
    }
    
    bool IsDepleted() {
    	return (gold <= 0);
    }
    
    bool IsFull() {
    	return (gold >= capacity);
    }
    
    bool TakeGold(Dwarf* dwarf) {
    	int dugGold = amountPerGain;
    	if (amountPerGain > gold) { // The dwarf is attempting to dig more gold than is available in the mine
    		dugGold = gold;
    	}
    	
    	gold -= dugGold;
    	int dwarfCanCarry = dwarf->GainGold(dugGold);
    	if (dwarfCanCarry < dugGold) {
    		gold += dugGold - dwarfCanCarry; // The dwarf is overburdend, put the surplus back into the mine
    	}
    	
    	return !(IsDepleted() || dwarf->IsCarryingMaximumGold());
    }
    
    bool DepositGold(Dwarf* dwarf) {
		int goldDeposited = dwarf->LoseGold(amountPerLose);
		if (gold + goldDeposited > capacity) {
			goldDeposited = capacity - gold; // We can only deposit the remaining up until the capacity limit
		}
		
		gold += goldDeposited;

    	return (goldDeposited == amountPerLose);
    }
    
    Location* GetLocation() {
    	return location;
    }
    
    IRenderNode* GetRenderNode() {
    	return storageRenderNode;
    }
    
    int GetCapacity() {
    	return capacity;
    }
};

#endif // _GOLD_STORAGE_H_
