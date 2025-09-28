//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#include "mixerchannel.h"
#include "customwidget/extmimedata.h"
#include "dmxchannel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QRect>
#include <QPen>

#define SHADOW_X_PERCENT 10
#define SHADOW_Y_PERCENT 10
#define LO_POS_PERCENT 85
#define HI_POS_PERCENT 15

MixerChannel::MixerChannel(QWidget *parent)
	: QAbstractSlider(parent)
{
	my_id = -1;
	my_universe = -1;
	my_dmx_channel = -1;
	my_dmx_channel_ref = nullptr;

	prop_channel_shown_f = false;
	prop_selectable_f = false;
	prop_selected_f = false;

	knob_scaled_xsize = 0;
	knob_scaled_ysize = 0;
	knob_xoffset = 0;
	knob_yoffset = 0;

	knob_selected_f = false;
	knob_over_f = false;
	do_paint_f = false;

	click_value = 0;
	currentButton = 0;


	lo_pos_percent = float(LO_POS_PERCENT) / 100;
	hi_pos_percent = float(HI_POS_PERCENT) / 100;
	pos_range_percent = 1.0f - (1.0f - lo_pos_percent) - hi_pos_percent;

	org_pix_back = QPixmap(":/gfx/customwidget/slider_back.png");
	org_pix_back_active = QPixmap(":/gfx/customwidget/slider_back_active.png");
	org_pix_knob = QPixmap(":/gfx/customwidget/slider_button.png");
	org_pix_knob_active = QPixmap(":/gfx/customwidget/slider_button_active.png");

	minimum_size_hint = QSize(org_pix_back.width() / 4,org_pix_back.height()/2);
	setMaximumWidth(org_pix_back.width());
	setMouseTracking(true);
}

MixerChannel::~MixerChannel()
{
	// qDebug("destroy mixer: %d", my_dmx_channel+1);
}

void MixerChannel::setDmxId(int universe, int dmx_channel, DmxChannel *dmxChP)
{
	my_universe = universe;
	my_dmx_channel = dmx_channel;
	my_dmx_channel_ref = dmxChP;
}

void MixerChannel::setUniverse(int universe)
{
	my_universe = universe;
}

void MixerChannel::setRange(int min, int max)
{
	QAbstractSlider::setRange(min,max);
	refSlider.setRange(min,max);
}

void MixerChannel::setMinimum(int min)
{
	QAbstractSlider::setMinimum(min);
	refSlider.setMinimum(min);
}

void MixerChannel::setMaximum(int max)
{
	QAbstractSlider::setMaximum(max);
	refSlider.setMaximum(max);
}

void MixerChannel::setValues(int val, int refval)
{
	QAbstractSlider::setValue(val);
	refSlider.setValue(refval);
}

void MixerChannel::setRefValue(int val, int colidx)
{
	if (refSlider.value() != val || (colidx >=0 && refSliderColorIndex != colidx) ) {
		refSlider.setValue(val);
		if (colidx >= 0) {
			refSliderColorIndex = colidx;
		}
		update();
	}
}

/**
 * @brief Force emit if signal mixerSliderMoved with the current value
 */
void MixerChannel::emitCurrentValue()
{
	emit mixerSliderMoved(value(), my_id);
}

int MixerChannel::refValue()
{
	return refSlider.value();
}

void MixerChannel::setSelected(bool state)
{
	if (prop_selected_f != state) {
//		qDebug() << Q_FUNC_INFO << "mixer" << my_id << state;
		prop_selected_f = state;
		update();
		emit mixerSelected(state,int(my_id));
	}
}

void MixerChannel::setSelectable(bool state)
{
	prop_selectable_f = state;
	if (!state) {
		prop_selected_f = false;
	}
}

void MixerChannel::mousePressEvent(QMouseEvent *event)
{
	if (knob_rect.contains(event->pos())  && event->button() == Qt::LeftButton) {
		knob_selected_f = true;
		knob_over_f = true;
		click_position = event->pos();
		click_value = value();
		update();
	} else {
		knob_selected_f = false;
		drag_begin_pos = event->pos();
	}

	currentButton = event->button();

	if (currentButton == Qt::LeftButton) {
		emit clicked();
	}
	else if (currentButton == Qt::RightButton) {
		emit rightClicked();
	}

}

