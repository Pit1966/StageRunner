#ifndef FIXTURE_H
#define FIXTURE_H

class QXmlStreamReader;

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>

// Fixture document type
#define SR_FIXTURE_DEF_DOCUMENT "FixtureDefinition"

// Fixture definition XML tags
#define SR_FIXTURE_DEF "FixtureDefinition"
#define SR_FIXTURE_DEF_MANUFACTURER "Manufacturer"
#define SR_FIXTURE_DEF_MODEL "Model"
#define SR_FIXTURE_DEF_TYPE "Type"

#define SR_FIXTURE_PHYSICAL	"Physical"

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

#define SR_FIXTURE_HEAD				"Head"
#define SR_FIXTURE_HEAD_CHANNEL		"Channel"


class SR_Fixture;

class SR_Channel : public QObject
{
	Q_OBJECT
public:
	/*********************************************************************
	 * Presets: These Presets are taken from QLCPLUS Project
	 *
	 * please see
	 * https://github.com/mcallegari/qlcplus/wiki/Fixture-definition-presets
	 * when changing this list
	 *********************************************************************/
	enum Preset
	{
		Custom = 0,
		IntensityMasterDimmer,
		IntensityMasterDimmerFine,
		IntensityDimmer,
		IntensityDimmerFine,
		IntensityRed,
		IntensityRedFine,
		IntensityGreen,
		IntensityGreenFine,
		IntensityBlue,
		IntensityBlueFine,
		IntensityCyan,
		IntensityCyanFine,
		IntensityMagenta,
		IntensityMagentaFine,
		IntensityYellow,
		IntensityYellowFine,
		IntensityAmber,
		IntensityAmberFine,
		IntensityWhite,
		IntensityWhiteFine,
		IntensityUV,
		IntensityUVFine,
		IntensityIndigo,
		IntensityIndigoFine,
		IntensityLime,
		IntensityLimeFine,
		IntensityHue,
		IntensityHueFine,
		IntensitySaturation,
		IntensitySaturationFine,
		IntensityLightness,
		IntensityLightnessFine,
		IntensityValue,
		IntensityValueFine,
		PositionPan,
		PositionPanFine,
		PositionTilt,
		PositionTiltFine,
		PositionXAxis,
		PositionYAxis,
		SpeedPanSlowFast,
		SpeedPanFastSlow,
		SpeedTiltSlowFast,
		SpeedTiltFastSlow,
		SpeedPanTiltSlowFast,
		SpeedPanTiltFastSlow,
		ColorMacro,
		ColorWheel,
		ColorWheelFine,
		ColorRGBMixer,
		ColorCTOMixer,
		ColorCTCMixer,
		ColorCTBMixer,
		GoboWheel,
		GoboWheelFine,
		GoboIndex,
		GoboIndexFine,
		ShutterStrobeSlowFast,
		ShutterStrobeFastSlow,
		ShutterIrisMinToMax,
		ShutterIrisMaxToMin,
		ShutterIrisFine,
		BeamFocusNearFar,
		BeamFocusFarNear,
		BeamFocusFine,
		BeamZoomSmallBig,
		BeamZoomBigSmall,
		BeamZoomFine,
		PrismRotationSlowFast,
		PrismRotationFastSlow,
		NoFunction,
		LastPreset // dummy for cycles
	};
#if QT_VERSION >= 0x050500
	Q_ENUM(Preset)
#else
	Q_ENUMS(Preset)
#endif

private:
	SR_Fixture *m_parentFixture;
	QString m_name;
	QString m_group;
	QString m_preset;
	int m_dmxOffset = 0;		///< not used
	int m_dmxFineOffset = 0;	///< used for dmx devices using 2 channels (or more) for a function with 16bit (or more)

public:
	SR_Channel(SR_Fixture *parent);
	SR_Channel(SR_Fixture *parent, SR_Channel *op);
	inline const QString & name() const {return m_name;}
	inline const QString & group() const {return m_group;}
	inline const QString & preset() const {return m_preset;}

	bool loadQLCChannel(QXmlStreamReader &xml);
	inline void setDmxOffset(int chanoffset) {m_dmxOffset = chanoffset;}
	int dmxOffset() const {return m_dmxOffset;}

	QJsonObject toJson() const;
	bool setFromJson(const QJsonObject &json);

	static SR_Channel * createLoadQLCChannel(SR_Fixture *parent, QXmlStreamReader &xml);
	static QString presetToString(Preset preset);
	static Preset stringToPreset(const QString &preset);

};

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

class SR_Mode
{
private:
	SR_Fixture *m_parentFixture;
	QString m_name;
	QList<SR_Channel*> m_channels;				///< list of channels used by this mode (channel pointers not owned!! They are managed by parent SR_Fixture)

public:
	SR_Mode(SR_Fixture *parent);
	SR_Mode(SR_Fixture *parent, SR_Mode *op);
	inline const QString & name() const {return m_name;}
	inline QList<SR_Channel*> & channels() {return m_channels;}
	bool loadQLCMode(QXmlStreamReader &xml);
	bool insertChannelAt(int pos, SR_Channel *srChan);

	int channelCount() const;
	QStringList getChannelTexts() const;
	QStringList getChannelAndPresetTexts() const;

	QJsonObject toJson() const;
	bool setFromJson(const QJsonObject &json);

	static SR_Mode * createLoadQLCMode(SR_Fixture *parent, QXmlStreamReader &xml);
};

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

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
	int m_curMode;						///< current active mode.

	// for universe layout
	int m_universe = 0;
	int m_dmxAdr = 0;

	QList<SR_Channel*>m_channels;
	QList<SR_Mode*>m_modes;

public:
	SR_Fixture();
	SR_Fixture(const SR_Fixture &o);
	~SR_Fixture();


	void cloneFrom(const SR_Fixture &o);

	inline const QString &manufacturer() const {return m_manufacturer;}
	inline const QString & modelName() const {return m_modelName;}

	bool loadQLCFixture(const QString &path);
	SR_Channel *getChannelByName(const QString &name);
	bool containsChannel(SR_Channel *srChan) const;

	int modeCount() const;
	QStringList modeList() const;
	SR_Mode * mode(int num);
	void setCurrentMode(int num);
	inline int currentMode() const {return m_curMode;}

	int usedChannelCount() const;
	QStringList getChannelTexts(int mode = 0) const;
	QStringList getChannelAndPresetTexts(int mode = 0) const;


	SR_Fixture::Type stringToType(const QString &type);
	QString typeToString(SR_Fixture::Type type);

	inline int dmxAdr() const {return m_dmxAdr;}
	void setDmxAdr(int dmxAdr) {m_dmxAdr = dmxAdr;}

	QJsonObject toJson() const;
	bool setFromJson(const QJsonObject &json);

private:
	bool loadQLCFixture(QXmlStreamReader &xml);
	bool subLoadCreator(QXmlStreamReader &xml);
};

// ------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------

class SR_FixtureList
{
private:
	QList <SR_Fixture*> m_list;

public:
	~SR_FixtureList();
	inline int size() const {return m_list.size();}
	inline SR_Fixture * at(int i) {return m_list.at(i);}
	inline const SR_Fixture * at(int i) const {return m_list.at(i);}

	int lastUsedDmxAddr() const;

	void addFixture(SR_Fixture *fix, int dmxAddr = 0);
	bool addQLCFixture(const QString &path, int dmxAddr = 0);

	QJsonObject toJson() const;
	int setFromJson(const QJsonObject &json);

};

#endif // FIXTURE_H
