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
