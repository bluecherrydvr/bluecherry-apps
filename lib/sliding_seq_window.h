#pragma once

#include <queue>

class sliding_seq_window {
public:
	sliding_seq_window();
	int setSeqWindow(int length);
	int getSeqWindow();
	void reset();
	int sum();
	int count();
	int push(int value);
private:
	struct entry {
		int value;
	};
	std::queue<entry> queue;
	int sum_value;
	int length;
};
