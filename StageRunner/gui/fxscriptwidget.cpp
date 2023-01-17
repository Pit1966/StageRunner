//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

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
	scriptEdit->moveCursor(QTextCursor::End);
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

		// check some possible script issues such as long wait time
		QString checkResult = FxScriptItem::checkScript(new_script);
		if (!checkResult.isEmpty()) {
			checkResult = tr("Possible error sources discovered in script:\n\n%1").arg(checkResult);
			POPUPINFOMSG("Script check", checkResult, this);
		}

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
