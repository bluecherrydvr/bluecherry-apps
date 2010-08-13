#include "ThreadTaskCourier.h"
#include "ThreadTask.h"
#include <QApplication>
#include <QThread>
#include <QMetaObject>
#include <QMetaType>

ThreadTaskCourier *ThreadTaskCourier::instance = NULL;

ThreadTaskCourier::ThreadTaskCourier()
{
	qRegisterMetaType<ThreadTask*>("ThreadTask*");
}

void ThreadTaskCourier::addTask(QObject *caller)
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());
	Q_ASSERT(caller->thread() == qApp->thread());
	if (!instance)
		instance = new ThreadTaskCourier;

	QHash<QObject*,int>::iterator it = instance->pending.find(caller);
	if (it != instance->pending.end())
	{
		(*it)++;
		return;
	}

	instance->pending.insert(caller, 1);
	connect(caller, SIGNAL(destroyed()), instance, SLOT(objectDestroyed()), Qt::DirectConnection);
}

void ThreadTaskCourier::notify(ThreadTask *task)
{
	Q_ASSERT(instance);
	
	bool ok = QMetaObject::invokeMethod(instance, "deliverNotify", Qt::QueuedConnection,
										Q_ARG(ThreadTask*,task));

	Q_ASSERT(ok);
	Q_UNUSED(ok);
}

void ThreadTaskCourier::deliverNotify(ThreadTask *task)
{
	QObject *caller = task->taskCaller;

	QHash<QObject*,int>::iterator it = pending.find(caller);
	if (it == pending.end())
	{
		/* Caller has probably been deleted already */
		delete task;
		return;
	}

	bool ok = QMetaObject::invokeMethod(caller, task->taskCallback, Qt::DirectConnection, Q_ARG(ThreadTask*,task));
	Q_ASSERT_X(ok, "ThreadTaskCourier", "Invocation of thread task callback failed");
	Q_UNUSED(ok);

	if (*it < 2)
	{
		disconnect(caller, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
		pending.erase(it);
	}
	else
		(*it)--;

	delete task;
}

void ThreadTaskCourier::objectDestroyed()
{
	pending.remove(sender());
}
