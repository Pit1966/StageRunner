#include "universeeditorwidget.h"
#include "config.h"
#include "log.h"
#include "system/dmx/fixture.h"
#include "deviceinfowidget.h"
#include "fxsceneitem.h"
#include "dmxchannel.h"

#include "appcentral.h"
#include "fxlist.h"
#include "fxlistvarset.h"

#include <QSettings>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>


#define COL_DEVICE 0
#define COL_DMX 1
#define COL_CHANNEL_MODE 2
#define COL_CHANNEL_DESC 3


DMXTableWidgetItem::DMXTableWidgetItem(int type)
	: QTableWidgetItem(type)
{

}

DMXTableWidgetItem::DMXTableWidgetItem(int dmxChan, const QString &text, int type)
	: QTableWidgetItem(text, type)
	, dmxChan(dmxChan)
{

}


UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
	: QWidget(parent)
	, m_fixtureList(new SR_FixtureList())
{
	setupUi(this);

	universeTable->setRowCount(512);
	copyFixturesToGui(m_fixtureList);

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("UserSettings");
	m_lastFixturePath = set.value("LastFixturePath").toString();

}

UniverseEditorWidget::~UniverseEditorWidget()
{
	delete m_fixtureList;
}

bool UniverseEditorWidget::saveToFilesystem(const QString &path)
{
	bool ok = false;

	QJsonObject json = m_fixtureList->toJson();
	QByteArray jsonstr = QJsonDocument(json).toJson(QJsonDocument::Indented);
	QFile file(path);
	if (file.open(QFile::WriteOnly)) {
		file.write(jsonstr);
		ok = true;
	}

	return ok;
}

bool UniverseEditorWidget::loadFromFilesystem(const QString &path)
{
	QFile file(path);
	if (!file.open(QFile::ReadOnly))
		return false;

	QByteArray dat = file.readAll();

	QJsonParseError error;
	QJsonObject json = QJsonDocument::fromJson(dat, &error).object();
	m_fixtureList->setFromJson(json);

	return true;
}

QString UniverseEditorWidget::defaultFilepath()
{
	QString defaultpath = QString("%1/.config/%2/%3.dmx.default")
			.arg(QDir::homePath())
			.arg(APP_CONFIG_PATH)
			.arg(APPNAME);

	return defaultpath;
}

FxSceneItem *UniverseEditorWidget::createSceneFromFixtureList(SR_FixtureList *fixList)
{
	FxSceneItem *sc = new FxSceneItem();
	int tubes = fixList->lastUsedDmxAddr();
	if (tubes == 0)
		return nullptr;

	sc->createDefaultTubes(tubes);

	// Loop over fixtures and add them to scene
	int dmx = 0;
	for (int t=0; t<fixList->size(); t++) {
		SR_Fixture *fix = fixList->at(t);
		SR_Mode *mode = fix->mode(fix->currentMode());
		int c = 0;
		for (SR_Channel *chan : mode->channels()) {
			if (c++ == 0) {
				if (fix->dmxAdr()-1 > dmx)
					dmx = fix->dmxAdr()-1;
			}
			DmxChannel *tube = sc->tube(dmx);
			SR_Channel::Preset type = SR_Channel::stringToPreset(chan->preset());
			qDebug() << "type" << chan->preset() << type;
			if (type >= DMX_GENERIC)
				tube->dmxType = int(type);

			dmx++;
		}
	}


	return sc;
}

