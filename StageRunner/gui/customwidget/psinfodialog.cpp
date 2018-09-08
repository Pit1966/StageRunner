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
