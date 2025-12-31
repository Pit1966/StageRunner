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
