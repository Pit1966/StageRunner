#include "fixture.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

bool SR_Channel::loadQLCChannel(QXmlStreamReader &xml)
{

	return true;
}

SR_Channel *SR_Channel::createLoadQLCChannel(QXmlStreamReader &xml)
{
	SR_Channel *ch = new SR_Channel();
	if (!ch->loadQLCChannel(xml)) {
		delete ch;
		return nullptr;
	}
	return ch;
}


SR_Fixture::SR_Fixture()
{

}

SR_Fixture::Type SR_Fixture::stringToType(const QString& type)
{
	if (type == "Color Changer") return FT_COLOR_CHANGER;
	else if (type == "Dimmer") return FT_DIMMER;
	else if (type == "Effect") return FT_EFFECT;
	else if (type == "Fan") return FT_FAN;
	else if (type == "Flower") return FT_FLOWER;
	else if (type == "Hazer") return FT_HAZER;
	else if (type == "Laser") return FT_LASER;
	else if (type == "Moving Head") return FT_MOVING_HEAD;
	else if (type == "Scanner") return FT_SCANNER;
	else if (type == "Smoke") return FT_SMOKE;
	else if (type == "Strobe") return FT_STROBE;
	else if (type == "LED Bar (Beams)") return FT_LEDBAR_BEAMS;
	else if (type == "LED Bar (Pixels)") return FT_LEDBAR_PIXELS;

	return FT_OTHER;
}

QString SR_Fixture::typeToString(SR_Fixture::Type type)
{
	switch(type)
	{
		case FT_COLOR_CHANGER: return "Color Changer";
		case FT_DIMMER: return "Dimmer";
		case FT_EFFECT: return "Effect";
		case FT_FAN: return "Fan";
		case FT_FLOWER: return "Flower";
		case FT_HAZER: return "Hazer";
		case FT_LASER: return "Laser";
		case FT_MOVING_HEAD: return "Moving Head";
		case FT_SCANNER: return "Scanner";
		case FT_SMOKE: return "Smoke";
		case FT_STROBE: return "Strobe";
		case FT_LEDBAR_BEAMS: return "LED Bar (Beams)";
		case FT_LEDBAR_PIXELS: return "LED Bar (Pixels)";
		default: return "Other";
	}
}


bool SR_Fixture::loadQLCFixture(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
		qWarning() << QT_TR_NOOP("Could not open fixture file") << path << file.errorString();
		return false;
	}

	QXmlStreamReader xmlread(&file);

	while (!xmlread.atEnd()) {
		if (xmlread.readNext() == QXmlStreamReader::DTD)
			break;
	}

	if (xmlread.hasError()) {
		qWarning() << QT_TR_NOOP("Error in xml structure of fixture file") << path << file.errorString();
		return false;
	}

	if (xmlread.dtdName() != SR_FIXTURE_DEF_DOCUMENT) {
		qWarning() << QT_TR_NOOP("Seems not to be a QLC compat fixture file") << path << file.errorString();
		return false;
	}

	if (!loadQLCFixture(xmlread)) {
		qWarning() << QT_TR_NOOP("Error while parsing fixture file") << path << file.errorString();
		return false;
	}

	return true;
}

bool SR_Fixture::loadQLCFixture(QXmlStreamReader &xml)
{
	if (!xml.readNextStartElement())
		return false;

	if (xml.name() != SR_FIXTURE_DEF)
		return false;

	bool ok = true;
	bool subok = true;

	while (xml.readNextStartElement()) {
		QStringRef name = xml.name();
		if (name == "Creator") {
			subok = subLoadCreator(xml);
		}
		else if (name == SR_FIXTURE_DEF_MANUFACTURER) {
			m_manufacturer = xml.readElementText();
		}
		else if (name == SR_FIXTURE_DEF_MODEL) {
			m_modelName = xml.readElementText();
		}
		else if (name == SR_FIXTURE_DEF_TYPE) {
			m_fixtureType = stringToType(xml.readElementText());
		}
		else if (name == SR_CHANNEL) {
			SR_Channel *ch = SR_Channel::createLoadQLCChannel(xml);
			if (!ch)
				subok = false;
		}
		else if (name == SR_FIXTURE_MODE) {

		}

	}

	return ok;
}

bool SR_Fixture::subLoadCreator(QXmlStreamReader &xml)
{
	QStringRef name = xml.name();
	if (name != "Creator") {
		qWarning() << QT_TR_NOOP("Creator information expected!");
		return false;
	}

	while (xml.readNextStartElement()) {
		name = xml.name();
		/// @implement me

		xml.skipCurrentElement();
	}

	return true;
}
