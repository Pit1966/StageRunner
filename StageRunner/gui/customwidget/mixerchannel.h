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

#ifndef MIXERCHANNEL_H
#define MIXERCHANNEL_H

#include <QWidget>
#include <QAbstractSlider>
#include <QPixmap>
#include <QSize>

#include "dmxtypes.h"

class DmxChannel;

class MixerChannel : public QAbstractSlider
{
	Q_OBJECT
public:
	Q_PROPERTY(bool channelShown READ channelShown WRITE setChannelShown)
	Q_PROPERTY(bool selectable READ isSelectable WRITE setSelectable)
	Q_PROPERTY(bool selected READ isSelected WRITE setSelected)

	int myLayoutIndex;				///< Index in parent layout

private:
	int my_id;						///< Id of MixerChannel instance (usualy the tube number) or -1 if not used
	int my_universe;				///< This is the universe that corresponds with the channel of this mixer (-1 if not used)
	int my_dmx_channel;				///< This is the dmx channel that is actually the target of the mixer value (-1 if not used)
	DmxChannel *my_dmx_channel_ref;	///< not realy used, only for debug
	QString label;
	DmxChannelType my_dmx_type;		///< The type of the dmx channel

	bool prop_channel_shown_f;
	bool prop_selectable_f;
	bool prop_selected_f;

	QPixmap org_pix_back;
	QPixmap org_pix_knob;
	QPixmap org_pix_knob_active;
	QPixmap org_pix_back_active;

	int knob_xsize;					///< Width of scaled knob without shadow border
	int knob_ysize;					///< Height of scaled knob without shadow border
	float knob_scaled_xsize;		///< Width of scaled knob including shadow border
	float knob_scaled_ysize;		///< Height of scaled knob including shadow border
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
	void setDmxId(int universe, int dmx_channel, DmxChannel *dmxChP);
	void setUniverse(int universe);
	inline int dmxUniverse() const {return my_universe;}
	inline int dmxChannel() const {return my_dmx_channel;}
	void setRange(int min, int max);
	void setMinimum(int min);
	void setMaximum(int max);
	void setValues(int val, int refval);
	void setRefValue(int val, int colidx = -1);
	void emitCurrentValue();
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
	inline const QString & labelText() const {return label;}
	inline void setDmxType(DmxChannelType type) {my_dmx_type = type;}

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
	void setLabelText(const QString & text);

signals:
	void mixerSliderMoved(int val, int id);
	void clicked();
	void rightClicked();
	void mixerSelected(bool state, int id);

public slots:

};

#endif // MIXERCHANNEL_H
