#include "universeeditorwidget.h"
#include "log.h"
#include "system/dmx/fixture.h"

UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
	: QWidget(parent)
	, m_fixtureList(new SR_FixtureList())
{
	setupUi(this);
}

UniverseEditorWidget::~UniverseEditorWidget()
{
	delete m_fixtureList;
}

bool UniverseEditorWidget::copyFixturesToGui(SR_FixtureList *fixlist)
{
	universeTable->clear();

	for (int t=0; t<fixlist->size(); t++) {

	}
}

void UniverseEditorWidget::on_pushButton_addFixture_clicked()
{
	if (!m_fixtureList->addQLCFixture("/home/peter/.qlcplus/fixtures/Peter_Stairville-LED-PAR36.qxf"))
		POPUPERRORMSG(__func__, "Could not add fixture");
	copyFixturesToGui(m_fixtureList);
}

void UniverseEditorWidget::on_pushButton_removeFixture_clicked()
{

}

void UniverseEditorWidget::on_pushButton_close_clicked()
{
	close();
}
