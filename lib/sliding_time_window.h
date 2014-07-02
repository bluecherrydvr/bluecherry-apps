#pragma once

#include <inttypes.h>

#include <queue>

class sliding_time_window {
public:
	sliding_time_window();
	int setTimeWindow(int64_t interval);
	void reset();
	int64_t sum();
	int64_t count();
	int push(int64_t timestamp, int64_t value);
private:
	struct entry {
		int64_t timestamp;
		int64_t value;
	};
	std::queue<entry> queue;
	int64_t sum_value;
	int64_t interval;
	int64_t prev_timestamp;

	bool isTooOld(int64_t oldest_timestamp, int64_t newest_timestamp);
};
