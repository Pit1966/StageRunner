//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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

#ifndef APPCENTRAL_H
#define APPCENTRAL_H

#include "commandsystem.h"
#include "config.h"
#include "dmxtypes.h"

#include <QList>
#include <QObject>
#include <QByteArray>
#include <QWidget>
#include <QPointer>
#include <QTcpSocket>
#include <QElapsedTimer>

// #ifdef USE_SDL
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_mixer.h>
// #endif


class AudioControl;
class LightControl;
class Project;
class UserSettings;
class ColorSettings;
class FxItem;
class FxList;
class IOPluginCentral;
class DmxMonitor;
class ExecCenter;
class Executer;
class FxList;
class FxControl;
class FxListVarSet;
class VideoControl;
class DmxUniverseProperty;
class FxSceneItem;
class QThread;
class NetServer;
class DmxChannel;
class FixtureLayout;

using namespace AUDIO;
using namespace LIGHT;

class AppCentral : public QObject
{
	Q_OBJECT
public:
	enum MODUL_ERROR {
		E_NO_ERROR					= 0,
		E_NO_AUDIO_DECODER			= 1<<0,			///< Audio decoding service not found
		E_AUDIO_DEVICE_NOT_FOUND	= 1<<1,			///< Audio device (non default config not found)
		E_AUDIO_MULTI_OUT_FAIL		= 1<<2			///< Audio multi output on differnt devices not possible due to wrong audio mode (only possible in classic mode)
	};

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;
	bool m_isEditMode						= false;
	bool m_isFadeToMode						= false;
	bool m_isInputAssignMode				= false;
	FxItem * input_assign_target_fxitem		= nullptr;
	FxItem * last_global_selected_fxitem	= nullptr;

	QPointer<DmxMonitor> m_openedInputDmxMonitorWidgets[MAX_DMX_UNIVERSE];
	QPointer<DmxMonitor> m_openedOutputDmxMonitorWidgets[MAX_DMX_UNIVERSE];

	int m_moduleErrorMask		= E_NO_ERROR;
	bool m_isSDLAvailable		= false;
	bool m_isApplicationStart	= true;			///< this is true in init phase after program start until Qt enters exec loop

	QThread *m_mainThread		= nullptr;

	// network
	QTcpSocket *m_remoteSocket	= nullptr;
	QString m_lastRemote;
	QStringList m_pendingRemoteCmdList;
	bool m_remoteOk				= false;

	// gui
	bool m_hasSecondScreen		= false;
	QPoint m_secondScreenPos;
	QSize m_secondScreenSize;

	// system
	QElapsedTimer m_uptime;
	uint m_activeKeyModifiers	= 0;

public:
	QWidget *mainwinWidget				= nullptr;
	QObject *mainWinObj					= nullptr;
	AudioControl *unitAudio				= nullptr;
	LightControl *unitLight				= nullptr;
	FxControl *unitFx					= nullptr;
	VideoControl *unitVideo				= nullptr;
	NetServer *tcpServer				= nullptr;
	Project *project					= nullptr;
	DmxUniverseProperty *universeLayout = nullptr;
	UserSettings *userSettings			= nullptr;
	ColorSettings *colorSettings		= nullptr;
	IOPluginCentral *pluginCentral		= nullptr;
	ExecCenter *execCenter				= nullptr;
	FixtureLayout *fixLayout			= nullptr;
	FxListVarSet *templateFxList		= nullptr;

public:
	static AppCentral * instance();
	static AppCentral & ref();
	static bool isReady() {return myinstance != nullptr;}
	static bool destroyInstance();
	static QString defaultFixtureConfigPath();

	bool isApplicationStart() const {return m_isApplicationStart;}
	void startupReady();
	void clearProject();
	bool setLightLoopEnabled(bool state);
	bool setFxExecLoopEnabled(bool state);

	bool initAudioControl();

	void stopAllFxAudio();
	void fadeoutAllFxAudio(int fadeoutTimeMs = 5000);
	void stopAllSequencesAndPlaylists();

