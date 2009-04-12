// Dummy
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _DUMMY_H_
#define _DUMMY_H_

// Game engine interface
#include <Scene/ISceneNode.h>
#include <Resources/ResourceManager.h>

// For the rendering of the properties of the dwarf
#include <Renderers/IRenderer.h>

// Forward declaration of the gold mine
class GoldMine;

// Namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;

class Dummy {
protected:
	ISceneNode* node;
	TransformationNode* transNode;

public:
    Dummy() {
		transNode = new TransformationNode();
	
		// TODO: Gem modellen i en display list!
	
		IModelResourcePtr objResource = 
            ResourceManager<IModelResource>::Create("Ragdoll/Ragdoll.obj");
		objResource->Load();
		node = objResource->GetSceneNode()->Clone();

		if (node != NULL) {
			transNode->AddNode(node);
			transNode->Scale(0.3, 0.3, 0.3);
		} else {
			printf("Error loading dummy model!\n");
		}

    }

    TransformationNode* GetTransformationNode() {
		return transNode;
    }
    
    ISceneNode* GetSceneNode() {
    	return node;
    }
};

#endif // _DUMMY_H_
