// Location
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _LOCATION_H_
#define _LOCATION_H_

// Game engine interface
#include <Renderers/IRenderer.h>
#include <Renderers/IRenderNode.h>

// Namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Renderers;

// Forward declaration of the dwarf class
class Dwarf;

class Location {
private:
	Vector<3,float> position;
	IRenderNode* renderNode;
	float size;
	Vector<3,float> color;
	
	class LocationRenderNode : public IRenderNode {
	private:
		Location* location;
		
	public:
		LocationRenderNode(Location* loc) : location(loc) {}
		
		virtual void Apply(IRenderingView* renderingView) {
		    IRenderer* renderer = renderingView->GetRenderer();
		    Vector<3,float> pos = location->GetPosition();
		    float size = location->GetSize();
		    Vector<3,float> color = location->GetColor();
		    float width = 2;
		    renderer->DrawLine(Line(pos + Vector<3,float>(-size/2, 0.0, size/2), pos + Vector<3,float>(size/2, 0.0, size/2)), color, width);
   		    renderer->DrawLine(Line(pos + Vector<3,float>(-size/2, 0.0, -size/2), pos + Vector<3,float>(size/2, 0.0, -size/2)), color, width);
   		    renderer->DrawLine(Line(pos + Vector<3,float>(-size/2, 0.0, size/2), pos + Vector<3,float>(-size/2, 0.0, -size/2)), color, width);
   		    renderer->DrawLine(Line(pos + Vector<3,float>(size/2, 0.0, size/2), pos + Vector<3,float>(size/2, 0.0, -size/2)), color, width);
			//renderer->DrawPoint(pos, color, width * 5);
		}
    };

public:
    Location(Vector<3,float> pos, Vector<3,float> color, float size = 50.0) {
		position = pos;
		this->size = size;
		this->color = color;
		renderNode = new LocationRenderNode(this);
    }

    Vector<3,float> GetPosition() {
		return position;
    }
    
    IRenderNode* GetRenderNode() {
    	return renderNode;
    }
    
    float GetSize() {
    	return size;
    }
    
    Vector<3,float> GetColor() {
    	return color;
    }
};

#endif // _LOCATION_H_