void MixerChannel::mouseMoveEvent(QMouseEvent *event)
{
	static bool was_in = false;
	if (knob_selected_f) {
		float movedif = click_position.y() - event->pos().y();
		int new_value = float(click_value) + ( movedif * maximum() / (pos_range_percent * height()));
		if (new_value != value()) {
			setValue( new_value );
			emit sliderMoved(new_value);
			emit mixerSliderMoved(new_value, my_id);
		}
		update();
	} else {

		knob_over_f = knob_rect.contains(event->pos());
		if (knob_over_f != was_in) {
			was_in = knob_over_f;
			update();
		}
		if ( currentButton == Qt::LeftButton && (event->pos()-drag_begin_pos).manhattanLength() > 10) {
			QDrag *drag = new QDrag(this);
			ExtMimeData *mdata = new ExtMimeData();
			QPixmap pixmap(size());
			render(&pixmap);

			mdata->setText(QString("Mixer Id: %1").arg(id()));
			mdata->mixerChannel = this;
			drag->setMimeData(mdata);
			drag->setPixmap(pixmap);
			drag->setHotSpot(drag_begin_pos);
			/*Qt::DropAction dropaction = */drag->exec();

			currentButton = 0;
		}

	}
}

void MixerChannel::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	if (!knob_selected_f) {
		if (prop_selectable_f) {
			setSelected(!prop_selected_f);
		}
	} else {
		knob_selected_f = false;
	}
	currentButton = 0;

	update();
}

void MixerChannel::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	do_paint_f = generate_scaled_knob();
}

