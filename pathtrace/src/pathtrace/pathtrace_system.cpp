#include <pathtrace/pathtrace_system.hpp>

#include <omp.h>

namespace pathtrace {

PathtraceSystem::PathtraceSystem()
{
	ui = std::make_unique<PathtraceSystemUI>(state);
	
}

PathtraceSystem::~PathtraceSystem()
{
}

void PathtraceSystem::update(float delta_time)
{
	ui->update(delta_time);
}

}