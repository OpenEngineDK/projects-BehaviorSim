// 
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _MINE_TASKS_H_
#define _MINE_TASKS_H_

#include <Logic/Task.h>
#include <Utils/Timer.h>
#include "../Location.h"

using namespace OpenEngine::Utils;

namespace OpenEngine {
namespace Logic {

  class MineAction : public Task {
  protected:
  	Dwarf* dwarf;

  public:
	  MineAction(Dwarf* dwarf) {
	  	this->dwarf = dwarf;
	  }

	  virtual ~MineAction() {}

	  virtual TaskStatus Execute() {
	  	GoldStorage* storage = dwarf->GetCurrentGoldStorage();
	  	bool result = storage->TakeGold(dwarf);
	  	if (result) {
		  	return TASK_SUCCESS;
		} else {
			return TASK_FAIL;
		}
	  }
  };

  class UnloadGoldAction : public Task {
  protected:
  	Dwarf* dwarf;
  	GoldStorage* storage;

  public:
	  UnloadGoldAction(Dwarf* dwarf, GoldStorage* storage) {
	  	this->dwarf = dwarf;
	  	this->storage = storage;
	  }

	  virtual ~UnloadGoldAction() {}

	  virtual TaskStatus Execute() {
	  	bool result = storage->DepositGold(dwarf);
	  	if (result) {
		  	return TASK_SUCCESS;
		} else {
			return TASK_FAIL;
		}
	  }
  };
  
  class IsGoldStorageNotDepletedCond : public Task {
  protected:
  	GoldStorage* storage;

  public:
	  IsGoldStorageNotDepletedCond(GoldStorage* storage) {
	  	this->storage = storage;
	  }

	  virtual ~IsGoldStorageNotDepletedCond() {}

	  virtual TaskStatus Execute() {
	  	bool depleted = storage->IsDepleted();
	  	if (!depleted) {
		  	return TASK_SUCCESS;
		} else {
			return TASK_FAIL;
		}
	  }
  };
  
  class IsGoldStorageNotFullCond : public Task {
  protected:
  	GoldStorage* storage;

  public:
	  IsGoldStorageNotFullCond(GoldStorage* storage) {
	  	this->storage = storage;
	  }

	  virtual ~IsGoldStorageNotFullCond() {}

	  virtual TaskStatus Execute() {
	  	bool full = storage->IsFull();
	  	if (!full) {
		  	return TASK_SUCCESS;
		} else {
			return TASK_FAIL;
		}
	  }
  };
  
  class SleepAction : public TimedTask {
  protected:
  	float sleepTime;
  	Dwarf* dwarf;

  public:
	  SleepAction(Dwarf* dwarf, float sleepTime = 10.0) : TimedTask() {
	  	this->dwarf = dwarf;
	  	this->sleepTime = sleepTime;
	  }

	  virtual ~SleepAction() {}
	  
	  virtual void Start() {
	  	dwarf->GetTransformationNode()->Rotate(PI/2, 0.0, 0.0);
	  }
	  
	  virtual void End() {
	  	dwarf->GetTransformationNode()->Rotate(-PI/2, 0.0, 0.0);
	  }

	  virtual TaskStatus Execute(double timeDt, double totalTime) {
	  	if (totalTime >= sleepTime * 1000) {
	  		return TASK_SUCCESS;
	  	} else {
	  		return TASK_RUNNING;
	  	}
	  }
  };
  
  class DrinkAction : public TimedTask {
  protected:
	Quaternion<float> oldRotation;
	Vector<3,float> oldPosition;
  	float drinkTime;
  	Dwarf* dwarf;

  public:
	  DrinkAction(Dwarf* dwarf, float drinkTime = 5.0) : TimedTask() {
	  	this->dwarf = dwarf;
	  	this->drinkTime = drinkTime;
	  }

	  virtual ~DrinkAction() {}
	  
	  virtual void Start() {
	  	oldPosition = dwarf->GetTransformationNode()->GetPosition();
	    oldRotation = dwarf->GetTransformationNode()->GetRotation();
	  	dwarf->GetTransformationNode()->Rotate(PI, 0.0, 0.0);
	  	dwarf->GetTransformationNode()->Move(0.0, -10.0, 0.0);
	  }
	  
	  virtual void End() {
	  	dwarf->GetTransformationNode()->SetRotation(oldRotation);
	  	dwarf->GetTransformationNode()->SetPosition(oldPosition);
	  }

	  virtual TaskStatus Execute(double timeDt, double totalTime) {
	  	if (totalTime >= drinkTime * 1000) {
	  		return TASK_SUCCESS;
	  	} else {
		  	dwarf->GetTransformationNode()->Rotate(0.0, PI*timeDt*0.005, 0.0);	  		
	  		return TASK_RUNNING;
	  	}
	  }
  };

}
}

#endif // _MINE_TASKS_H_
