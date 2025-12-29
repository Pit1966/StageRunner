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

#ifndef CONSOLIDATEDIALOG_H
#define CONSOLIDATEDIALOG_H

#include "ui_consolidatedialog.h"

class Project;

class ConsolidateDialog : public QDialog, private Ui::ConsolidateDialog
{
	Q_OBJECT
private:
	Project *m_project;

public:
	explicit ConsolidateDialog(Project *project, QWidget *parent = nullptr);
	void setExportProjectName(const QString &name);
	QString exportProjectName() const;
	void setExportDir(const QString &dir);
	QString exportDir() const;

protected:
	void changeEvent(QEvent *e);

private slots:
	void on_targetDirButton_clicked();
	void on_cancelButton_clicked();
	void on_okButton_clicked();
	void setProgress(int val);

};

#endif // CONSOLIDATEDIALOG_H
