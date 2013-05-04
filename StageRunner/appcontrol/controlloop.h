#ifndef CONTROLLOOP_H
#define CONTROLLOOP_H

#include <QElapsedTimer>
#include <QTimer>

class ControlLoop : public QObject
{
	Q_OBJECT
public:
	enum STATUS {
		STAT_IDLE,
		STAT_RUNNING
	};

private:
	volatile STATUS loop_status;
	QElapsedTimer loop_time;
	QTimer loop_timer;
	qint32 loop_exec_target_time_ms;
	bool first_process_event_f;

public:
	ControlLoop(QObject *parent = 0);
	~ControlLoop();

private:
	void init();

signals:


public slots:
	void startProcessTimer();
	void stopProcessTimer();
	void processPendingEvents();

};

#endif // CONTROLLOOP_H
