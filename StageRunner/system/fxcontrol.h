#ifndef FXCONTROL_H
#define FXCONTROL_H

#include "config.h"
#include "toolclasses.h"

#include <QObject>


class AppCentral;
class FxSeqItem;
class FxSceneItem;
class FxAudioItem;
class FxPlayListItem;
class FxScriptItem;
class FxExecLoop;
class ExecLoopThreadInterface;
class Executer;
class FxListExecuter;
class ScriptExecuter;
class ExecCenter;

class FxControl : public QObject
{
	Q_OBJECT
public:
	AppCentral &myApp;
	ExecCenter &execCenter;
	ExecLoopThreadInterface *execLoopInterface;

public:
	FxControl(AppCentral &appCentral, ExecCenter &exec_center);
	~FxControl();
	bool setExecLoopEnabled(bool state);

	FxListExecuter * startFxSequence(FxSeqItem *fxseq);
	bool stopFxSequence(FxSeqItem *fxseq);
	int stopAllFxSequences();
	bool pauseFxPlaylist(FxPlayListItem *fxplay);
	bool stopFxPlayList(FxPlayListItem *fxplay);
	int pauseAllFxPlaylist();
	int stopAllFxPlaylists();

	ScriptExecuter *startFxScript(FxScriptItem *fxscript);
	bool stopFxScript(FxScriptItem *fxscript);



public slots:
	FxListExecuter * startFxAudioPlayList(FxPlayListItem *fxplay);
	FxListExecuter * continueFxAudioPlayList(FxPlayListItem *fxplay, FxAudioItem *fxaudio);

signals:
	void executerChanged(Executer *exec);

public slots:

};

#endif // FXCONTROL_H