bool UniverseEditorWidget::copyFixturesToGui(SR_FixtureList *fixlist)
{
	universeTable->clearContents();

	int dmx = 0;
	for (int t=0; t<fixlist->size(); t++) {
		SR_Fixture *fix = fixlist->at(t);
		SR_Mode *mode = fix->mode(fix->currentMode());
		int c = 0;
		for (auto chan : mode->channels()) {
			if (c++ == 0) {
				if (fix->dmxAdr()-1 > dmx)
					dmx = fix->dmxAdr()-1;

				QString device = QString("%1: %2").arg(fix->manufacturer(), fix->modelName());
				DMXTableWidgetItem *item = new DMXTableWidgetItem(dmx, device);
				universeTable->setItem(dmx, COL_DEVICE, item);
			}


//			DMXTableWidgetItem *item = new DMXTableWidgetItem(dmx, QString("%1").arg(dmx+1,3,10,QLatin1Char('0')));
//			item->setTextAlignment(Qt::AlignCenter);
//			universeTable->setItem(dmx, COL_DMX, item);

			DMXTableWidgetItem *item = new DMXTableWidgetItem(dmx, QString("%1").arg(chan->name()));
			// item->setTextAlignment(Qt::AlignCenter);
			universeTable->setItem(dmx, COL_CHANNEL_MODE, item);

			item = new DMXTableWidgetItem(dmx, QString("%1").arg(chan->preset()));
			// item->setTextAlignment(Qt::AlignCenter);
			universeTable->setItem(dmx, COL_CHANNEL_DESC, item);

			dmx++;
		}
	}

	// fill channels
	dmx = 0;
	while (dmx < 512) {
		DMXTableWidgetItem *item = new DMXTableWidgetItem(dmx, QString("%1").arg(dmx+1,3,10,QLatin1Char('0')));
		item->setTextAlignment(Qt::AlignCenter);
		universeTable->setItem(dmx, COL_DMX, item);
		dmx++;
	}

	universeTable->resizeColumnsToContents();
	m_currentTargetDmxAddr = 0;

	return true;
}

void UniverseEditorWidget::on_pushButton_close_clicked()
{
	close();
}

void UniverseEditorWidget::on_selectDeviceButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open a device definition"), m_lastFixturePath);
	if (path.isEmpty())
		return;

	if (!deviceInfoWidget->setDeviceDefinition(path))
		POPUPERRORMSG(__func__, "Could not set fixture");
	else
		m_lastFixturePath = path;

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("UserSettings");
	set.setValue("LastFixturePath", m_lastFixturePath);
	set.endGroup();
}

void UniverseEditorWidget::on_addDeviceButton_clicked()
{
	if (deviceInfoWidget->isValid()) {
		SR_Fixture *newfix = new SR_Fixture(*deviceInfoWidget->device());
		newfix->setCurrentMode(deviceInfoWidget->modeNumber());
		m_fixtureList->addFixture(newfix, m_currentTargetDmxAddr);
		copyFixturesToGui(m_fixtureList);
	}
}

void UniverseEditorWidget::on_removeDeviceButton_clicked()
{
}


void UniverseEditorWidget::on_pushButton_saveLayout_clicked()
{
	bool ok = saveToFilesystem(defaultFilepath());
	if (ok)
		POPUPINFOMSG(Q_FUNC_INFO, "Default DMX layout saved!");
}

void UniverseEditorWidget::on_pushButton_loadLayout_clicked()
{
	bool ok = loadFromFilesystem(defaultFilepath());

	copyFixturesToGui(m_fixtureList);
}

void UniverseEditorWidget::on_pushButton_createTemplate_clicked()
{
	FxSceneItem *sc = createSceneFromFixtureList(m_fixtureList);
	if (!sc) return;

	sc->setName("Default_Universe_0");
	sc->generateNewID(11000);
	AppCentral::ref().templateFxList->fxList()->addFx(sc);
	AppCentral::ref().templateFxList->fxList()->emitListChangedSignal();
}

void UniverseEditorWidget::on_universeTable_cellClicked(int row, int column)
{
	qDebug() << Q_FUNC_INFO << row << column;
	m_currentTargetDmxAddr = row +1;
}

void UniverseEditorWidget::on_universeTable_cellChanged(int row, int column)
{
	Q_UNUSED(row)
	Q_UNUSED(column)
}

void UniverseEditorWidget::on_universeTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	qDebug() << Q_FUNC_INFO << currentRow << currentColumn;
}

