#include "dmxmonitor.h"

#include <QPainter>
#include <QDebug>


// #include <QDBusConnection>

DmxMonitor::DmxMonitor(QWidget *parent) :
	QWidget(parent)
{
	init();
}

void DmxMonitor::init()
{
	bars = 0;
	used_bars = 0;
	m_myUniverse = 0;
	m_autoBarsEnabled = false;
	memset(bar_value,0,sizeof(int) * 512);
	m_frameRateUpdateTimer.start();
}

void DmxMonitor::closeEvent(QCloseEvent *)
{
	emit monitorClosed(this);
}

void DmxMonitor::setChannelPeakBars(int num)
{
	if (num < 0 || num > 512) num = 512;
	bars = num;
	used_bars = num;
	memset(bar_value,0,sizeof(int)*512);

	if (bars < 48) {
		setMinimumWidth(bars * 20);
	}
	setMaximumWidth(bars * 20);

	update();
}

void DmxMonitor::paintEvent(QPaintEvent *)
{
	int xsize = width();
	int ysize = height();

	QPainter paint(this);
	// Hintergrund löschen
	paint.setPen(Qt::blue);
	paint.setBrush(Qt::white);
	paint.eraseRect(0,0,xsize,ysize);

	paint.translate(0,ysize-1);
	paint.scale(1,-1);


	if (bars) {
		int bar_width = xsize / bars;
		int bar_height = ysize - 20;

		// Balken
		paint.setPen(Qt::blue);
		for (int t=0; t< bars; t++) {
			if (t >= used_bars) {
				paint.setPen(Qt::red);
			}
			int h = bar_height * bar_value[t] / 255;
			paint.drawRect(t * bar_width + 1, 0, bar_width - 2, h);
		}
		// Texte
		if (bar_width > 14) {
			int yp = height();
			paint.resetTransform();
			for (int t=0; t< bars; t++) {
				if (m_autoBarsEnabled) {
					paint.setPen(Qt::white);
					QString val = QString("%1").arg(t+1,3,10);
					QRect rect = paint.boundingRect(0,18,bar_width,12,Qt::AlignCenter,val);
					paint.drawText(t * bar_width + rect.x(), rect.y(), val);
				}

				if (t >= used_bars) {
					paint.setPen(Qt::red);
				} else {
					paint.setPen(Qt::black);
				}
				QRect rect = paint.boundingRect(0,18,bar_width,18,Qt::AlignCenter,QString::number(bar_value[t],16));
				paint.drawText(t * bar_width + rect.x(), yp - 4/*-rect.y()*/ , QString::number(bar_value[t],16) );
			}
		}
	}

	paint.resetTransform();
	paint.setPen(Qt::white);
	paint.drawText(5,20, m_displayedFrameRateMsg);
}

void DmxMonitor::setValueInBar(quint32 bar, uchar value)
{
	if (bar >= quint32(bars)) return;
	if (bar_value[bar] != value) {
		bar_value[bar] = value;
		update();
	}
}

void DmxMonitor::setDetectedChannelsNumber(int channels)
{
	used_bars = channels;
}

void DmxMonitor::setDmxValues(int universe, const QByteArray &dmxValues)
{
	if (universe != m_myUniverse)
		return;

	bool modified = false;
	for (int t=0; t<dmxValues.size(); t++) {
		if (t>=512)
			break;

		uchar val = dmxValues.at(t);
		if (m_autoBarsEnabled && val > 0 && t >= bars) {
			bars = ((t/12)+1) * 12 + 1;
			used_bars = bars;
		}

		if (bar_value[t] != val) {
			bar_value[t] = val;
			modified = true;
		}
	}

	if (modified)
		update();
}

void DmxMonitor::setFrameRateInfo(YadiDevice *yadiDev, const QString &frameRateMsg)
{
	Q_UNUSED(yadiDev);
	if (m_lastReceivedFrameRateMsg != frameRateMsg) {
		m_lastReceivedFrameRateMsg = frameRateMsg;

		if (m_frameRateUpdateTimer.elapsed() > 200) {
			m_frameRateUpdateTimer.start();
			m_displayedFrameRateMsg = m_lastReceivedFrameRateMsg;
			update();
		}
	}
}
