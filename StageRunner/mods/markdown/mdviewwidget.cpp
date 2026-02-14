#include "mdviewwidget.h"
#include "ui_mdviewwidget.h"

#include "configrev.h"

#include "appcontrol/appcentral.h"
#include "appcontrol/usersettings.h"

#include "thirdparty/md4d/src/md4c-html.h"

#include <QFileDialog>
#include <QFile>
#include <QScrollBar>

MDViewWidget::MDViewWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MDViewWidget)
{
	ui->setupUi(this);

	ui->mdView->setOpenExternalLinks(true);

	// QPalette pal = ui->mdView->palette();
	// pal.setColor(QPalette::Link, QColor("#2685b4"));
	// ui->mdView->setPalette(pal);

}

MDViewWidget::~MDViewWidget()
{
	delete ui;
}

void MDViewWidget::cbProcessOutput(const char *input, unsigned int input_size, void *myobj)
{
	auto* self = static_cast<MDViewWidget*>(myobj);

	self->m_viewHtmlData += QString::fromUtf8(input, input_size);
}

void MDViewWidget::renderMD(bool keepScrollAnchor)
{
	md_html(m_mdData.constData(), m_mdData.size(), &MDViewWidget::cbProcessOutput, this, MD_DIALECT_GITHUB, 0);


	QScrollBar* sb = ui->mdView->verticalScrollBar();
	int scrollvalue = sb->value();
	bool wasAtBottom = (sb->value() >= sb->maximum() - 2);

	QString style = "a {color: #2685b4;}";
	ui->mdView->document()->setDefaultStyleSheet(style);
	ui->mdView->setHtml(m_viewHtmlData);

	if (keepScrollAnchor) {
		QMetaObject::invokeMethod(sb, [sb, scrollvalue, wasAtBottom]() {
			if (wasAtBottom) {
				sb->setValue(sb->maximum());
			} else {
				sb->setValue(scrollvalue);
			}
		}, Qt::QueuedConnection);
	}
}

void MDViewWidget::on_fileChooserButton_clicked()
{
	UserSettings *set = AppCentral::ref().userSettings;
	m_currentFile = QFileDialog::getOpenFileName(this, tr("Choose markdown file"), set->pLastHelpDocPath);
	ui->mdFileEdit->setText(m_currentFile);
	if (m_currentFile.isEmpty())
		return;

	m_mdParseActive = false;

	set->pLastHelpDocPath = m_currentFile;

	if (!QFile::exists(m_currentFile))
		return;

	QFile file(m_currentFile);
	if (!file.open(QFile::ReadOnly))
		return;

	m_mdData = file.readAll();
	ui->mdEdit->setPlainText(QString::fromUtf8(m_mdData));
	m_mdParseActive = true;
}


void MDViewWidget::on_closeButton_clicked()
{
	close();
}


void MDViewWidget::on_showButton_clicked()
{
	renderMD();
}


void MDViewWidget::on_mdEdit_textChanged()
{
	if (m_mdParseActive) {
		m_mdData = ui->mdEdit->toPlainText().toUtf8();
		m_viewHtmlData.clear();
		renderMD(true);
	}
}

