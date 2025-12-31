#include "fixturelayout.h"

#include "appcontrol/appcentral.h"
#include "system/lightcontrol.h"
#include "system/dmx/fixture.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

FixtureLayout::FixtureLayout(AppCentral &appcentral, LightControl &unitlight)
	: QObject()
	, myApp(appcentral)
	, unitLight(unitlight)
{
	for (int i=0; i<MAX_DMX_UNIVERSE; i++) {
		m_layouts[i] = new SR_FixtureList();
	}
}

FixtureLayout::~FixtureLayout()
{
	for (int i=0; i<MAX_DMX_UNIVERSE; i++) {
		delete m_layouts[i];
	}
}

bool FixtureLayout::loadFixtureLayouts(const QString &filePath)
{
	QString path(filePath);
	if (path.isEmpty())
		path = myApp.defaultFixtureConfigPath();

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
			m_layouts[i]->setFromJson(jsonOneUniv);
		}
	}
	else { // single universe file
		// load to first universe fixture list
		m_layouts[0]->setFromJson(json);
	}

	return true;
}

/**
 * @brief FixtureLayout::getDeviceShortId
 * @param universe [1:MAX_DMX_UNIVERSE]
 * @param dmxAddr [1:512]
 * @return short ident string for device, if assigned. Also empty string, if universe/dmxAddr not valid
 *
 */
QString FixtureLayout::getDeviceShortId(uint universe, uint dmxAddr)
{
	universe--;
	if (universe >= MAX_DMX_UNIVERSE || dmxAddr == 0 || dmxAddr > 512)
		return {};

	SR_FixtureList *fixlist = m_layouts[universe];

	for (int i=0; i<fixlist->size(); i++) {
		SR_Fixture *fix = fixlist->at(i);
		if (fix->containsDmxAddr(dmxAddr))
			return fix->shortIdent();
	}

	return {};
}
