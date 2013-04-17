#ifndef MIXERCHANNEL_H
#define MIXERCHANNEL_H

#include <QWidget>
#include <QAbstractSlider>
#include <QPixmap>
#include <QSize>

class MixerChannel : public QAbstractSlider
{
	Q_OBJECT
private:
	int my_id;						///< Id of MixerChannel instance (usualy the channel number) or -1 if not used

	QPixmap org_pix_back;
	QPixmap org_pix_knob;
	QPixmap org_pix_knob_active;
	QPixmap org_pix_back_active;

	int knob_xsize;					///< Width of scaled knob without shadow border
	int knob_ysize;					///< Height of scaled knob without shadow border
	int knob_scaled_xsize;			///< Width of scaled knob including shadow border
	int knob_scaled_ysize;			///< Heigh of scaled knob including shadow border
	int knob_xoffset;
	int knob_yoffset;
	QRect knob_scaled_rect;			///< The position of the knob on screen
	QRect knob_rect;				///< The position of the knob on screen without shadow

	float lo_pos_percent;
	float hi_pos_percent;
	float pos_range_percent;

	bool do_paint_f;				///< Widget Resources ok and can be drawn

	bool knob_selected_f;			///< Slider knob is clicked and selelected
	bool knob_over_f;				///< Mouse is over knob
	QPoint click_position;			///< Mouseposition when clicked in Widget
	int click_value;				///< Value when mouse clicked in widget
	QSize minimum_size_hint;

public:
	MixerChannel(QWidget *parent = 0);
	~MixerChannel();
	inline void setId(int id) {my_id = id;}
	inline int id() {return my_id;}

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	void leaveEvent(QEvent *event);
	bool generate_scaled_knob();
	QSize minimumSizeHint() const;



signals:
	void mixerMoved(int val, int id);

public slots:

};

#endif // MIXERCHANNEL_H
