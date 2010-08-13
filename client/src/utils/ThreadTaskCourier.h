#ifndef THREADTASKCOURIER_H
#define THREADTASKCOURIER_H

#include <QObject>
#include <QHash>

class ThreadTask;

class ThreadTaskCourier : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ThreadTaskCourier)

	friend class ThreadTask;

private slots:
	void deliverNotify(ThreadTask *task);
	void objectDestroyed();

private:
	static ThreadTaskCourier *instance;
	QHash<QObject*,int> pending;

	ThreadTaskCourier();

	static void addTask(QObject *caller);
	static void notify(ThreadTask *task);
};

#endif
