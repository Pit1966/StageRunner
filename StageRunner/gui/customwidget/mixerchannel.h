#ifndef MIXERCHANNEL_H
#define MIXERCHANNEL_H

#include <QWidget>
#include <QAbstractSlider>
#include <QPixmap>
#include <QSize>

class MixerChannel : public QAbstractSlider
{
	Q_OBJECT
public:
	Q_PROPERTY(bool channelShown READ channelShown WRITE setChannelShown)
	Q_PROPERTY(bool selectable READ isSelectable WRITE setSelectable)
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected)


private:
	int my_id;						///< Id of MixerChannel instance (usualy the tube number) or -1 if not used
	int my_universe;				///< This is the universe that corresponds with the channel of this mixer (-1 if not used)
	int my_dmx_channel;				///< This is the dmx channel that is actually the target of the mixer value (-1 if not used)

	bool prop_channel_shown_f;
	bool prop_selectable_f;
	bool prop_selected_f;

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

	bool knob_selected_f;			///< Slider knob is clicked and selected
	bool knob_over_f;				///< Mouse is over knob
	QPoint click_position;			///< Mouseposition when clicked in Widget
	int click_value;				///< Value when mouse clicked in widget
	QSize minimum_size_hint;
	QPoint drag_begin_pos;

protected:
	int refSliderColorIndex;
	QAbstractSlider refSlider;
	int currentButton;				///< Current pressed mouse button

public:
	MixerChannel(QWidget *parent = 0);
	~MixerChannel();
	inline void setId(int id) {my_id = id;}
	inline int id() {return my_id;}
	inline void setDmxId(int universe, int dmx_channel) {my_universe = universe; my_dmx_channel = dmx_channel;}
	inline int dmxUniverse() const {return my_universe;}
	inline int dmxChannel() const {return my_dmx_channel;}
	void setRange(int min, int max);
	void setMinimum(int min);
	void setMaximum(int max);
	void setValues(int val, int refval);
	void setRefValue(int val, int colidx = -1);
	int refValue();
	inline void setChannelShown(bool state) {prop_channel_shown_f = state;}
	inline bool channelShown() const {return prop_channel_shown_f;}
	void setSelected(bool state);
	inline bool isSelected() const {return prop_selected_f;}
	void setSelectable(bool state);
	inline bool isSelectable() const {return prop_selectable_f;}
	inline QSize backgroundSize() const {return org_pix_back.size();}
	inline int backGroundWidth() const {return org_pix_back.size().width();}
	inline int backGroundHeight() const {return org_pix_back.size().height();}

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	void leaveEvent(QEvent *event);
	bool generate_scaled_knob();
	QSize minimumSizeHint() const;


public slots:
	void notifyChangedDmxChannel(int universe, int dmxchannel, int dmxvalue);
	void setRefSliderColorIndex(int colidx);

signals:
	void mixerSliderMoved(int val, int id);
	void clicked();
	void rightClicked();
	void mixerSelected(bool state, int id);

public slots:

};

#endif // MIXERCHANNEL_H
