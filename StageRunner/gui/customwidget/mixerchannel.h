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
	qint32 my_id			= -1;	///< Id of MixerChannel instance (usualy the tube number) or -1 if not used
	qint32 my_universe		= -1;	///< This is the universe that corresponds with the channel of this mixer (-1 if not used)
	qint32 my_dmx_channel	= -1;	///< This is the dmx channel that is actually the target of the mixer value (-1 if not used)
	QString label;
	DmxChannelType m_myDmxType		= DmxChannelType::DMX_GENERIC;	///< The type of the dmx channel
	DmxChannelType m_globalDmxType	= DmxChannelType::DMX_GENERIC;	///< type from global universe layout
	DmxChannel *my_dmx_channel_ref	= nullptr;						///< not realy used, only for debug


	bool prop_channel_shown_f	= false;
	bool prop_selectable_f		= false;
	bool prop_selected_f		= false;
	bool do_paint_f				= false;	///< Widget Resources ok and can be drawn
	bool knob_selected_f		= false;	///< Slider knob is clicked and selected
	bool knob_over_f			= false;	///< Mouse is over knob

	QPixmap org_pix_back;
	QPixmap org_pix_knob;
	QPixmap org_pix_knob_active;
	QPixmap org_pix_back_active;
	QRect knob_scaled_rect;			///< The position of the knob on screen
	QRect knob_rect;				///< The position of the knob on screen without shadow

	int knob_xsize			= 0;	///< Width of scaled knob without shadow border
	int knob_ysize			= 0;	///< Height of scaled knob without shadow border
	float knob_scaled_xsize	= 0;	///< Width of scaled knob including shadow border
	float knob_scaled_ysize	= 0;	///< Height of scaled knob including shadow border
	int knob_xoffset		= 0;
	int knob_yoffset		= 0;

	float lo_pos_percent	= 0;
	float hi_pos_percent	= 0;
	float pos_range_percent	= 0;

	QPoint click_position;				///< Mouseposition when clicked in Widget
	int click_value			= 0;		///< Value when mouse clicked in widget
	QSize minimum_size_hint;
	QPoint drag_begin_pos;

protected:
	QAbstractSlider refSlider;
	int refSliderColorIndex	= 0;
	int currentButton		= 0;		///< Current pressed mouse button

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
	void setDmxType(DmxChannelType type, DmxChannelType globalType);
	void setLocalDmxType(DmxChannelType type);
	DmxChannelType dmxType() const;
	bool setDmxPlusOne();
	bool setDmxMinusOne();
	void setDmxValue(int dmxVal);

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
