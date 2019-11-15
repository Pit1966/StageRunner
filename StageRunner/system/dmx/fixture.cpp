#include "fixture.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

SR_Channel::SR_Channel(SR_Fixture *parent)
	: m_parentFixture(parent)
{
}

SR_Channel::SR_Channel(SR_Fixture *parent, SR_Channel *op)
	: m_parentFixture(parent)
	, m_name(op->m_name)
	, m_group(op->m_group)
	, m_dmxOffset(op->m_dmxOffset)
	, m_dmxFineOffset(op->m_dmxFineOffset)
{
}

bool SR_Channel::loadQLCChannel(QXmlStreamReader &xml)
{
	if (xml.name() != SR_CHANNEL) {
		qWarning() << __func__ << "Channel node not found";
		return false;
	}

	QXmlStreamAttributes attrs = xml.attributes();

	// channel name
	m_name = attrs.value(SR_CHANNEL_NAME).toString();
	if (m_name.isEmpty()) {
		qWarning() << __func__ << "Missing channel name";
		return false;
	}

	while (xml.readNextStartElement()) {
		if (xml.name() == SR_CHANNEL_GROUP) {
			m_dmxFineOffset = xml.attributes().value(SR_CHANNEL_GROUPBYTE).toInt();
			m_group = xml.readElementText();
		}
		else {
			xml.skipCurrentElement();
		}
	}
	return true;
}

SR_Channel *SR_Channel::createLoadQLCChannel(SR_Fixture *parent, QXmlStreamReader &xml)
{
	SR_Channel *ch = new SR_Channel(parent);
	if (!ch->loadQLCChannel(xml)) {
		delete ch;
		return nullptr;
	}
	return ch;
}

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------


SR_Mode::SR_Mode(SR_Fixture *parent)
	: m_parentFixture(parent)
{
}

SR_Mode::SR_Mode(SR_Fixture *parent, SR_Mode *op)
	: m_parentFixture(parent)
	, m_name(op->name())
{
	for (auto chan : op->channels()) {
		SR_Channel *ownch = parent->getChannelByName(chan->name());
		m_channels.append(ownch);
	}
}

bool SR_Mode::loadQLCMode(QXmlStreamReader &xml)
{
	if (xml.name() != SR_FIXTURE_MODE) {
		qWarning() << __func__ << "Mode node not found";
		return false;
	}

	// mode name
	m_name = xml.attributes().value(SR_FIXTURE_MODE_NAME).toString();
	if (m_name.isEmpty()) {
		qWarning() << __func__ << "Mode name not found";
		return false;
	}

	while (xml.readNextStartElement()) {
		if (xml.name() == SR_FIXTURE_MODE_CHANNEL) {
			int chanNum = xml.attributes().value(SR_FIXTURE_MODE_CANNEL_NUM).toInt();
			SR_Channel *ch = m_parentFixture->getChannelByName(xml.readElementText());
			if (!insertChannelAt(chanNum, ch)) {
				qWarning() << __func__ << m_name << ": Failure on inserting channel" << xml.readElementText();
				return false;
			}
		}
		else if (xml.name() == SR_FIXTURE_HEAD) {
			/// @todo implement me
			xml.skipCurrentElement();
		}
		else if (xml.name() == SR_FIXTURE_PHYSICAL) {
			/// @todo implement me
			xml.skipCurrentElement();
		}
		else {
			xml.skipCurrentElement();
		}

	}
	return true;
}

bool SR_Mode::insertChannelAt(int pos, SR_Channel *srChan)
{
	if (!srChan) {
		qWarning() << __func__ << "Tried to add null pointer channel";
		return false;
	}

	if (!m_parentFixture->containsChannel(srChan)) {
		qWarning() << __func__ << srChan->name() << "is not part of parent fixture";
		return false;
	}

	if (m_channels.contains(srChan)) {
		qWarning() << __func__ << srChan->name() << "was already added to mode" << m_name;
		return false;
	}

	m_channels.insert(pos, srChan);
	return true;
}

