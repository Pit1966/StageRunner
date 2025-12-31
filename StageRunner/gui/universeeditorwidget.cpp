#include "universeeditorwidget.h"
#include "configrev.h"
#include "log.h"
#include "system/dmx/fixture.h"
#include "deviceinfowidget.h"
#include "fx/fxitem.h"
#include "fx/fxsceneitem.h"
#include "dmxchannel.h"

#include "appcentral.h"
#include "fxlist.h"
#include "fxlistvarset.h"

#include <QSettings>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QMessageBox>

#define COL_DEVICE 0
#define COL_SHORT_IDENT 1
#define COL_DMX 2
#define COL_CHANNEL_MODE 3
#define COL_CHANNEL_DESC 4

// -------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------

DMXTableWidgetItem::DMXTableWidgetItem(int devIdx, int dmxChan, const QString &text, int type)
	: QTableWidgetItem(text, type)
	, devIdx(devIdx)
	, dmxChan(dmxChan)
{
	setData(Qt::UserRole + 1, devIdx);
	setData(Qt::UserRole + 2, dmxChan);
}

// -------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------


UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	for (int i=0; i<MAX_DMX_UNIVERSE; i++) {
		m_universeLayouts[i] = new SR_FixtureList();
	}

	m_fixtureList = m_universeLayouts[0];

	universeTable->setRowCount(512);
	universeTable->setSelectionBehavior(QTableWidget::SelectRows);
	universeTable->setItemDelegate(new DmxItemBgDelegate(universeTable));
	copyFixturesToGui(m_fixtureList);

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("UniverseEditor");
	m_lastFixturePath = set.value("LastFixturePath").toString();
}

UniverseEditorWidget::~UniverseEditorWidget()
{
	for (int i=0; i<MAX_DMX_UNIVERSE; i++) {
		delete m_universeLayouts[i];
	}
}

