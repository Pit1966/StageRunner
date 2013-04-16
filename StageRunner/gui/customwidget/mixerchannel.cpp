#include "mixerchannel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QRect>

#define SHADOW_X_PERCENT 10
#define SHADOW_Y_PERCENT 10
#define LO_POS_PERCENT 82
#define HI_POS_PERCENT 17

MixerChannel::MixerChannel(QWidget *parent) :
	QAbstractSlider(parent)
{
	knob_scaled_xsize = 0;
	knob_scaled_ysize = 0;
	knob_xoffset = 0;
	knob_yoffset = 0;

	knob_selected_f = false;
	knob_over_f = false;

	click_value = 0;

	lo_pos_percent = float(LO_POS_PERCENT) / 100;
	hi_pos_percent = float(HI_POS_PERCENT) / 100;
	pos_range_percent = 1.0f - (1.0f - lo_pos_percent) - hi_pos_percent;

	org_pix_back = QPixmap(":/gfx/customwidget/slider_back.png");
	org_pix_knob = QPixmap(":/gfx/customwidget/slider_button.png");
	org_pix_knob_active = QPixmap(":/gfx/customwidget/slider_button_active.png");

	minimum_size_hint = QSize(org_pix_back.width() / 4,org_pix_back.height()/2);
	setMaximumWidth(org_pix_back.width());
	setMouseTracking(true);
}

MixerChannel::~MixerChannel()
{
}

void MixerChannel::mousePressEvent(QMouseEvent *event)
{
	if (knob_rect.contains(event->pos() )) {
		knob_selected_f = true;
		click_position = event->pos();
		click_value = value();
	} else {
		knob_selected_f = false;
	}
}

void MixerChannel::mouseMoveEvent(QMouseEvent *event)
{
	static bool was_in = false;
	if (knob_selected_f) {
		knob_over_f = false;
		float movedif = click_position.y() - event->pos().y();
		setValue( float(click_value) + ( movedif * maximum() / (pos_range_percent * height())) );
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
	generate_scaled_knob();
}

void MixerChannel::paintEvent(QPaintEvent *event)
{
	if (org_pix_back.isNull() || org_pix_knob.isNull() || !knob_scaled_xsize || !knob_scaled_ysize) return;

	QPainter p(this);

	p.drawPixmap(event->rect(),org_pix_back);

	int y = lo_pos_percent * height();
	y -= knob_ysize / 2;
	y -= float(value()) / float(maximum()) * pos_range_percent * height();

	knob_rect.setRect((width() - knob_xsize) / 2 , y ,knob_xsize, knob_ysize);
	knob_scaled_rect.setRect((width() - knob_xsize) / 2 , y ,knob_scaled_xsize, knob_scaled_ysize);
	if (knob_selected_f || knob_over_f) {
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

void MixerChannel::generate_scaled_knob()
{
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
	if (org_pix_back.width() / width() > 2) {
		knob_scaled_ysize *= 2;
	}

	// Calculate an offset to adjust the misallignment caused by the shadow in the image
	knob_xoffset = knob_scaled_xsize * SHADOW_X_PERCENT / 100;
	knob_yoffset = knob_scaled_ysize * SHADOW_Y_PERCENT / 100;
	knob_xsize = knob_scaled_xsize - knob_xoffset;
	knob_ysize = knob_scaled_ysize - knob_yoffset;

}

QSize MixerChannel::minimumSizeHint() const
{
	return minimum_size_hint;
}
