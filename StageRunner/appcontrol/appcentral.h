//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

#include <QList>
#include <QObject>
#include <QByteArray>
#include <QWidget>
#include <QPointer>
#include <QTcpSocket>

#ifdef USE_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif


class AudioControl;
class LightControl;
class Project;
class UserSettings;
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

using namespace AUDIO;
using namespace LIGHT;

class AppCentral : public QObject
{
	Q_OBJECT
public:
	enum MODUL_ERROR {
		E_NO_ERROR				= 0,
		E_NO_AUDIO_DECODER		= 1<<0			///< Audio decoding service not found
	};

private:
	static AppCentral *myinstance;
	QList<FxList*>registered_fx_lists;
	bool edit_mode_f;
	bool input_assign_mode_f;
	FxItem * input_assign_target_fxitem;
	FxItem * last_global_selected_fxitem;

	QPointer<DmxMonitor> m_openedInputDmxMonitorWidgets[MAX_DMX_UNIVERSE];
	QPointer<DmxMonitor> m_openedOutputDmxMonitorWidgets[MAX_DMX_UNIVERSE];

	int m_moduleErrorMask;
	bool m_isSDLAvailable;

	QThread *m_mainThread;

	// network
	QTcpSocket *m_remoteSocket;
	QString m_lastRemote;
	QStringList m_pendingRemoteCmdList;
	bool m_remoteOk			= false;

public:
	QWidget *mainwinWidget;
	QObject *mainWinObj;
	AudioControl *unitAudio;
	LightControl *unitLight;
	FxControl *unitFx;
	VideoControl *unitVideo;
	NetServer *tcpServer;
	Project *project;
	DmxUniverseProperty *universeLayout;
	UserSettings *userSettings;
	IOPluginCentral *pluginCentral;
	ExecCenter *execCenter;
	FxListVarSet *templateFxList;

#ifdef USE_SDL
	Mix_Music *testsdl;
#endif


public:
	static AppCentral * instance();
	static AppCentral & ref();
	static bool isReady() {return myinstance != nullptr;}
	static bool destroyInstance();

	void clearProject();
	bool setLightLoopEnabled(bool state);
	bool setFxExecLoopEnabled(bool state);

	void stopAllFxAudio();
	void fadeoutAllFxAudio(int fadeoutTimeMs = 5000);
	void stopAllSequencesAndPlaylists();

	void lightBlack(qint32 time_ms = 0);
	void videoBlack(qint32 time_ms = 0);
	void sequenceStop(FxItem *fxseq = nullptr);

	int registerFxList(FxList *fxlist);
	FxList *getRegisteredFxList(int id);
	inline bool isEditMode() const {return edit_mode_f;}
	bool isExperimentalAudio() const;
	inline bool isInputAssignMode() const {return input_assign_mode_f;}
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
	FxItem *addDefaultSceneToFxList(FxList *fxlist);

	// Module error handling
	void setModuleError(MODUL_ERROR e, bool state = true);
	MODUL_ERROR moduleErrors() const;
	bool hasModuleError() const;
	static QString moduleErrorText(MODUL_ERROR e);

	// Audio
	AUDIO::AudioOutputType usedAudioOutputType() const;
	inline bool hasSDL() const {return m_isSDLAvailable;}

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
	void setAudioOutputType(AUDIO::AudioOutputType type);
	void setFFTAudioChannelMask(qint32 mask);

	void testSetDmxChannel(int val, int channel);
	void onInputUniverseChannelChanged(quint32 universe, quint32 channel, uchar value);
	void setGlobalSelectedFx(FxItem *item);

	void deleteFxSceneItem(FxSceneItem *scene);

	void connectToRemote(const QString &host, quint16 port, const QString &cmd);

private slots:
	void onRemoteConnected();
	void onRemoteStateChanged(QAbstractSocket::SocketState state);

signals:
	void audioCtrlMsgEmitted(AudioCtrlMsg msg);
	void editModeChanged(bool state);
	void inputAssignModeChanged(bool state);
	void inputAssigned(FxItem *);

	void fxSceneDeleted(FxSceneItem *scene);
};

#endif // APPCENTRAL_H
