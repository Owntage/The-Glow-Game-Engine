//
// Created by Owntage on 17.09.2018.
//

#ifndef ANDROID_SFML_GAME_PERFORMANCE_PROFILER_H
#define ANDROID_SFML_GAME_PERFORMANCE_PROFILER_H

#include <string>

struct PerformanceProfilerImpl;

struct PerformanceProfiler
{
	PerformanceProfiler();
	~PerformanceProfiler();
	void enterSection(const std::string& name);
	void exitSection(const std::string& name);
	float getSectionRelativeTime(const std::string& name);
private:
	PerformanceProfilerImpl* impl;
};

#endif //ANDROID_SFML_GAME_PERFORMANCE_PROFILER_H