bool UniverseEditorWidget::saveToFilesystem(const QString &path, bool saveCurrent)
{
	bool ok = false;

	if (saveCurrent) {
		QJsonObject json = m_fixtureList->toJson();
		json["universeNum"] = m_universe + 1;
		QByteArray jsonstr = QJsonDocument(json).toJson(QJsonDocument::Indented);
		QFile file(path);
		if (file.open(QFile::WriteOnly)) {
			file.write(jsonstr);
			ok = true;
		}
	}
	else {
		QJsonObject jsonAll;
		QJsonArray universes;
		for (int i=0; i<MAX_DMX_UNIVERSE; i++) {
			QJsonObject jsonOneUniv = m_universeLayouts[i]->toJson();
			universes.append(jsonOneUniv);
		}
		jsonAll["universeLayouts"] = universes;
		jsonAll["universeCount"] = MAX_DMX_UNIVERSE;

		QByteArray jsonstr = QJsonDocument(jsonAll).toJson(QJsonDocument::Indented);
		QFile file(path);
		if (file.open(QFile::WriteOnly)) {
			file.write(jsonstr);
			ok = true;
		}
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
	if (error.error)
		return false;

	if (json.contains("universeCount")) { // multi universe file
		int count = json["universeCount"].toInt();
		if (count > MAX_DMX_UNIVERSE)
			count = MAX_DMX_UNIVERSE;
		// get universe list
		QJsonArray universes = json["universeLayouts"].toArray();
		for (int i=0; i<count; i++) {
			QJsonObject jsonOneUniv = universes.at(i).toObject();
			m_universeLayouts[i]->setFromJson(jsonOneUniv);
		}
	}
	else { // single universe file
		// load to current active fixture list
		m_fixtureList->setFromJson(json);
	}

	return true;
}

FxSceneItem *UniverseEditorWidget::createSceneFromFixtureList(SR_FixtureList *fixList, uint universe)
{
	int tubes = fixList->lastUsedDmxAddr();
	if (tubes == 0)
		return nullptr;

	FxSceneItem *sc = new FxSceneItem();
	sc->createDefaultTubes(tubes, universe);

	// Loop over fixtures and add them to scene
	int dmx = 0;
	for (int t=0; t<fixList->size(); t++) {
		SR_Fixture *fix = fixList->at(t);
		SR_Mode *mode = fix->mode(fix->currentMode());
		int c = 0;
		for (SR_Channel *chan : std::as_const(mode->channels())) {
			if (c++ == 0) {
				if (fix->dmxAdr()-1 > dmx)
					dmx = fix->dmxAdr()-1;
			}
			DmxChannel *tube = sc->tube(dmx);
			SR_Channel::Preset type = SR_Channel::stringToPreset(chan->preset());
			if (type >= SR_Channel::Custom /*DMX_GENERIC*/)
				tube->dmxType = int(type);

			// device info
			tube->deviceUniverseIndex = (universe<<16) + (t+1);
			tube->deviceShortId = fix->shortIdent();

			// qDebug() << "target dmx" << dmx << "type" << chan->preset() << type << "deviveUniverseIdx" << tube->deviceUniverseIndex << tube->deviceShortId;

			dmx++;
		}
	}

	return sc;
}

bool UniverseEditorWidget::copyFixturesToGui(SR_FixtureList *fixlist)
{
	universeTable->blockSignals(true);
	universeTable->clearContents();

	int dmx = 1;			// this is real DMX address base 1
	for (int t=0; t<fixlist->size(); t++) {
		SR_Fixture *fix = fixlist->at(t);
		SR_Mode *mode = fix->mode(fix->currentMode());
		if (!mode)
			continue;

		// QColor bg;
		// if (t % 2) {
		// 	bg = palette().alternateBase().color();
		// } else {
		// 	bg = palette().base().color();
		// }

		int c = 0;
		for (auto chan : mode->channels()) {
			if (c++ == 0) {
				if (fix->dmxAdr() > dmx)
					dmx = fix->dmxAdr();

				QString device = QString("%1: %2").arg(fix->manufacturer(), fix->modelName());
				DMXTableWidgetItem *item = new DMXTableWidgetItem(t, dmx, device);
				universeTable->setItem(dmx - 1, COL_DEVICE, item);
			}
			else {
				DMXTableWidgetItem *item = new DMXTableWidgetItem(t, dmx, QString());
				universeTable->setItem(dmx - 1, COL_DEVICE, item);
			}

			// short ident
			DMXTableWidgetItem *item = new DMXTableWidgetItem(t, dmx, QString("%1").arg(fix->shortIdent()));
			// item->setTextAlignment(Qt::AlignCenter);
			universeTable->setItem(dmx - 1, COL_SHORT_IDENT, item);

			// dmx channel
			item = new DMXTableWidgetItem(t, dmx, QString("%1").arg(dmx,3,10,QLatin1Char('0')));
			item->setTextAlignment(Qt::AlignCenter);
			universeTable->setItem(dmx - 1, COL_DMX, item);

			// channel type
			item = new DMXTableWidgetItem(t, dmx, QString("%1").arg(chan->name()));
			// item->setTextAlignment(Qt::AlignCenter);
			universeTable->setItem(dmx - 1, COL_CHANNEL_MODE, item);

			// description
			item = new DMXTableWidgetItem(t, dmx, QString("%1").arg(chan->preset()));
			// item->setTextAlignment(Qt::AlignCenter);
			universeTable->setItem(dmx - 1, COL_CHANNEL_DESC, item);

			dmx++;
		}
	}

	// fill channels
	dmx = 1;
	while (dmx <= 512) {
		if (!universeTable->item(dmx - 1, COL_DMX)) {
			DMXTableWidgetItem *item = new DMXTableWidgetItem(-1, dmx, QString("%1").arg(dmx,3,10,QLatin1Char('0')));
			item->setTextAlignment(Qt::AlignCenter);
			item->setForeground(Qt::green);
			universeTable->setItem(dmx - 1, COL_DMX, item);
		}
		dmx++;
	}

	// universeTable->resizeColumnsToContents();
	universeTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	m_currentTargetDmxAddr[m_universe] = 0;

	universeTable->blockSignals(false);
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
	set.beginGroup("UniverseEditor");
	set.setValue("LastFixturePath", m_lastFixturePath);
	set.endGroup();
}

void UniverseEditorWidget::on_addDeviceButton_clicked()
{
	if (deviceInfoWidget->isValid()) {
		SR_Fixture *newfix = new SR_Fixture(*deviceInfoWidget->device());
		newfix->setCurrentMode(deviceInfoWidget->modeNumber());
		m_fixtureList->addFixture(newfix, m_currentTargetDmxAddr[m_universe]);
		copyFixturesToGui(m_fixtureList);
	}
}

void UniverseEditorWidget::on_removeDeviceButton_clicked()
{
	if (m_currentTargetDmxAddr[m_universe] <= 0)
		return;

	qDebug() << "dmx addr" << m_currentTargetDmxAddr[m_universe];

	m_fixtureList->removeFixtureAt(m_currentTargetDmxAddr[m_universe]);
	copyFixturesToGui(m_fixtureList);
}


void UniverseEditorWidget::on_pushButton_saveLayout_clicked()
{
	QString defPath = AppCentral::ref().defaultFixtureConfigPath();
	bool ok = saveToFilesystem(defPath);
	if (ok)
		POPUPINFOMSG(Q_FUNC_INFO, "Default DMX layout saved!");
}

void UniverseEditorWidget::on_pushButton_loadLayout_clicked()
{
	QString defPath = AppCentral::ref().defaultFixtureConfigPath();
	bool ok = loadFromFilesystem(defPath);
	Q_UNUSED(ok)

	copyFixturesToGui(m_fixtureList);
}

void UniverseEditorWidget::on_pushButton_createTemplate_clicked()
{
	FxSceneItem *sc = createSceneFromFixtureList(m_fixtureList, m_universe);
	if (!sc) return;

	sc->setName(QString("Default_Universe_%1").arg(m_universe));
	sc->generateNewID(11000);
	sc->setSubId(m_universe + 1);

	FxList *templateList = AppCentral::ref().templateFxList->fxList();

	// remove existing scene from template list
	FxItem *oldfx = templateList->findFxItemBySubId(1);
	templateList->removeFx(oldfx);

	templateList->addFx(sc);
	templateList->emitListChangedSignal();

	POPUPINFOMSG("Create default universe",
				 tr("Layout template: %1 <font color=green>successfully created.</font>\n"
					"You have to save the template scenes in order to make this persistant")
				 .arg(sc->name()));
}

void UniverseEditorWidget::on_universeTable_cellClicked(int row, int column)
{
	qDebug() << Q_FUNC_INFO << row << column;
	m_currentTargetDmxAddr[m_universe] = row +1;
}

void UniverseEditorWidget::on_universeTable_cellChanged(int row, int column)
{
	Q_UNUSED(row)
	Q_UNUSED(column)

	// qDebug() << "cell changed" << row << column;

	SR_Fixture *fix = m_fixtureList->findFixtureByDmxAddr(row+1);
	if (!fix) return;

	DMXTableWidgetItem *item = static_cast<DMXTableWidgetItem*>(universeTable->item(row, column));

	qDebug() << "found fixture dmx offset" << fix->dmxStartAddr() << "short ident" << fix->shortIdent();

	if (column == COL_SHORT_IDENT) {
		// set short ident in fixture
		QString shortid = item->text();
		fix->setShortIdent(shortid);

		// set short ident in Table for every channel of the fixture
		universeTable->blockSignals(true);
		int dmx = fix->dmxStartAddr();
		while (dmx <= fix->dmxEndAddr()) {
			universeTable->item(dmx-1, COL_SHORT_IDENT)->setText(shortid);
			dmx++;
		}

		universeTable->blockSignals(false);
	}
}

void UniverseEditorWidget::on_universeSpin_valueChanged(int arg1)
{
	int univ = arg1-1;
	if (m_universe != univ) {
		m_universe = univ;
		m_fixtureList = m_universeLayouts[univ];
		copyFixturesToGui(m_fixtureList);
	}
}

void UniverseEditorWidget::on_saveAsButton_clicked()
{
	QString currentDir = QDir::homePath();

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("UniverseEditor");
	if (set.contains("LastUniverseSavePath"))
		currentDir = set.value("LastUniverseSavePath").toString();

	QString filepath = QFileDialog::getSaveFileName(this, tr("Save as ..."), currentDir);
	if (filepath.isEmpty())
		return;

	if (!filepath.endsWith(".srdmx", Qt::CaseInsensitive))
		filepath.append(".srdmx");

	if (saveToFilesystem(filepath, true)) {
		QMessageBox::information(this, tr("Info"), tr("Layout of universe %1\nsuccessfully saved!").arg(m_universe + 1));
		set.setValue("LastUniverseSavePath", filepath);;
	}
}


void UniverseEditorWidget::on_loadButton_clicked()
{
	QString currentDir = QDir::homePath();

	QSettings set(QSETFORMAT,APPNAME);
	set.beginGroup("UniverseEditor");
	if (set.contains("LastUniverseSavePath"))
		currentDir = set.value("LastUniverseSavePath").toString();

	if (set.contains("LastUniverseLoadPath"))
		currentDir = set.value("LastUniverseLoadPath").toString();

	QString filepath = QFileDialog::getOpenFileName(this, "Load universe to current", currentDir);
	if (filepath.isEmpty())
		return;

	if (!loadFromFilesystem(filepath)) {
		QMessageBox::critical(this, tr("System error"), tr("Could not load json universe config from %1").arg(filepath));
	}
	else {
		copyFixturesToGui(m_fixtureList);
	}
}



