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

#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

#include "varset.h"
#include "varsetlist.h"
#include "messagedialog.h"

#include <QObject>
#include <QString>


class VMsg : public VarSet
{
public:
	enum Type {
		MT_UNDEF
	};


protected:
	Type type;
	QString messageText;
	bool doNotShowAgain;
	Ps::Button doNotShowDefaultButton;

public:
	VMsg();

private:
	void init();

	friend class MessageHub;
};


class MessageHub : public VarSet
{
	Q_OBJECT
private:
	static MessageHub *my_instance;
	VarSetList<VMsg*>msglist;

public:
	static MessageHub * instance();
	static void destroy();
	static Ps::Button question(const QString &mainText, const QString &subText, VMsg::Type msgType = VMsg::MT_UNDEF);

	Ps::Button showQuestion(const QString &mainText, const QString &subText, VMsg::Type msgType = VMsg::MT_UNDEF);

private:
	MessageHub();
	~MessageHub();
	void init();
	VMsg *find_message(const QString &text, VMsg::Type msgType);

signals:

public slots:

};

#endif // MESSAGEHUB_H
