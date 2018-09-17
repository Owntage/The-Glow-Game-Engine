//
// Created by Owntage on 17.09.2018.
//

#include "performance_profiler.h"
#include <unordered_map>
#include <cstdint>
#include <chrono>

struct PerformanceProfilerImpl
{
	PerformanceProfilerImpl();
	void enterSection(const std::string& name);
	void exitSection(const std::string& name);
	float getSectionRelativeTime(const std::string& name);
private:
	std::unordered_map<std::string, int64_t> timeStorage;
	std::unordered_map<std::string, std::chrono::system_clock::time_point> lastTimeEntered;
	int64_t summaryTime;
};

PerformanceProfilerImpl::PerformanceProfilerImpl() : summaryTime(0)
{}

void PerformanceProfilerImpl::enterSection(const std::string& name)
{
	lastTimeEntered[name] = std::chrono::system_clock::now();
}

void PerformanceProfilerImpl::exitSection(const std::string& name)
{
	if (lastTimeEntered.find(name) == lastTimeEntered.end())
	{
		return;
	}
	auto currentTime = std::chrono::system_clock::now();
	auto enterTime = lastTimeEntered[name];
	int64_t microsecondsSpent = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - enterTime).count();
	summaryTime += microsecondsSpent;
	if (timeStorage.find(name) == timeStorage.end())
	{
		timeStorage[name] = 0;
	}
	timeStorage[name] += microsecondsSpent;
}

float PerformanceProfilerImpl::getSectionRelativeTime(const std::string& name)
{
	int64_t sTime = timeStorage[name];
	return (float) ((double) sTime / (double) summaryTime);
}

PerformanceProfiler::PerformanceProfiler()
{
	impl = new PerformanceProfilerImpl;
}

PerformanceProfiler::~PerformanceProfiler()
{
	delete impl;
}

void PerformanceProfiler::enterSection(const std::string& name)
{
	impl->enterSection(name);
}

void PerformanceProfiler::exitSection(const std::string& name)
{
	impl->exitSection(name);
}

float PerformanceProfiler::getSectionRelativeTime(const std::string& name)
{
	return impl->getSectionRelativeTime(name);
}