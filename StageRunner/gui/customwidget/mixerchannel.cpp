#include "mixerchannel.h"

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

MixerChannel::MixerChannel(QWidget *parent) :
	QAbstractSlider(parent)
{
	my_id = -1;

	knob_scaled_xsize = 0;
	knob_scaled_ysize = 0;
	knob_xoffset = 0;
	knob_yoffset = 0;

	knob_selected_f = false;
	knob_over_f = false;
	do_paint_f = false;

	click_value = 0;

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

void MixerChannel::setRefValue(int val)
{
	refSlider.setValue(val);
}

int MixerChannel::refValue()
{
	return refSlider.value();
}

void MixerChannel::mousePressEvent(QMouseEvent *event)
{
	if (knob_rect.contains(event->pos() )) {
		knob_selected_f = true;
		knob_over_f = true;
		click_position = event->pos();
		click_value = value();
		update();
	} else {
		knob_selected_f = false;
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
			emit sliderMoved(value());
			emit mixerMoved(value(),int(my_id));
		}
		update();
	} else {
		knob_over_f = knob_rect.contains(event->pos());
		if (knob_over_f != was_in) {
			was_in = knob_over_f;
			update();
		}
	}
}

void MixerChannel::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	knob_selected_f = false;
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

	if (knob_over_f) {
		p.drawPixmap(event->rect(),org_pix_back_active);
	} else {
		p.drawPixmap(event->rect(),org_pix_back);
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

	// Draw the reference value;
	QPen pen;
	pen.setColor(Qt::blue);
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
	int y1 = lo_pos_percent * height();
	int y2 = y1 - float(refSlider.value()) / float(maximum()) * pos_range_percent * height();
	p.drawLine(width()/2, y1, width()/2, y2);

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
	float yprop = org_pix_back.height() / org_pix_knob.height();
	float xprop = org_pix_back.width() / org_pix_knob.width();
	// Proportion of the know. We will try to keep this value
	float knobprob = org_pix_knob.height() / org_pix_knob.width();

	// Height of the knob will be scaled to fit original proportion
	knob_scaled_xsize = float(width()) / xprop;
	knob_scaled_ysize = float(height()) / yprop;
	if (knob_scaled_ysize > knobprob * knob_scaled_xsize) {
		knob_scaled_ysize = knobprob * knob_scaled_xsize;
	}
	if (float(org_pix_back.width()) / width() > 2.0f) {
		knob_scaled_ysize *= 2;
	}

	// Calculate an offset to adjust the misallignment caused by the shadow in the image
	knob_xoffset = knob_scaled_xsize * SHADOW_X_PERCENT / 100;
	knob_yoffset = knob_scaled_ysize * SHADOW_Y_PERCENT / 100;
	knob_xsize = knob_scaled_xsize - knob_xoffset;
	knob_ysize = knob_scaled_ysize - knob_yoffset;

	if (knob_scaled_xsize && knob_scaled_ysize) {
		return true;
	} else {
		return false;
	}
}

QSize MixerChannel::minimumSizeHint() const
{
	return minimum_size_hint;
}