	void lightBlack(qint32 time_ms = 0);
	void videoBlack(qint32 time_ms = 0);
	void sequenceStop(FxItem *fxseq = nullptr);

	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);
	bool isExperimentalAudio() const;
	inline bool isInputAssignMode() const {return m_isInputAssignMode;}
	void setInputAssignMode(bool state);
	void setInputAssignMode(FxItem *fx);

	bool startTcpServer();

	void loadPlugins();
	void openPlugins();
	void reOpenPlugins();
	void closePlugins();
	DmxMonitor *openDmxInMonitor(int universe);
	DmxMonitor *openDmxOutMonitor(int universe);
	int closeAllDmxMonitors();

	void assignInputToSelectedFxItem(qint32 universe, qint32 channel, int value);
	bool addFxAudioDialog(FxList *fxlist, QWidget *widget = nullptr, int row = -1);
	FxItem *globalSelectedFx() {return last_global_selected_fxitem;}
	FxItem *addDefaultSceneToFxList(FxList *fxlist, int row = -1);

	// Modes
	inline bool isFadeToMode() const {return m_isFadeToMode;}
	inline bool isEditMode() const {return m_isEditMode;}

	// Module error handling
	void setModuleError(MODUL_ERROR e, bool state = true);
	MODUL_ERROR moduleErrors() const;
	bool hasModuleError() const;
	static QString moduleErrorText(MODUL_ERROR e);

	// Audio
	void setAudioOutputType(AUDIO::AudioOutputType type);
	AUDIO::AudioOutputType usedAudioOutputType() const;
	bool isAudioInThread() const;
	inline bool hasSDL() const {return m_isSDLAvailable;}
	bool isLogarithmicVolume() const;
	QObject * audioWorker();
	QThread * audioThread();

	// Sytem
	qint64 uptimeMs() const {return m_uptime.elapsed();}
	void closeVideoWidget();
	bool isVideoWidgetVisible(QWidget **videoWid = nullptr) const;
	void setCurrentKeyModifiers(uint val);
	uint currentKeyModifiers() const {return m_activeKeyModifiers;}
	bool isShiftPressed() const {return m_activeKeyModifiers & Qt::SHIFT;}
	bool isCtrlPressed() const {return m_activeKeyModifiers & Qt::CTRL;}
	bool isModifierPressed() const {return m_activeKeyModifiers & (Qt::SHIFT | Qt::CTRL);}

	// query functions for universe layout scenes
	DmxChannel *globalDmxChannel(quint32 universe, qint32 dmxChan);
	DmxChannelType globalDmxType(quint32 universe, qint32 dmxChan);
	qint32 globalScalerNumerator(quint32 universe, qint32 dmxChan);
	qint32 globalScalerDenominator(quint32 universe, qint32 dmxChan);
	QString globalDeviceShortId(quint32 universe, qint32 dmxChan);
	qint32 globalDeviceIndex(quint32 universe, qint32 dmxChan);
	qint32 globalFindDmxAddrForShortId(const QString &shortId, int universe = -1);

	// Gui
	bool hasSecondScreen() const {return m_hasSecondScreen;}
	QSize secondScreenSize() const;
	QPoint secondScreenCenterPoint() const;
	QStyle *mainDialWidgetStyle() const;

private:
	AppCentral();
	~AppCentral();
	void init();

public slots:
	void executeFxCmd(FxItem *fx, CtrlCmd cmd, Executer * exec);
	void executeFxCmd(qint32 id, CtrlCmd cmd, Executer *exec);
	void executeNextFx(int listID = 1);
	void executePrevFx(int listID);
	void clearCurrentFx(int listID);
	void moveToFollowerFx(int listID);
	void moveToForeRunnerFx(int listID);
	void setEditMode(bool state);
	void setFadeToMode(bool state);
	void setFFTAudioChannelMask(qint32 mask);

	void testSetDmxChannel(int val, int channel);
	void onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value);
	void setGlobalSelectedFx(FxItem *item);

	void deleteFxSceneItem(FxSceneItem *scene);
	void connectToRemote(const QString &host, quint16 port, const QString &cmd);

private slots:
	void onRemoteConnected();
	void onRemoteStateChanged(QAbstractSocket::SocketState state);
	void onRemoteSocketDataReceived();

signals:
	void audioCtrlMsgEmitted(AUDIO::AudioCtrlMsg msg);
	void editModeChanged(bool state);
	void fadeSceneToModeChanged(bool state);
	void inputAssignModeChanged(bool state);
	void inputAssigned(FxItem *);

	void fxSceneDeleted(FxSceneItem *scene);

	void keyPressModifiersChanged(uint keyModifiers);
};


#endif // APPCENTRAL_H
