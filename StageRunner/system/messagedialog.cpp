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

#include "messagedialog.h"

using namespace Ps;

MessageDialog::MessageDialog(QWidget *parent) :
	QDialog(parent)
{
	doNotShowAgain = false;
	pressedButton = NONE;

	specialFunctionObj = 0;

	setupUi(this);

	noButton->hide();
	yesButton->hide();
	specialFunctionButton->hide();

}

MessageDialog::~MessageDialog()
{
}

void MessageDialog::setMainText(const QString &text)
{
	main_text = text;
	update_text();
}

void MessageDialog::setSubText(const QString &text)
{
	sub_text = text;
	update_text();
}

Ps::Button MessageDialog::execMessage(const QString &mainText, const QString &subText)
{
	main_text = mainText;
	sub_text = subText;
	update_text();

	exec();

	return pressedButton;
}

void MessageDialog::showMessage(const QString &mainText, const QString &subText)
{
	main_text = mainText;
	sub_text = subText;
	update_text();

	setAttribute(Qt::WA_DeleteOnClose);

	show();
}

void MessageDialog::showInformation(const QString &mainText, const QString &subText)
{
	cancelButton->hide();
	showMessage(mainText, subText);
}

void MessageDialog::connectSpecialFunction(QObject *target, const QString &func)
{
	specialFunctionObj = target;
	specialFunction = func;
	specialFunctionButton->show();
}

void MessageDialog::update_text()
{
	textEdit->clear();
	textEdit->setTextColor(textEdit->palette().color(QPalette::HighlightedText));
	textEdit->insertPlainText(main_text);
	textEdit->insertPlainText("\n\n");
	textEdit->setTextColor(textEdit->palette().color(QPalette::WindowText));
	textEdit->insertPlainText(sub_text);
}


void MessageDialog::on_doNotShowAgainCheck_clicked(bool checked)
{
	doNotShowAgain = checked;
}

void MessageDialog::on_cancelButton_clicked()
{
	pressedButton = CANCEL;
	reject();
}

void MessageDialog::on_noButton_clicked()
{
	pressedButton = NO;
	reject();
}

void MessageDialog::on_yesButton_clicked()
{
	pressedButton = YES;
	accept();
}

void MessageDialog::on_continueButton_clicked()
{
	pressedButton = CONTINUE;
	accept();
}

void MessageDialog::on_specialFunctionButton_clicked()
{
	if (!specialFunctionObj) return;


	QMetaObject::invokeMethod(specialFunctionObj,specialFunction.toLocal8Bit().data());
	accept();

}
