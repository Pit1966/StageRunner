#ifndef FIXTURE_H
#define FIXTURE_H

class QXmlStreamReader;

#include <QString>
#include <QList>

// Fixture document type
#define SR_FIXTURE_DEF_DOCUMENT "FixtureDefinition"

// Fixture definition XML tags
#define SR_FIXTURE_DEF "FixtureDefinition"
#define SR_FIXTURE_DEF_MANUFACTURER "Manufacturer"
#define SR_FIXTURE_DEF_MODEL "Model"
#define SR_FIXTURE_DEF_TYPE "Type"

// Fixture instance XML tags
#define SR_FIXTURE_NAME "Name"
#define SR_FIXTURE_ID "ID"
#define SR_FIXTURE_UNIVERSE "Universe"
#define SR_FIXTURE_ADDRESS "Address"

#define SR_CHANNEL	         QString("Channel")
#define SR_CHANNEL_NAME      QString("Name")
#define SR_CHANNEL_PRESET    QString("Preset")
#define SR_CHANNEL_GROUP     QString("Group")
#define SR_CHANNEL_DEFAULT   QString("Default")
#define SR_CHANNEL_GROUPBYTE QString("Byte")
#define SR_CHANNEL_COLOUR    QString("Colour")

/* Compound strings used by PaletteGenerator to identify
 * special fixture modes
 */
#define SR_CHANNEL_MOVEMENT  QString("Movement")
#define SR_CHANNEL_RGB       QString("RGB")
#define SR_CHANNEL_CMY       QString("CMY")
#define SR_CHANNEL_WHITE     QString("White")

#define SR_FIXTURE_MODE               "Mode"
#define SR_FIXTURE_MODE_NAME          "Name"
#define SR_FIXTURE_MODE_CHANNEL       "Channel"
#define SR_FIXTURE_MODE_CANNEL_NUM    "Number"


class SR_Channel
{
public:
	bool loadQLCChannel(QXmlStreamReader &xml);
	static SR_Channel * createLoadQLCChannel(QXmlStreamReader &xml);
};

class SR_Mode
{

};

class SR_Fixture
{
public:
	enum Type
	{
		FT_COLOR_CHANGER,
		FT_DIMMER,
		FT_EFFECT,
		FT_FAN,
		FT_FLOWER,
		FT_LASER,
		FT_HAZER,
		FT_MOVING_HEAD,
		FT_SCANNER,
		FT_SMOKE,
		FT_STROBE,
		FT_LEDBAR_BEAMS,
		FT_LEDBAR_PIXELS,
		FT_OTHER
	};

private:
	QString m_manufacturer;
	QString m_modelName;
	Type m_fixtureType;

	QList<SR_Channel*>m_channels;
	QList<SR_Mode*>m_modes;

public:
	SR_Fixture();

	bool loadQLCFixture(const QString &path);

	SR_Fixture::Type stringToType(const QString &type);
	QString typeToString(SR_Fixture::Type type);

private:
	bool loadQLCFixture(QXmlStreamReader &xml);
	bool subLoadCreator(QXmlStreamReader &xml);
};

#endif // FIXTURE_H
