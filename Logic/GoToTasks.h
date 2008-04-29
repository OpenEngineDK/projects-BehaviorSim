// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _GO_TO_TASKS_H_
#define _GO_TO_TASKS_H_

#include <Logic/TimedTask.h>
#include <Utils/Timer.h>
#include "../Location.h"

using namespace OpenEngine::Utils;

namespace OpenEngine {
namespace Logic {

  class GoToAction : public TimedTask {
  protected:
  	TransformationNode* transNode;
    Vector<3,float> destination;
    float speed;
    float acceptanceRadius;

  public:
	  GoToAction(TransformationNode* trans, Vector<3,float> dest, float speed, float accRadius) : TimedTask() {
	  	transNode = trans;
		destination = dest;
		this->speed = speed;
		acceptanceRadius = accRadius;
	  }

	  virtual ~GoToAction() {}

	  virtual TaskStatus Execute(double timeDt, double totalTime) {
		Vector<3,float> diff = destination - transNode->GetPosition();
		if (diff.GetLength() <= acceptanceRadius) {
			return TASK_SUCCESS;
		}
		
		diff.Normalize();
		transNode->SetPosition(transNode->GetPosition() + diff * speed * timeDt);

		return TASK_RUNNING;
	  }
  };
  
  class GoToLocationAction : public GoToAction {
  public:
	  GoToLocationAction(TransformationNode* trans, Location* loc, float speed)
	  : GoToAction(trans, loc->GetPosition(), speed, 5.0 /* loc->GetSize()/4 */) {
	  	destination[0] += -loc->GetSize()/2 + ((float)std::rand()/RAND_MAX)*loc->GetSize();
	  	destination[2] += -loc->GetSize()/2 + ((float)std::rand()/RAND_MAX)*loc->GetSize();
	  }

	  virtual ~GoToLocationAction() {}
  };
  
  class GoToGoldStorageAction : public GoToLocationAction {
  private:
  	Dwarf* dwarf;
  	GoldStorage* storage;  
  
  public:
	  GoToGoldStorageAction(Dwarf* dwarf, GoldStorage* storage, float speed)
	  : GoToLocationAction(dwarf->GetTransformationNode(), storage->GetLocation(), speed) {
	  	this->dwarf = dwarf;
	  	this->storage = storage;
	  }
	  
	  virtual TaskStatus Execute(double timeDt, double totalTime) {
	  	TaskStatus status = GoToLocationAction::Execute(timeDt, totalTime);
	  	if (status == TASK_SUCCESS) {
	  		dwarf->SetCurrentGoldStorage(storage);
	  	} else {
		  	dwarf->SetCurrentGoldStorage(NULL); // We definitely not in the designated gold mine
		}
	  	return status;
	  }

	  virtual ~GoToGoldStorageAction() {}
  };

}
}

#endif // _GO_TO_TASKS_H_
