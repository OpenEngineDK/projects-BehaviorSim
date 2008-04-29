// Dwarf
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _DWARF_H_
#define _DWARF_H_

// Game engine interface
#include <Scene/ISceneNode.h>
#include <Resources/ResourceManager.h>

// For the rendering of the properties of the dwarf
#include <Renderers/IRenderer.h>
#include <Renderers/IRenderNode.h>

#include "Dummy.h"

// Forward declaration of the gold mine
class GoldStorage;

// Namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;

class Dwarf : public Dummy {
	private:
	IRenderNode* renderNode;
	
	static const int MAX_GOLD = 5;
	int gold;
	GoldStorage* currentGoldStorage;
	
	class DwarfRenderNode : public IRenderNode {
	private:
		Dwarf* dwarf;
		
	public:
		DwarfRenderNode(Dwarf* dwarf) : dwarf(dwarf) {}
		
		virtual void Apply(IRenderingView* renderingView) {
		    IRenderer* renderer = renderingView->GetRenderer();
		    //Vector<3,float> pos = dwarf->GetTransformationNode()->GetPosition();
		    Vector<3,float> color = Vector<3,float>(0.7, 0.6, 0.1);
		    float width = 5;
		    
		    int gold = dwarf->GetGoldAmount();
		    float goldRadius = 10.0;
		    
		    for (int i = 0; i < gold; i++) {
				Vector<3,float> goldPosition(
					cos(((float)i/Dwarf::MAX_GOLD)*PI*2)*goldRadius, 
					70.0, 
					sin(((float)i/Dwarf::MAX_GOLD)*PI*2)*goldRadius
					);
				renderer->DrawPoint(goldPosition, color, width);
			}
		}
    };
    
    public:

	Dwarf() : Dummy() {
		GetTransformationNode()->AddNode(new DwarfRenderNode(this));
		gold = 0;
		currentGoldStorage = NULL;
	}
	
	int GainGold(unsigned int amount) {
		int oldGold = gold;
		gold += amount;
		if (gold > Dwarf::MAX_GOLD) {
			gold = Dwarf::MAX_GOLD;
		}
		return gold - oldGold; // How much was actually gained
	}
	
	int LoseGold(unsigned int amount) {
		int oldGold = gold;
		gold -= amount;
		if (gold < 0) {
			gold = 0;
		}
		return oldGold - gold; // How much was actually lost
	}
	
	bool IsCarryingMaximumGold() {
		return (gold == Dwarf::MAX_GOLD);
	}
	
	int GetGoldAmount() {
		return gold;
	}
	
	GoldStorage* GetCurrentGoldStorage() {
		return currentGoldStorage;
	}
	
	void SetCurrentGoldStorage(GoldStorage* storage) {
		currentGoldStorage = storage;
	}
};

#endif // _DWARF_H_
