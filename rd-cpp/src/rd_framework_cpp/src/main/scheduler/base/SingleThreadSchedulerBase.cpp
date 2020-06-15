#include "SingleThreadSchedulerBase.h"

#include "ctpl_stl.h"
// clang-format off
#include "util/fix_ho_spdlog.h"
// clang-format on
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "util/core_util.h"

namespace rd
{
SingleThreadSchedulerBase::PoolTask::PoolTask(std::function<void()> f, SingleThreadSchedulerBase* scheduler)
	: f(std::move(f)), scheduler(scheduler)
{
}

void SingleThreadSchedulerBase::PoolTask::operator()(int id) const
{
	try
	{
		f();
		--scheduler->tasks_executing;
	}
	catch (std::exception const& e)
	{
		scheduler->log->error("Background task failed, scheduler={}, thread_id={} | {}", scheduler->name, id, e.what());
		--scheduler->tasks_executing;
	}
}

SingleThreadSchedulerBase::SingleThreadSchedulerBase(std::string name)
	: log(spdlog::stderr_color_mt<spdlog::synchronous_factory>("log", spdlog::color_mode::automatic))
	, name(std::move(name))
	, pool(std::make_unique<ctpl::thread_pool>(1))
{
	thread_id = std::this_thread::get_id();
}

void SingleThreadSchedulerBase::flush()
{
	RD_ASSERT_MSG(!is_active(), "Can't flush this scheduler in a reentrant way: we are inside queued item's execution");

	while (tasks_executing != 0)
	{
		std::this_thread::yield();
	}
}

void SingleThreadSchedulerBase::queue(std::function<void()> action)
{
	++tasks_executing;
	PoolTask task(action, this);
	pool->push(std::move(task));
}

bool SingleThreadSchedulerBase::is_active() const
{
	return thread_id == std::this_thread::get_id();
}

SingleThreadSchedulerBase::~SingleThreadSchedulerBase() = default;
}	 // namespace rd