QStringList SR_Mode::getChannelTexts() const
{
	QStringList list;
	int chanNum = 0;
	for (auto chan : m_channels) {
		QString s = QString("%1: %2").arg(++chanNum).arg(chan->name());
		list.append(s);
	}
	return list;
}

SR_Mode *SR_Mode::createLoadQLCMode(SR_Fixture *parent, QXmlStreamReader &xml)
{
	SR_Mode *m = new SR_Mode(parent);
	if (!m->loadQLCMode(xml)) {
		delete m;
		return nullptr;
	}
	return m;
}


// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------
SR_Fixture::SR_Fixture()
	: m_fixtureType(FT_OTHER)
	, m_curMode(-1)
	, m_universe(0)
	, m_dmxAdr(0)
{
}

SR_Fixture::SR_Fixture(const SR_Fixture &o)
{
	cloneFrom(o);
}

SR_Fixture::~SR_Fixture()
{
	while (!m_channels.isEmpty())
		delete m_channels.takeFirst();

	while (!m_modes.isEmpty())
		delete m_modes.takeFirst();
}

void SR_Fixture::cloneFrom(const SR_Fixture &o)
{
	m_manufacturer = o.m_manufacturer;
	m_modelName = o.m_modelName;
	m_fixtureType = o.m_fixtureType;
	m_curMode = o.m_curMode;
	m_universe = o.m_universe;
	m_dmxAdr = o.m_dmxAdr;

	while (!m_channels.isEmpty())
		delete m_channels.takeFirst();

	while (!m_modes.isEmpty())
		delete m_modes.takeFirst();

	for (auto chan : o.m_channels) {
		SR_Channel *newchan = new SR_Channel(this, chan);
		m_channels.append(newchan);
	}

	for (auto mode : o.m_modes) {
		SR_Mode *newmode = new SR_Mode(this, mode);
		m_modes.append(newmode);
	}
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

SR_Channel *SR_Fixture::getChannelByName(const QString &name)
{
	for (auto *ch : m_channels) {
		if (ch->name() == name)
			return ch;
	}
	return nullptr;
}

bool SR_Fixture::containsChannel(SR_Channel *srChan) const
{
	return m_channels.contains(srChan);
}

int SR_Fixture::modeCount() const
{
	return m_modes.size();
}

QStringList SR_Fixture::modeList() const
{
	QStringList modenames;
	for (auto mode : m_modes) {
		modenames.append(mode->name());
	}
	return modenames;
}

SR_Mode *SR_Fixture::mode(int num)
{
	if (num >= 0 && num < m_modes.size())
		return m_modes.at(num);

	return nullptr;
}

void SR_Fixture::setCurrentMode(int num)
{
	if (num < m_modes.size())
		m_curMode = num;
}

QStringList SR_Fixture::getChannelTexts(int mode)
{
	if (mode >= m_modes.size())
		return QStringList();

	return m_modes.at(mode)->getChannelTexts();
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
			SR_Channel *ch = SR_Channel::createLoadQLCChannel(this, xml);
			if (!ch) {
				subok = false;
			}
			else {
				ch->setDmxOffset(m_channels.size());
				m_channels.append(ch);
			}
		}
		else if (name == SR_FIXTURE_MODE) {
			SR_Mode *mode = SR_Mode::createLoadQLCMode(this, xml);
			if (!mode) {
				subok = false;
			}
			else {
				m_modes.append(mode);
			}
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


// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------



SR_FixtureList::~SR_FixtureList()
{
	while (!m_list.isEmpty())
		delete m_list.takeFirst();
}

void SR_FixtureList::addFixture(SR_Fixture *fix)
{
	m_list.append(fix);
}

bool SR_FixtureList::addQLCFixture(const QString &path)
{
	SR_Fixture *fix = new SR_Fixture();
	if (fix->loadQLCFixture(path)) {
		addFixture(fix);
		return true;
	} else {
		delete fix;
		return false;
	}
}
