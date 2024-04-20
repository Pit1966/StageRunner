#include "exttimelineitem.h"
#include "widgets/fxtimelineeditwidget.h"
#include "fx/fxtimelineitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QMenu>

using namespace PS_TL;

ExtTimeLineItem::ExtTimeLineItem(TimeLineWidget *timeline, int trackId)
	: TimeLineItem(timeline, trackId)
{

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

	FxItem *fx = FxItem::findFxById(fxId);
	if (fx) {
		m_fxID = fxId;
		m_linkedObjType = LINKED_FXITEM;
		setLabel(fx->name());
	}
}
