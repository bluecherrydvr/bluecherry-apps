#include <assert.h>

#include "sliding_seq_window.h"

sliding_seq_window::sliding_seq_window()
{
	length = 1;
	sum_value = 0;
}

int sliding_seq_window::setSeqWindow(int length)
{
	if (length < 1)
		return -1;
	this->length = length;
	return 0;
}

int sliding_seq_window::getSeqWindow()
{
	return length;
}

void sliding_seq_window::reset()
{
	sum_value = 0;
	while(!queue.empty())
		queue.pop();
}

int sliding_seq_window::sum()
{
	return sum_value;
}

int sliding_seq_window::count()
{
	return queue.size();
}

int sliding_seq_window::push(int value)
{
	entry entry;
	entry.value = value;
	queue.push(entry);
	sum_value += value;

	while (queue.size() > length) {
		struct entry oldest_entry = queue.front();
		queue.pop();
		sum_value -= oldest_entry.value;
	}

	return 0;
}
