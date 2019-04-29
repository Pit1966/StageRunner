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

#include "consolidatedialog.h"
#include "project.h"
#include "appcentral.h"
#include "usersettings.h"

#include <QFileDialog>
#include <QDir>
#include <QApplication>

ConsolidateDialog::ConsolidateDialog(Project *project, QWidget *parent)
	: QDialog(parent)
	, m_project(project)
{
	setupUi(this);

	connect(m_project, SIGNAL(progressWanted(int,int)), progressBar, SLOT(setRange(int,int)));
	connect(m_project, SIGNAL(progressed(int)), this, SLOT(setProgress(int)));
	progressBar->hide();
	progressBar->reset();

	QString proname = m_project->projectName() + " (consolidated)";

	targetProjectNameEdit->setText(proname);
	targetPathEdit->setText(QDir::homePath());

	QString lastpath = AppCentral::ref().userSettings->pLastCondolidatePath;
	if (!lastpath.isEmpty())
		targetPathEdit->setText(lastpath);

}

void ConsolidateDialog::setExportProjectName(const QString &name)
{
	targetProjectNameEdit->setText(name);
}

QString ConsolidateDialog::exportProjectName() const
{
	return targetProjectNameEdit->text();
}

void ConsolidateDialog::setExportDir(const QString &dir)
{
	targetPathEdit->setText(dir);
}

QString ConsolidateDialog::exportDir() const
{
	return targetPathEdit->text();
}

void ConsolidateDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void ConsolidateDialog::on_targetDirButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(
				this,
				tr("Choose Consolidate folder"),
				targetPathEdit->text());

	if (!path.isEmpty())
		targetPathEdit->setText(path);

}

void ConsolidateDialog::on_cancelButton_clicked()
{
	reject();
}

void ConsolidateDialog::on_okButton_clicked()
{
	progressBar->show();

	// this will be given to export subroutines and stores export summary and messages
	Project::EXPORT_RESULT result;
	result.forceKeepFxItemIDs = keepFxIDsCheck->isChecked();
	result.exportWithRelativeFxFilePaths = true;

	if (!m_project->consolidateToDir(targetProjectNameEdit->text(), targetPathEdit->text(), result, this)) {

	} else {
		AppCentral::ref().userSettings->pLastCondolidatePath = targetPathEdit->text();
		accept();
	}

	progressBar->hide();
}

void ConsolidateDialog::setProgress(int val)
{
	progressBar->setValue(val);
	QApplication::processEvents();
}
