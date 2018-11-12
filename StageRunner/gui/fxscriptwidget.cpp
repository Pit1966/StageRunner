#include "fxscriptwidget.h"
#include "log.h"
#include "fxscriptitem.h"


QList<FxScriptWidget*>FxScriptWidget::m_scriptWidgetList;

FxScriptWidget *FxScriptWidget::openFxScriptPanel(FxScriptItem *fx, QWidget *parent)
{
	foreach(FxScriptWidget *scrwid, m_scriptWidgetList) {
		if (scrwid->m_OriginFxScript == fx) {
			scrwid->setWindowState(scrwid->windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
			scrwid->raise();
			scrwid->show();
			return 0;
		}
	}

	FxScriptWidget *scriptwid = new FxScriptWidget(fx,parent);
	m_scriptWidgetList.append( scriptwid );

	scriptwid->setWindowIcon(QPixmap(":/gfx/icons/script.png"));
	return scriptwid;
}

void FxScriptWidget::destroyAllScriptPanels()
{
	while (!m_scriptWidgetList.isEmpty())
		delete m_scriptWidgetList.takeFirst();
}

FxScriptWidget *FxScriptWidget::findParentFxScriptWidget(FxItem *fx)
{
	foreach(FxScriptWidget *scrwid, m_scriptWidgetList) {
		if (scrwid->m_OriginFxScript == fx)
			return scrwid;
	}

	return 0;
}

bool FxScriptWidget::setFxScriptContent(FxScriptItem *fxscr)
{
	if (!fxscr)
		return false;

	QString script = fxscr->m_scriptRaw;
	script.replace(';',"\n");

	scriptEdit->setPlainText(script);

	return true;
}

FxScriptWidget::FxScriptWidget(FxScriptItem *fxscr, QWidget *parent)
	: QWidget(parent)
	, m_OriginFxScript(fxscr)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	scriptEdit->addKeywords(FxScriptLine::keywords.keywordList());

	setFxScriptContent(fxscr);
}

FxScriptWidget::~FxScriptWidget()
{
	m_scriptWidgetList.removeOne(this);
	DEBUGTEXT("Script Panel destroyed");
}

bool FxScriptWidget::applyChanges()
{
	if (!m_OriginFxScript)
		return false;

	QString new_script = scriptEdit->toPlainText();
	new_script.replace("\n",";");
	if (new_script != m_OriginFxScript->m_scriptRaw) {
		m_OriginFxScript->m_scriptRaw = new_script;
		m_OriginFxScript->setModified(true);
		emit modified();
		return true;
	}

	return false;
}

void FxScriptWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void FxScriptWidget::on_cancelButton_clicked()
{
	close();
}

void FxScriptWidget::on_applyButton_clicked()
{
	applyChanges();
}

void FxScriptWidget::on_applyCloseButton_clicked()
{
	applyChanges();
	close();
}
