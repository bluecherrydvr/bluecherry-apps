#include <assert.h>

#include "sliding_time_window.h"

sliding_time_window::sliding_time_window()
{
	sum_value = 0;
}

int sliding_time_window::setTimeWindow(int64_t interval)
{
	if (interval < 0)
		return -1;
	this->interval = interval;
	return 0;
}

void sliding_time_window::reset()
{
	sum_value = 0;
	while(!queue.empty())
		queue.pop();
}

int64_t sliding_time_window::sum()
{
	return sum_value;
}

int64_t sliding_time_window::count()
{
	return queue.size();
}

bool sliding_time_window::isTooOld(int64_t oldest_timestamp, int64_t newest_timestamp)
{
	assert(newest_timestamp >= oldest_timestamp);
	// Check for {over,under}flows
	if (newest_timestamp - interval <= newest_timestamp)
		return (oldest_timestamp < newest_timestamp - interval);
	else if (oldest_timestamp + interval > oldest_timestamp)
		return (oldest_timestamp + interval < newest_timestamp);
	else
		return (newest_timestamp - oldest_timestamp > interval);
}

int sliding_time_window::push(int64_t timestamp, int64_t value)
{
	entry entry;

	if (!queue.empty()  // Check that prev_timestamp is actual
			&& (timestamp < prev_timestamp))
		return -1;  // Monotonity not preserved
	prev_timestamp = timestamp;

	entry.timestamp = timestamp;
	entry.value = value;

	queue.push(entry);
	sum_value += value;

	while (!queue.empty()) {
		const struct entry &oldest_entry = queue.front();
		if (isTooOld(oldest_entry.timestamp, timestamp)) {
			queue.pop();
			sum_value -= oldest_entry.value;
		} else {
			return 0;
		}
	}
}
