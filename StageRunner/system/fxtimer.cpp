#include "fxtimer.h"
#include "fxitem.h"

FxTimer::FxTimer(QObject *parent)
	: QTimer(parent)
{
	setSingleShot(true);
	connect(this, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

FxTimer::FxTimer(FxItem *fx)
	: QTimer()
	, m_fxItem(fx)
{
	setSingleShot(true);
	connect(this, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

FxTimer::~FxTimer()
{
//	qDebug() << "destroyed fxtimer" << this;
}

void FxTimer::onTimeout()
{
	if (m_fxItem && FxItem::exists(m_fxItem))
		emit fxTimeout(m_fxItem);

	emit fxTimeout(this);

	deleteLater();
}