void MixerChannel::paintEvent(QPaintEvent *event)
{
	if (!do_paint_f) return;

	QPainter p(this);
	QFont unifont(p.font());

	if (knob_over_f) {
		p.drawPixmap(event->rect(),org_pix_back_active);
	} else {
		p.drawPixmap(event->rect(),org_pix_back);
	}

	// If Selected draw a box if selected
	if (prop_selected_f) {
		QBrush brush(Qt::SolidPattern);
		brush.setColor(QColor(0,50,0,40));
		p.setPen(Qt::NoPen);
		p.setBrush(brush);
		p.drawRect(0,0,width(),height());
	}


	int y;
	int scales = 11;
	int linelen = width() / 3;
	float step = pos_range_percent / scales;
	p.setPen(Qt::white);
	for (int s=0; s<=scales; s++) {
		y = (hi_pos_percent + step * s) * height();
		p.drawLine(0,y,linelen,y);
		p.drawLine(width()-linelen,y,width(),y);
	}

	int y1 = lo_pos_percent * height();

	// Draw the reference value;
	if (refSlider.value()) {
		QPen pen;
		pen.setColor(Qt::darkGreen);
		if (width() < 20) {
			pen.setWidth(1);
		}
		else if (width() < 30) {
			pen.setWidth(2);
		}
		else {
			pen.setWidth(3);
		}
		p.setPen(pen);
		int y2 = y1 - float(refSlider.value()) / float(maximum()) * pos_range_percent * height();
		p.drawLine(width()/2, y1, width()/2, y2);

		if (refSliderColorIndex == 1) {
			pen.setColor(Qt::red);
			p.setPen(pen);
			y2 = y1 - float(value()) / float(maximum()) * pos_range_percent * height();
			p.drawLine(width()/2, y1, width()/2, y2);
		}
	}
	// Draw Channelinfo if desired
	if (prop_channel_shown_f) {
		QFont font(p.font());
		font.setFixedPitch(true);
		font.setItalic(true);
		font.setBold(true);
		p.setFont(font);
		p.setPen(Qt::white);
		QRect bound(QPoint(0,y1),QPoint(width(),height()));
		p.drawText(bound,QString::number(my_dmx_channel+1),QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

	}
	if (label.size()) {
		QFont font(p.font());
		font.setFixedPitch(true);
		font.setItalic(false);
		font.setBold(false);
		p.setFont(font);
		p.setPen(Qt::gray);
		QRect bound(QPoint(0,y1),QPoint(width(),height()));
		p.drawText(bound,label,QTextOption(Qt::AlignHCenter | Qt::AlignBottom));
	}

//	{   // For drag/drop position debugging
//		QFont font(p.font());
//		font.setFixedPitch(true);
//		font.setItalic(false);
//		font.setPixelSize(9);
//		p.setFont(font);
//		p.setPen(Qt::yellow);
//		QRect bound(QPoint(0,y1),QPoint(width(),height()));
//		QString s = QString("%1").arg(my_id);
//		p.drawText(bound,s,QTextOption(Qt::AlignLeft | Qt::AlignBottom));
//	}

//	if (my_dmx_channel_ref && my_dmx_channel_ref->deskPositionIndex >= 0) {
//		QFont font(p.font());
//		font.setFixedPitch(true);
//		font.setItalic(false);
//		font.setPixelSize(9);
//		p.setFont(font);
//		p.setPen(Qt::black);
//		QRect bound(QPoint(0,y1),QPoint(width()-1,height()));
//		QString s = QString("%1").arg(my_dmx_channel_ref->deskPositionIndex);
//		p.drawText(bound,s,QTextOption(Qt::AlignRight | Qt::AlignBottom));
//	}

	if (my_dmx_type > DMX_GENERIC) {
		QString tstr;

		if (my_dmx_type >= DMX_INTENSITY_MASTER_DIMMER && my_dmx_type <= DMX_INTENSITY_VALUE_FINE)
			tstr = "I";
		else if (my_dmx_type >= DMX_POSITION_PAN && my_dmx_type <= DMX_POSITION_PAN_FINE)
			tstr = "PAN";
		else if (my_dmx_type >= DMX_POSITION_TILT && my_dmx_type <= DMX_POSITION_TILT_FINE)
			tstr = "TILT";
		else if (my_dmx_type >= DMX_COLOR_MACRO && my_dmx_type <= DMX_COLOR_CTB_MIXER)
			tstr = "COL";
		else
			tstr.clear();

		QFont font(p.font());
		font.setFixedPitch(true);
		font.setItalic(false);
		font.setBold(false);
		p.setFont(font);
		p.setPen(Qt::red);
		QRect bound(QPoint(0,0),QPoint(width(),height()));
		p.drawText(bound,tstr,QTextOption(Qt::AlignHCenter | Qt::AlignTop));
	}

	if (my_universe > 0) {
		p.setFont(unifont);
		p.setPen(Qt::gray);
		QRect bound(QPoint(0,20),QPoint(width(), height()));
		QString uniTxt = QString("U%1").arg(my_universe + 1);
		p.drawText(bound, uniTxt, QTextOption(Qt::AlignHCenter | Qt::AlignTop));
	}


	// Draw the knob
	y = lo_pos_percent * height();
	y -= (knob_ysize / 2) + (knob_ysize * 3 / 100);
	y -= float(value()) / float(maximum()) * pos_range_percent * height();

	knob_rect.setRect((width() - knob_xsize) / 2 , y ,knob_xsize, knob_ysize);
	knob_scaled_rect.setRect((width() - knob_xsize) / 2 , y ,knob_scaled_xsize, knob_scaled_ysize);
	if (knob_selected_f) {
		p.drawPixmap(knob_scaled_rect,org_pix_knob_active);
	} else {
		p.drawPixmap(knob_scaled_rect,org_pix_knob);
	}

}

void MixerChannel::leaveEvent(QEvent *event)
{
	Q_UNUSED(event);
	knob_over_f = false;
	update();
}

bool MixerChannel::generate_scaled_knob()
{
	if (org_pix_back.isNull() || org_pix_back_active.isNull()
			|| org_pix_knob.isNull() || org_pix_knob_active.isNull()
			|| width() < 5)
		return false;

	// Calc original proportion between slider knob and slider background taken
	// from the images
	float yprop = float(org_pix_back.height()) / org_pix_knob.height();
	float xprop = float(org_pix_back.width()) / org_pix_knob.width();
	// Proportion of the knob. We will try to keep this value
	float knobprob = float(org_pix_knob.height()) / org_pix_knob.width();

	// Height of the knob will be scaled to fit original proportion
	knob_scaled_xsize = float(width()) / xprop;
	knob_scaled_ysize = float(height()) / yprop;
	if (knob_scaled_ysize > knobprob * knob_scaled_xsize) {
		knob_scaled_ysize = knobprob * knob_scaled_xsize;
	}
//	if (float(org_pix_back.width()) / width() > 2.0f) {
//		knob_scaled_ysize *= 2;
//	}

	// Calculate an offset to adjust the misallignment caused by the shadow in the image
	knob_xoffset = knob_scaled_xsize * SHADOW_X_PERCENT / 100;
	knob_yoffset = knob_scaled_ysize * SHADOW_Y_PERCENT / 100;
	knob_xsize = knob_scaled_xsize - knob_xoffset;
	knob_ysize = knob_scaled_ysize - knob_yoffset;

	if (knob_scaled_xsize > 5 && knob_scaled_ysize > 5) {
		return true;
	} else {
		return false;
	}
}

QSize MixerChannel::minimumSizeHint() const
{
	return minimum_size_hint;
}

void MixerChannel::notifyChangedDmxChannel(int universe, int dmxchannel, int dmxvalue)
{
	if (dmxchannel == my_dmx_channel && universe == my_universe) {
		int unscaled_dmx_value = dmxvalue * refSlider.maximum() / 255;
		refSlider.setValue(unscaled_dmx_value);
		update();
	}
}

void MixerChannel::setRefSliderColorIndex(int colidx)
{
	if (colidx >= 0) {
		if (colidx != refSliderColorIndex) {
			refSliderColorIndex = colidx;
			update();
		}
	}
}

void MixerChannel::setLabelText(const QString &text)
{
	if (label != text) {
		label = text;
		update();
	}
}
