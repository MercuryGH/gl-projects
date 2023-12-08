#pragma once
#include <chrono>

namespace renderer {

class CpuTimer
{
public:
	void start();
	void stop();
	double elapsed_milliseconds();
	double elapsed_seconds();

private:
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::chrono::time_point<std::chrono::steady_clock> end_time;
	bool running{ false };
};

}

