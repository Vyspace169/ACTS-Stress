#include "MovementStack.hpp"

MovementStack::MovementStack() {
	size_t DataSize = NFS_BLOB_SIZE;
	nvs_handle MovementStackHandle;

	nvs_open("storage", NVS_READWRITE, &MovementStackHandle);
	nvs_get_blob(MovementStackHandle, "MSD", (char*)DataStack, &DataSize);
	nvs_close(MovementStackHandle);

	DataStackPointer = DataSize / sizeof(movement_type_t);
}

movement_type_t MovementStack::GetActivityData() {
	if(DataStackPointer == 0) {
		return 0;
	}

	// Very lazy FIFO, but buffer is more easy to use for WriteStackToFlash method
	movement_type_t ret = DataStack[0];
	DataStackPointer--;
	memcpy(DataStack, &DataStack[1], (sizeof(movement_type_t) * DataStackPointer));
	return ret;
}

void MovementStack::PushData(movement_type_t ActivityData) {
	if(DataStackPointer == QUEUE_MAX_SIZE) {
		return;
	}
	DataStack[DataStackPointer] = ActivityData;
	DataStackPointer++;
}

int MovementStack::DataCount() {
	return DataStackPointer;
}

void MovementStack::WriteStackToFlash() {
	size_t DataSize = DataStackPointer * sizeof(movement_type_t);
	nvs_handle MovementStackHandle;

	nvs_open("storage", NVS_READWRITE, &MovementStackHandle);
	nvs_set_blob(MovementStackHandle, "MSD", (char *)DataStack, DataSize);
	nvs_commit(MovementStackHandle);
	nvs_close(MovementStackHandle);
}
