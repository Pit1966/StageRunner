//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include "ui_messagedialog.h"

namespace Ps
{
enum Button {
	NONE,
	CONTINUE,
	CANCEL,
	YES,
	NO,

	AUTO = 0x10000
};
}


class MessageDialog : public QDialog, private Ui::MessageDialog
{
	Q_OBJECT
private:
	QString main_text;
	QString sub_text;

	QObject *specialFunctionObj;
	QString specialFunction;

public:
	Ps::Button pressedButton;
	bool doNotShowAgain;

public:
	MessageDialog(QWidget *parent = 0);
	~MessageDialog();
	void setMainText(const QString &text);
	void setSubText(const QString &text);
	Ps::Button execMessage(const QString &mainText, const QString &subText);
	void showMessage(const QString &mainText, const QString &subText);
	void showInformation(const QString &mainText, const QString &subText);
	void connectSpecialFunction(QObject *target, const QString &func);


private:
	void update_text();

private slots:
	void on_doNotShowAgainCheck_clicked(bool checked);
	void on_cancelButton_clicked();
	void on_noButton_clicked();
	void on_yesButton_clicked();
	void on_continueButton_clicked();

	void on_specialFunctionButton_clicked();

signals:
	void specialFunctionClicked();
};

#endif // MESSAGEDIALOG_H
