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
