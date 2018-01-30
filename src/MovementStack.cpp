#include "MovementStack.hpp"

double MovementStack::GetActivityData() {
	if(!ActivityDataQueue.empty()) {
		return ActivityDataQueue.front();
	}
	else {
		return 0;
	}
}

void MovementStack::PopData() {
	ActivityDataQueue.pop();
}

void MovementStack::PushData(double ActivityData) {
	if(ActivityDataQueue.size() < QUEUE_MAX_SIZE) {
		ActivityDataQueue.push(ActivityData);
	}
}

int MovementStack::DataCount() {
	return ActivityDataQueue.size();
}
