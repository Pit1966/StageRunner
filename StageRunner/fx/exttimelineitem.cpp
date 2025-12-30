#include "exttimelineitem.h"
#include "widgets/fxtimelineeditwidget.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxitem_includes.h"
#include "system/log.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QMenu>

using namespace PS_TL;

ExtTimeLineItem::ExtTimeLineItem(TimeLineWidget *timeline, int trackId)
	: TimeLineBox(timeline, trackId)
	, TimeLineItemData()
{
}

FxItem *ExtTimeLineItem::linkToFxWithId(int fxId)
{
	FxItem *fx = FxItem::findFxById(fxId);
	if (!fx)
		return nullptr;

	if (!linkToFxItem(fx))
		return nullptr;

	return fx;
}

bool ExtTimeLineItem::linkToFxItem(FxItem *fx)
{
	if (fx) {
		m_fxID = fx->id();
		setLabel(fx->name());

		int minLenMs = -1;
		// preset of base time values, such as minimal execution duration of item
		switch (fx->fxType()) {
		case FX_SCENE:
			m_linkedObjType = LINKED_FX_SCENE;
			minLenMs = fx->durationHint();
			m_maxDurationMs = 0;
			if (minLenMs <= 0)	// set default duration to 2 seconds, if there is no other value given.
				minLenMs = 2000;
			m_colorBG = 0x923d0c;
			m_colorBorder = 0x923d0c;
			break;
		case FX_AUDIO:
			m_linkedObjType = LINKED_FX_AUDIO;
			minLenMs = fx->durationHint();
			m_maxDurationMs = minLenMs;
			qDebug() << "item duration" << minLenMs;
			if (minLenMs <= 0)
				minLenMs = 30000;
			break;
		case FX_SCRIPT:
			m_linkedObjType = LINKED_FX_SCRIPT;
			minLenMs = fx->durationHint();
			m_maxDurationMs = 0;
			m_colorBG = 0x413f32;
			m_colorBorder = 0x413f32;
			break;

		default:

			POPUPERRORMSG(tr("System message"),
						  tr("%1 is not supported (yet) in FX timeline").arg(FxItem::fxTypeToName(fx->fxType())),
						  m_timeline);
			return false;
		}

		if (minLenMs > 0)
			setDuration(minLenMs);

		recalcPixelPos();
		scene()->update();

		// expand timeline if item lies beyond the timeline end
		if (endPosition() > m_timeline->timeLineDuration())
			m_timeline->setTimeLineDuration(endPosition());

		return true;
	}

	return false;
}

// void ExtTimeLineItem::doubleClicked(QGraphicsSceneMouseEvent *event)
// {
// 	qDebug() << "double click";
// 	if (m_fxID <= 0)
// 		return;

// 	FxItem *fx = FxItem::findFxById(m_fxID);
// 	if (fx->fxType() == FX_SCENE) {

// 	}
// }


void ExtTimeLineItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu(m_timeline);

	menu.addAction(tr("Delete item"), this, &ExtTimeLineItem::contextDeleteMe);
	menu.addAction(tr("Link to Fx"), this, &ExtTimeLineItem::contextLinkToFx);
	if (m_fxID == 0) {
		menu.addAction(tr("Link to PAUSE"), this, &ExtTimeLineItem::contextLinkToPause);
	}
	menu.addAction(tr("Edit label"), this, &ExtTimeLineItem::contextEditLabel);

	menu.addAction(tr("Edit fadeIN time"), this, &ExtTimeLineItem::contextFadeInTime);
	menu.addAction(tr("Edit fadeOut time"), this, &ExtTimeLineItem::contextFadeOutTime);


	// get additional menu entries from overlays
	TimeLineTrack *track = timeLineTrack();
	if (track) {
		QList<TimeLineContextMenuEntry> entries = track->getOverlayContextMenu(event->scenePos());
		for (const TimeLineContextMenuEntry &e : std::as_const(entries)) {
			if (e.staticCmdFuncPara) {
				menu.addAction(e.menuLabel, this, [=](void) {
					(e.staticCmdFuncPara)(e.paraPointer);
				});
			}
			else if (e.staticCmdFunc) {
				menu.addAction(e.menuLabel, this, e.staticCmdFunc);
			}
			else {
				menu.addAction(e.menuLabel);
			}
		}
	}

	menu.exec(m_timeline->gfxView()->mapToGlobal(event->scenePos().toPoint()));
}

void ExtTimeLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	TimeLineBox::paint(painter, option, widget);

	// If there is a linked FX item, there could additional things to render
	FxItem * fx = linkedFxItem();
	if (!fx) return;

	if (m_linkedObjType == LINKED_FX_SCENE || m_linkedObjType == LINKED_FX_AUDIO) {
		int fadeInMs = fadeInTime();
		if (fadeInMs > 0) {
			// draw a ramp
			qreal baselen = m_timeline->msToPixel(fadeInMs);
			painter->setBrush(QColor(180,255,180,80));
			painter->setPen(Qt::NoPen);

			qreal b = m_penWidthBorder / 2;

			QVector<QPointF> triangle;
			triangle << QPointF(-b, m_ySize + b)
					 << QPointF(baselen, m_ySize + b)
					 << QPointF(baselen, 0/*m_ySize * 2/4*/);
			painter->drawPolygon(triangle);
		}
		int fadeOutMs = fadeOutTime();
		if (fadeOutMs > 0) {
			// draw a ramp
			qreal baselen = m_timeline->msToPixel(fadeOutMs);
			painter->setBrush(QColor(255,180,180,80));
			painter->setPen(Qt::NoPen);

			qreal b = m_penWidthBorder / 2;

			QVector<QPointF> triangle;
			triangle << QPointF(m_xSize + b, m_ySize + b)
					 << QPointF(m_xSize - baselen, m_ySize + b)
					 << QPointF(m_xSize - baselen, 0/*m_ySize * 2/4*/);
			painter->drawPolygon(triangle);
		}
	}
}

void ExtTimeLineItem::rightClicked(QGraphicsSceneMouseEvent */*event*/)
{
	qDebug() << "ExtTimeLineItem right clicked";
}

int ExtTimeLineItem::maxDuration() const
{
	return m_maxDurationMs;
}

/**
 * @brief Get fadein time.
 * @return fadein time [ms] or 0, if not set
 *
 * use fadein time stored in this object, if it is > 0
 * otherwise use fadeInTime from FxItem, but only if there is a linked FxItem
 */
int ExtTimeLineItem::fadeInTime() const
{
	int ms = 0;
	if (m_fadeInDurationMs > 0) {
		ms = m_fadeInDurationMs;
	}
	else {
		FxItem * fx = linkedFxItem();
		if (fx)
			ms = fx->fadeInTime();
	}

	return ms;
}

void ExtTimeLineItem::setFadeInTime(int ms)
{
	m_fadeInDurationMs = ms;
}

/**
 * @brief Get fadeout time.
 * @return fadeout time [ms] or 0, if not set
 *
 * use fadeout time stored in this object, if it is > 0
 * otherwise use fadeOutTime from FxItem, but only if there is a linked FxItem
 */
int ExtTimeLineItem::fadeOutTime() const
{
	int ms = 0;
	if (m_fadeOutDurationMs > 0) {
		ms = m_fadeOutDurationMs;
	}
	else {
		FxItem * fx = linkedFxItem();
		if (fx)
			ms = fx->fadeOutTime();
	}

	return ms;
}

void ExtTimeLineItem::setFadeOutTime(int ms)
{
	m_fadeOutDurationMs = ms;
}


void ExtTimeLineItem::contextDeleteMe()
{
	m_timeline->removeTimeLineItem(this, true);
}

void ExtTimeLineItem::contextEditLabel()
{
	bool ok;
	QString newlabel = QInputDialog::getText(m_timeline,
											 tr("Item config"),
											 tr("Edit Label"),
											 QLineEdit::Normal,
											 m_label,
											 &ok);
	if (!ok)
		return;

	if (newlabel != m_label) {
		if (newlabel.isEmpty()) {
			if (m_linkedObjType >= LINKED_FX_SCENE && m_linkedObjType <= LINKED_FX_LAST) {
				FxItem *fx = FxItem::findFxById(m_fxID);
				if (fx) {
					newlabel = fx->name();
				}
			}
		}
		setLabel(newlabel);
	}
}

void ExtTimeLineItem::contextLinkToFx()
{
	int fxId = QInputDialog::getInt(m_timeline, tr("Item config"),
									tr("Enter ID of Fx"));

	linkToFxWithId(fxId);
}

void ExtTimeLineItem::contextLinkToPause()
{
	m_linkedObjType = CMD_PAUSE;
	m_colorBG = 0xb02127;
	setDuration(1000);
	m_maxDurationMs = 5000;
}

void ExtTimeLineItem::contextFadeInTime()
{
	int ms = fadeInTime();
	int valms = QInputDialog::getInt(m_timeline,
									 tr("Item config"),
									 tr("Edit fadeIN time"),
									 ms);
	if (valms == ms)
		return;

	m_fadeInDurationMs = valms;
	update();
}

void ExtTimeLineItem::contextFadeOutTime()
{
	int ms = fadeOutTime();
	int valms = QInputDialog::getInt(m_timeline,
									 tr("Item config"),
									 tr("Edit fadeOut time"),
									 ms);
	if (valms == ms)
		return;

	m_fadeOutDurationMs = valms;
	update();
}
