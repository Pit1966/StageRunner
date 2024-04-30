#include "exttimelineitem.h"
#include "widgets/fxtimelineeditwidget.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxitem_includes.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QMenu>

using namespace PS_TL;

ExtTimeLineItem::ExtTimeLineItem(TimeLineWidget *timeline, int trackId)
	: TimeLineItem(timeline, trackId)
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
		m_linkedObjType = LINKED_FXITEM;
		setLabel(fx->name());

		int minLenMs = -1;
		// preset of base time values, such as minimal execution duration of item
		switch (fx->fxType()) {
		case FX_SCENE:
			minLenMs = fx->durationHint();
			if (minLenMs <= 0)	// set default duration to 2 seconds, if there is no other value given.
				minLenMs = 2000;
			break;
		case FX_AUDIO:
			minLenMs = fx->durationHint();
			qDebug() << "item duration" << minLenMs;
			if (minLenMs <= 0)
				minLenMs = 30000;
		}

		if (minLenMs > 0)
			setDuration(minLenMs);

		recalcPixelPos();
		scene()->update();

		// expand timeline if item lies beyond the timeline end
		if (endPosition() > m_timeline->timeLineDuration())
			m_timeline->setTimeLineDuration(endPosition());
	}
}

void ExtTimeLineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
}

void ExtTimeLineItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu(m_timeline);

	menu.addAction(tr("Delete item"), this, &ExtTimeLineItem::contextDeleteMe);
	menu.addAction(tr("Link to Fx"), this, &ExtTimeLineItem::contextLinkToFx);
	menu.addAction(tr("Edit label"), this, &ExtTimeLineItem::contextEditLabel);

	menu.exec(m_timeline->gfxView()->mapToGlobal(event->scenePos().toPoint()));
}

void ExtTimeLineItem::rightClicked(QGraphicsSceneMouseEvent */*event*/)
{
	qDebug() << "ExtTimeLineItem right clicked";
}

void ExtTimeLineItem::contextDeleteMe()
{
	m_timeline->removeTimeLineItem(this, true);
}

void ExtTimeLineItem::contextEditLabel()
{
	QString newlabel = QInputDialog::getText(m_timeline,
											 tr("Item config"),
											 tr("Edit Label"),
											 QLineEdit::Normal,
											 m_label);

	if (newlabel != m_label) {
		if (newlabel.isEmpty()) {
			if (m_linkedObjType == LINKED_FXITEM) {
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
