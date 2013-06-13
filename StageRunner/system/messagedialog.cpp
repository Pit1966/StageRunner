#include "messagedialog.h"

using namespace Ps;

MessageDialog::MessageDialog(QWidget *parent) :
	QDialog(parent)
{
	doNotShowAgain = false;
	pressedButton = NONE;

	setupUi(this);

	noButton->hide();
	yesButton->hide();

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
