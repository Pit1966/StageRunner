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

#include "psinfodialog.h"
#include "ui_psinfodialog.h"

PsInfoDialog::PsInfoDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PsInfoDialog)
{
	ui->setupUi(this);
}

PsInfoDialog::~PsInfoDialog()
{
	delete ui;
}

void PsInfoDialog::clear()
{
	ui->infoEdit->clear();
}

void PsInfoDialog::show()
{
	setAttribute(Qt::WA_DeleteOnClose);
	QDialog::show();
}

void PsInfoDialog::closeEvent(QCloseEvent *)
{
	this->accept();
}

void PsInfoDialog::setText(const QString &text)
{
	if (Qt::mightBeRichText(text)) {
		QString richtext = text;
		richtext.replace("\n","<br>");
		QStringList lines = richtext.split("<br>");
		setText(lines);
	} else {
		ui->infoEdit->setPlainText(text);
	}
}

void PsInfoDialog::setText(const QStringList &textList)
{
	ui->infoEdit->clear();
	appendText(textList);
}

void PsInfoDialog::appendText(const QStringList &textList)
{
	for (int i=0; i<textList.size(); i++) {
		ui->infoEdit->appendHtml(textList.at(i));
	}
}

void PsInfoDialog::on_closeButton_clicked()
{
	this->accept();
}
