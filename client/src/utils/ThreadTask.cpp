#include "ThreadTask.h"
#include "ThreadTaskCourier.h"
#include <QApplication>

ThreadTask::ThreadTask(QObject *caller, const char *callback)
	: taskCaller(caller), taskCallback(callback), cancelFlag(false)
{
	/* This restriction could be removed by making the courier thread-local */
	Q_ASSERT(caller->thread() == qApp->thread());

	setAutoDelete(false);
	ThreadTaskCourier::addTask(caller);
}

void ThreadTask::run()
{
	Q_ASSERT(!autoDelete());
	runTask();
	ThreadTaskCourier::notify(this);
}
