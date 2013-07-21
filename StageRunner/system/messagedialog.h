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
