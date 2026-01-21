#include "mdviewwidget.h"
#include "ui_mdviewwidget.h"

#include "configrev.h"

#include "appcontrol/appcentral.h"
#include "appcontrol/usersettings.h"

#include "thirdparty/md4d/src/md4c-html.h"

#include <QFileDialog>
#include <QFile>

MDViewWidget::MDViewWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MDViewWidget)
{
	ui->setupUi(this);
}

MDViewWidget::~MDViewWidget()
{
	delete ui;
}

void MDViewWidget::cbProcessOutput(const char *input, unsigned int input_size, void *myobj)
{
	auto* self = static_cast<MDViewWidget*>(myobj);

	self->m_viewHtmlData += QString::fromUtf8(input);

	// qDebug() << "--> size:" << input_size;
	qDebug() << input;
}

void MDViewWidget::on_fileChooserButton_clicked()
{
	UserSettings *set = AppCentral::ref().userSettings;
	m_currentFile = QFileDialog::getOpenFileName(this, tr("Choose markdown file"), set->pLastHelpDocPath);
	ui->mdFileEdit->setText(m_currentFile);
	if (m_currentFile.isEmpty())
		return;

	set->pLastHelpDocPath = m_currentFile;
}


void MDViewWidget::on_closeButton_clicked()
{
	close();
}


void MDViewWidget::on_showButton_clicked()
{
	if (!QFile::exists(m_currentFile))
		return;

	QFile file(m_currentFile);
	if (!file.open(QFile::ReadOnly))
		return;

	QByteArray mdtxt = file.readAll();
	md_html(mdtxt.constData(), mdtxt.size(), &MDViewWidget::cbProcessOutput, this, 0, 0);
	ui->mdViewEdit->setHtml(m_viewHtmlData);
}

