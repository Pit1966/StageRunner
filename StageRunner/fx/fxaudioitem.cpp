#include "fxaudioitem.h"
#include "../config.h"

#include <QFileInfo>
#include <QMediaPlayer>

QStringList FxAudioItem::attachedCmdStrings = QStringList()
		<< QObject::tr("None")
		<< QObject::tr("Fade out all audio")
		<< QObject::tr("Stop all audio")
		<< QObject::tr("Stop/BLACK video")
		<< QObject::tr("Start FX")
		<< QObject::tr("Stop all SEQ & SCRIPTS")
		<< QString("DEVELOPER ADD TEXT!");


FxAudioItem::FxAudioItem(FxList *fxList)
	: FxItem(fxList)
{
	init();
}

FxAudioItem::FxAudioItem(const FxAudioItem &o)
	: FxItem(o.myParentFxList)
{
	init();
	cloneFrom(o);
}

FxAudioItem::FxAudioItem(const QString &path, FxList *fxList)
	: FxItem(fxList)
{
	init();
	QFileInfo fi(path);
	myPath = path;
	myFile = fi.fileName();
	myName = fi.completeBaseName();
}

void FxAudioItem::setFilePath(const QString &path)
{
	QFileInfo fi(path);
	myPath = path;
	myFile = fi.fileName();
}

/**
 * @brief Initialize the FxAudioItem for executing it in a sequence
 *
 *  This function is called typically by a Executer on setting the current Fx
 */
void FxAudioItem::initForSequence()
{
	mySeqStatus = AUDIO_OFF;
}

void FxAudioItem::resetFx()
{
	mySeekPosition = 0;
	if (audioDuration > 0) {
		mySeekPosPerMille = 0;
	} else {
		mySeekPosPerMille = -1;
	}
	mySeqStatus = AUDIO_OFF;
}

void FxAudioItem::setSeekPosition(qint64 posMs)
{
	mySeekPosition = posMs;
	if (audioDuration) {
		mySeekPosPerMille = posMs * 1000 / audioDuration;
	} else {
		mySeekPosPerMille = -1;
	}
}

bool FxAudioItem::checkForVideoClip()
{
	const QString &suf = QFileInfo(filePath()).suffix().toLower();
	if (suf == "mkv"
			|| suf == "avi"
			|| suf == "mpg"
			|| suf == "mp4"
			|| suf == "m4v"
			|| suf == "mov") {
		isFxClip = true;
		qDebug() << fileName() << " is video file";
	} else {
		isFxClip = false;
	}

	return isFxClip;
}


void FxAudioItem::init()
{
	startInProgress = false;
	isDmxStarted = false;
	isFxClip = false;
	mySeekPosition = 0;
	mySeekPosPerMille = -1;
	myFxType = FX_AUDIO;
	mySeqStatus = AUDIO_OFF;
	myclass = PrefVarCore::FX_AUDIO_ITEM;

	addExistingVar(initialVolume,"InitialVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(currentVolume,"CurrentVolume",0,MAX_VOLUME,INITIAL_VOLUME);
	addExistingVar(audioDuration,"DurationMS");
	addExistingVar(initialSeekPos,"InitialSeekPos");
	addExistingVar(stopAtSeekPos,"StopAtSeekPos");
	addExistingVar(loopTimes,"LoopTimes");
	addExistingVar(playBackSlot,"PlayBackSlot");
	addExistingVar(attachedStartCmd,"AttachedStartCmd");
	addExistingVar(attachedStopCmd,"AttachedStopCmd");
	addExistingVar(attachedStartPara1,"AttachedStartPara1");
	addExistingVar(attachedStartPara2,"AttachedStartPara2");
	addExistingVar(attachedStopPara1,"AttachedStopPara1");
	addExistingVar(attachedStopPara2,"AttachedStopPara2");
	addExistingVar(blackAtVideoEnd,"BlackAtVideoEnd");
	// qDebug("init audio fx with id: %d",myId);
}
