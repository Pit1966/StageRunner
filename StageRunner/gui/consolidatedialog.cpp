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

	if (!m_project->consolidateToDir(targetProjectNameEdit->text(), targetPathEdit->text(), this)) {

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
