#ifndef PSINFODIALOG_H
#define PSINFODIALOG_H

#include <QDialog>

namespace Ui {
class PsInfoDialog;
}

class PsInfoDialog : public QDialog
{
	Q_OBJECT
private:
	Ui::PsInfoDialog *ui;

public:
	explicit PsInfoDialog(QWidget *parent = nullptr);
	~PsInfoDialog();
	void clear();
	void show();

protected:
	void closeEvent(QCloseEvent *);

public slots:
	void setText(const QString &text);
	void setText(const QStringList &textList);
	void appendText(const QStringList &textList);

private slots:
	void on_closeButton_clicked();
};

#endif // PSINFODIALOG_H
