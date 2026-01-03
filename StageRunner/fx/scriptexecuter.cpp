#include "scriptexecuter.h"

#include "system/log.h"
#include "system/qt_versions.h"
#include "system/videocontrol.h"
#include "system/lightcontrol.h"
#include "system/dmxchannel.h"
#include "appcontrol/audiocontrol.h"
#include "appcontrol/appcentral.h"
#include "tool/qtstatictools.h"
#include "fx/fxitem_includes.h"

#include <QRegularExpression>


using namespace SCRIPT;

bool ScriptExecuter::processExecuter()
{
	if (myState == EXEC_IDLE) return false;

	bool proceed = true;
	bool active = true;

	while (proceed) {
		FxScriptLine *line = m_script.at(m_currentLineNum);
		if (line && myState == EXEC_RUNNING) {
			bool reExecLineDelayed = false;
			if (line->command().size() > 2 && !line->command().startsWith("#"))
				proceed = executeLine(line, reExecLineDelayed);

			if (!reExecLineDelayed)
				m_currentLineNum++;

		} else {
			proceed = false;
			active = false;
			LOGTEXT(tr("<font color=darkGreen>Script</font> '%1' <font color=darkGreen>finished</font>")
					.arg(m_fxScriptItem->name()));
		}
	}

	return active;
}

void ScriptExecuter::processProgress()
{
	if (runTime.elapsed() > lastProgressTimeMs) {
		lastProgressTimeMs += 50;
		int perMille = 0;
		if (m_script.execDuration() > 0) {
			qint64 timeMs = runTime.elapsed() % m_script.execDuration();
			perMille = timeMs * 1000 / m_script.execDuration();
		}
		emit listProgressStepChanged(perMille, 0);
	}

	if (myState == EXEC_PAUSED) {
		if (m_lastState != EXEC_PAUSED) {
			emit executerStatusChanged(parentFxItem, tr("$$Paused"));		// $$ is a control code in order to let the display blink
		}
	}
	else if (myState == EXEC_RUNNING) {
		if (m_lastState != EXEC_RUNNING) {
			emit executerStatusChanged(parentFxItem, QString());
		}
	}
	else if (myState == EXEC_FINISH && m_breakOnCancel) {
		if (m_currentLineNum < m_script.size()) {
			// fadeout all scenes that where cloned by this executer
			for (FxSceneItem *fxs : std::as_const(m_clonedSceneList)) {
				fxs->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT, 200);
			}
			LOGTEXT(tr("<font color=darkGreen>Script</font> '%1' <font color=darkOrange>Canceled</font>")
					.arg(m_fxScriptItem->name()));
			destroyLater();
		}
		emit executerStatusChanged(parentFxItem, QString());
	}

	m_lastState = myState;
}

void ScriptExecuter::onPauseEvent(bool active)
{
	if (!active) {
		runTime.cont();
		runTimeOne.cont();
	}
}

ScriptExecuter::ScriptExecuter(AppCentral &app_central, FxScriptItem *script, FxItem *parentFx)
	: Executer(app_central,script)
	, m_fxScriptItem(script)
	, m_currentLineNum(0)
	, m_breakOnCancel(false)
	, m_disableMultiStart(false)
	, m_startedByDMX(false)
{
	parentFxItem = parentFx;
	if (script)
		FxScriptItem::rawToScript(script->rawScript(), m_script);

	// qDebug("new script executer: %s: duration: %llims",script->name().toLocal8Bit().data(),m_script.execDuration());
}

ScriptExecuter::~ScriptExecuter()
{
	emit listProgressStepChanged(0, 0);
	emit executerStatusChanged(nullptr, QString());

	while (!m_clonedSceneList.isEmpty()) {
		FxSceneItem *scene = m_clonedSceneList.takeFirst();
		if (!scene->isOnStageIntern() && !scene->isActive()) {
			emit wantedDeleteFxScene(scene);
		} else {
			scene->setDeleteOnFinished();
		}
	}
}

/**
 * @brief ScriptExecuter::getTargetFxItemFromPara
 * @param line
 * @param paras
 * @param fxList
 * @return
 *
 * This should parse strings like:
 * [id|text] number
 * [id|text] text
 * number
 * text
 *
 */
QString ScriptExecuter::getTargetFxItemFromPara(FxScriptLine *line , const QString &paras, FxItemList &fxList)
{
	QString returnparas;

	// parse parameter string
	QStringList tlist = QtStaticTools::parameterStringSplit(paras);

	int searchmode = 0;

	if (tlist.size()) {
		QString p1 = tlist.first();
		QString p1low = p1.toLower();
		if (p1low == "id") {
			searchmode = 1;
			tlist.removeFirst();
		}
		else if (p1low == "text" || p1low == "name") {
			searchmode = 2;
			tlist.removeFirst();
		}
		else {
			// Test if parameter maybe an ID or a FX name
			bool ok;
			int id = p1.toInt(&ok);
			Q_UNUSED(id)
			if (ok) {
				searchmode = 1;
			} else {
				searchmode = 2;
			}
		}
	}

	if (tlist.isEmpty()) {
		LOGERROR(tr("Script '%1':  Line #%2: <font color=darkOrange>Target parameter missing!</font>")
				 .arg(m_fxScriptItem->name())
				 .arg(line->lineNumber()));

		return returnparas;
	}

	if (searchmode == 1) { // search for an FX by ID
		int id = tlist.takeFirst().toInt();

		FxItem *fx = FxItem::findFxById(id);
		if (!fx) {
			searchmode = 0;
			LOGTEXT(tr("Script '%1':  Line #%2: <font color=darkOrange>FX with ID #%3 not found!</font>")
					.arg(m_fxScriptItem->name())
					.arg(line->lineNumber())
					.arg(id));
		} else {
			fxList.append(fx);
		}
	}
	else if (searchmode == 2) { // search for an FX by name
		QString name = tlist.takeFirst();
		QString sname = name;
		FxSearchMode sm = FXSM_EXACT;
		if (name.startsWith("*")) {
			sm = FXSM_RIGHT;
			sname = name.mid(1);
		}
		if (name.endsWith("*")) {
			sname.chop(1);
			if (sm == FXSM_RIGHT)
				sm = FXSM_LIKE;
			else
				sm = FXSM_LEFT;
		}
		QList<FxItem*>list = FxItem::findFxByName(sname,sm);
		if (list.isEmpty()) {
			LOGTEXT(tr("Script '<font color=#6666ff>%1</font>': Line #%2:  <font color=darkOrange>No FX with name '%3' found!</font>")
					.arg(m_fxScriptItem ? m_fxScriptItem->name() : QString("no fxitem"))
					.arg(line->lineNumber())
					.arg(name));
		}
		else {
			fxList.append(list);
		}
	}

	for (int t=0; t<tlist.size(); t++) {
		if (t>0)
			returnparas += " ";
		returnparas += tlist.at(t);
	}

	return returnparas;
}

/**
 * @brief Get leading part of a string, which is seperated by a space and remove it from original string
 * @param parastr
 * @param allowCommaSep
 * @return
 */
QString ScriptExecuter::getFirstParaOfString(QString &parastr, bool allowCommaSep)
{
	parastr = parastr.simplified();

	QString first;
	if (allowCommaSep) {
		static QRegularExpression re("[ ,]");
		first = parastr.section(re, 0, 0);
	} else {
		first = parastr.section(' ', 0, 0);
	}
	parastr.remove(0, first.size() + 1);

	return first;
}

int ScriptExecuter::getFrontIntOfString(QString &parastr, bool *ok, const QString &errHint)
{
	parastr = parastr.simplified();
	QString front = parastr.section(' ', 0, 0);
	bool intok;
	int val = front.toInt(&intok);
	if (front.isEmpty()) {
		m_lastScriptError += tr("Missing numeric parameter");
		if (errHint.size())
			m_lastScriptError.append(tr("at %1").arg(errHint));
		m_lastScriptError.append('\n');
		if (ok)
			*ok = false;
		return 0;
	}
	else if (!intok) {
		m_lastScriptError += tr("Parameter must be numeric");
		if (errHint.size())
			m_lastScriptError.append(tr("at %1").arg(errHint));
		m_lastScriptError.append('\n');
		if (ok)
			*ok = false;
		return 0;
	}

	if (ok)
		*ok = true;
	return val;
}

int ScriptExecuter::getPos(QString &restPara)
{
	QStringList paras = restPara.split(' ', QT_SKIP_EMPTY_PARTS);
	int pos = -2;

	QMutableListIterator<QString> it(paras);
	while (it.hasNext()) {
		QString tag = it.next();
		if (tag.toLower() == "pos") {
			pos = -1;
			it.remove();
			if (it.hasNext()) {
				QString timestr = it.next();
				int time = QtStaticTools::timeStringToMS(timestr);
				pos = time;
				it.remove();
			}
			break;
		}
	}

	if (pos > -2) {
		restPara.clear();
		for (const QString &val : paras) {
			if (restPara.size())
				restPara += " ";
			restPara += val;
		}
	}

	return pos;
}

int ScriptExecuter::convertToDmxVal(const QString &valstr)
{
	bool ok;
	if (valstr.endsWith('%')) {
		float val = valstr.left(valstr.size() - 1).toFloat(&ok);
		if (ok) {
			int ival = 255.0f * val / 100.0f;
			return std::clamp(ival, 0, 255);
		}
	}
	else {
		int val = valstr.toInt(&ok);
		val = std::clamp(val, 0,255);
		if (ok)
			return val;
	}

	return -1;
}


/**
 * @brief This function does return the temorary copied FxItems for a given parent FxItem
 * @param fx
 * @return
 *
 * @note Only FxItems that were created by this executer instance are returned!
 */
FxItemList ScriptExecuter::getExecuterTempCopiesOfFx(FxItem *fx) const
{
	QList<FxItem*> list;
	foreach (FxItem *fxcopy, m_clonedSceneList) {
		if (fxcopy->tempCopyOrigin() == fx)
			list.append(fxcopy);
	}
	return list;
}


/**
 * @brief ScriptExecuter::executeLine
 * @param line
 * @param reExecDelayed
 * @return
 *
 * @attention m_fxScriptItem maybe NULL in execution!
 */
bool ScriptExecuter::executeLine(FxScriptLine *line, bool & reExecDelayed)
{
	bool ok = true;

	if (line->execTimeMs() >= 0) {
		if (line->execTimeMs() <= currentRunTimeMs()) {
			reExecDelayed = false;
		} else {
			reExecDelayed = true;
			setEventTargetTimeAbsolute(line->execTimeMs());
			return false;
		}
	} else {
		reExecDelayed = false;
	}

	const QString &cmd = line->command();
	KEY_WORD key = FxScriptLine::keywords.keyNumber(cmd);
	switch (key) {
	case KW_WAIT:
		{
			qint64 delayMs = QtStaticTools::timeStringToMS(line->parameters());
			setEventTargetTimeRelative(delayMs);
		}
		return false;

	case KW_START:
		ok = executeCmdStart(line);
		break;

	case KW_STOP:
		ok = executeCmdStop(line);
		break;

	case KW_FADEIN:
		ok = executeFadeIn(line);
		break;

	case KW_FADEOUT:
		ok = executeFadeOut(line);
		break;

	case KW_LOOP:
		if (myState == EXEC_RUNNING) {
			ok = executeLoopExt(line);
		}
		break;

	case KW_YADI_DMX_MERGE:
		ok = executeYadiDMXMergeMode(line);
		break;

	case KW_GRAP_SCENE:
		ok = executeGrabScene(line);
		break;

	case KW_BLACK:
		ok = executeBlack(line);
		break;

	case KW_REMOTE:
		ok = executeRemote(line);
		break;

	case KW_FADE_VOL:
		ok = executeFadeVolume(line);
		break;

	case KW_MODE:
		ok = executeMode(line);
		break;

	case KW_PAUSE:
		/*ok = */executePause(line);
		return false;

	case KW_DMX:
		ok = executeDMX(line);
		break;

	case KW_DEFAULT:
		ok = executeDefault(line);
		break;

	case KW_FIX:
		ok = executeFix(line);
		break;

	default:
		ok = false;
		LOGERROR(tr("<font color=darkOrange>Command '%1' not supported by scripts</font>").arg(cmd));
		break;
	}

	if (!ok) {
		LOGERROR(tr("Script '<font color=#6666ff>%1</font>': Line #%2: <font color=darkOrange>Failed to execute script line</font> ('<font color=#6666ff>%3</font>')%4")
				 .arg(m_fxScriptItem ? m_fxScriptItem->name() : QString("no script name"),
					  QString::number(line->lineNumber()),
					  QString("%1 %2").arg(line->command(), line->parameters()),
					  m_lastScriptError.size() ? QString(": %1").arg(m_lastScriptError) : QString()));

		m_lastScriptError.clear();
	}

	return true;
}

bool ScriptExecuter::executeCmdStart(FxScriptLine *line)
{
	// Try to find the target fx here

	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	// qDebug() << Q_FUNC_INFO << restparas;

	bool ok = true;

	foreach (FxItem *fx, fxlist) {
		switch (fx->fxType()) {
		case FX_AUDIO:
			{
				FxAudioItem *fxa = static_cast<FxAudioItem*>(fx);
				int pos = getPos(restparas);
				// qDebug() << "pos" << pos << "restparas" << restparas;

				if (fxa->isFxClip) {
					ok &= myApp.unitVideo->startFxClip(static_cast<FxClipItem*>(fx));
				}
				else if (pos >= -1) {
					ok &= myApp.unitAudio->startFxAudio(fxa, this, pos);
				}
				else {
					ok &= myApp.unitAudio->startFxAudio(fxa, this);
				}
			}
			break;
		case FX_SCENE:
			/// @todo it would be better to copy the scene and actually fade in the new instance here
			/// Further the executer is not handed over to scene
			ok &= myApp.unitLight->startFxScene(static_cast<FxSceneItem*>(fx));
			break;
		default:
			LOGERROR(tr("Script '%1': Executing of target is not supported! Line #%2: %3 %4")
					 .arg(m_fxScriptItem ? m_fxScriptItem->name() : QString("no script name"))
					 .arg(line->lineNumber())
					 .arg(line->command(), line->parameters()));
		}
	}

	return ok;
}

bool ScriptExecuter::executeCmdStop(FxScriptLine *line)
{
	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	bool ok = true;

	foreach (FxItem *fx, fxlist) {

		switch (fx->fxType()) {
		case FX_SCENE:
			{
				FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
				if (!scene->isOnStageIntern()) {
					LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': Target '%2' not on stage. Line %3")
							.arg(originFxItem->name(),
								 scene->name(),
								 QString::number(line->lineNumber())));
					FxItemList fxlist = getExecuterTempCopiesOfFx(fx);
					foreach (FxItem *fx_cp, fxlist) {
						FxSceneItem *fxsc_cp = dynamic_cast<FxSceneItem*>(fx_cp);
						if (fxsc_cp) {
							LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': Stop temp copy of '%2' -> '%3'")
									.arg(originFxItem->name(),
										 scene->name(),
										 fxsc_cp->name()));
							myApp.unitLight->stopFxScene(fxsc_cp);
						}
					}
				} else {
					LOGTEXT(tr("Script <font color=darkGreen>'%1'</font>: Stop FX '%2'")
							.arg(originFxItem->name(), scene->name()));
					return myApp.unitLight->stopFxScene(scene);
				}
			}
			break;

		case FX_AUDIO:
			if (myApp.unitAudio->stopFxAudioWithID(fx->id())) {
				LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': Stopped FX audio '%2'")
						.arg(originFxItem->name(), fx->name()));
			}
			else {
				LOGTEXT(tr("<font color=darkGreen>Script</font> '%1': FX audio '%2' not stopped, cause is already idle")
						.arg(originFxItem->name(), fx->name()));
			}
			break;

		default:
			LOGERROR(tr("Script '%1': Execute stop command on target is not supported! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
		}

	}

	if (restparas.size()) {
		LOGERROR(tr("Script '%1': Command STOP does not support parameters: '%2'! Line #%3")
				 .arg(m_fxScriptItem->name(),
					  restparas,
					  QString::number(line->lineNumber())));
				ok = false;
	}
	return ok;
}

bool ScriptExecuter::executeFadeIn(FxScriptLine *line)
{
	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	qint64 fadein_time_ms = 0;
	// Do we have more parameters? This could be the fadein time
	if (restparas.size()) {
		fadein_time_ms = QtStaticTools::timeStringToMS(restparas);
	}

	bool ok = true;
	foreach (FxItem *fx, fxlist) {

		FxSceneItem *scene = dynamic_cast<FxSceneItem*>(fx);
		if (!scene) {
			LOGERROR(tr("Script '%1': FADEIN must be applied on a SCENE! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
			continue;
		}

		FxItemList clonelist = getExecuterTempCopiesOfFx(scene);

		// now clone FxSceneItem
		FxSceneItem *clonescene = nullptr;

		if (clonelist.size()) {
			clonescene = dynamic_cast<FxSceneItem*>(clonelist.first());
		} else {
			clonescene = new FxSceneItem(*scene);
			clonescene->setName(tr("%1:%2_tmp").arg(originFxItem->name(), scene->name()));
			clonescene->setIsTempCopyOf(scene);
			m_clonedSceneList.append(clonescene);
		}
		if (!clonescene)
			continue;

		bool is_active = clonescene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEIN, fadein_time_ms);
		if (is_active) {
			myApp.unitLight->setSceneActive(clonescene);
		} else {
			m_lastScriptError = tr("not active");
		}
		ok &= is_active;
	}

	return ok;
}

bool ScriptExecuter::executeFadeOut(FxScriptLine *line)
{
	FxItemList fxlist;
	QString restparas = getTargetFxItemFromPara(line, line->parameters(), fxlist);
	if (fxlist.isEmpty()) return false;

	qint64 fadeout_ms = 0;
	bool fadeout_all = false;
	// Do we have more parameters? This could be the fadeout time
	if (restparas.size()) {
		if (restparas.startsWith("all", Qt::CaseInsensitive)) {
			fadeout_all = true;
			restparas = restparas.mid(3).simplified();
		}
		fadeout_ms = QtStaticTools::timeStringToMS(restparas);
	}


	bool ok = true;
	foreach (FxItem *fx, fxlist) {

		if (fx->fxType() == FX_AUDIO) {
			FxAudioItem *fxa = dynamic_cast<FxAudioItem*>(fx);
			if (fxa) {
				if (fadeout_ms > 0) {
					myApp.unitAudio->fadeoutFxAudio(fxa, fadeout_ms);
				} else {
					myApp.unitAudio->stopFxAudio(fxa);
				}
			}
			continue;
		}

		FxSceneItem *scene = dynamic_cast<FxSceneItem*>(fx);
		if (!scene) {
			LOGERROR(tr("Script '%1': FADEOUT must be applied on a SCENE! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
			continue;
		}

		FxItemList clonelist;
		if (fadeout_all) {
			clonelist = FxItem::getTempCopiesOfFx(scene);
			clonelist.append(scene);
		} else {
			clonelist = getExecuterTempCopiesOfFx(scene);
		}
		foreach (FxItem *clonefx, clonelist) {
			FxSceneItem *clonescene = dynamic_cast<FxSceneItem*>(clonefx);
			if (!clonescene)
				continue;

			bool is_active = clonescene->initSceneCommand(MIX_INTERN, CMD_SCENE_FADEOUT, fadeout_ms);
			if (! (is_active | fadeout_all) ) {
				ok = false;
				m_lastScriptError = tr("not active");
			}
		}
	}

	return ok;
}

bool ScriptExecuter::executeYadiDMXMergeMode(FxScriptLine *line)
{
	QStringList paras = line->parameters().split(" ", QT_SKIP_EMPTY_PARTS);

	if (paras.size() < 2) {
		m_lastScriptError = tr("Missing parameter. Format is: DMXMERGE <input> <mergemode>");
		return false;
	}

	quint32 input = paras[0].toInt()-1;
	if (input > 3) {
		m_lastScriptError = tr("Input number should be in range of 1 - 4");
		return false;
	}
	int mode = 0;
	QString mtxt = paras[1].toLower();

	if (mtxt == "htp" || mtxt.startsWith("def")) {
		mode = 0;
	}
	else if (mtxt == "dmxin") {
		mode = 3;
	}
	else if (mtxt == "usb") {
		mode = 4;
	}
	else {
		m_lastScriptError = tr("Unknown merge mode %1. Try HTP, DMXIN or USB").arg(paras[1]);
		return false;
	}

	bool ok = myApp.unitLight->setYadiInOutMergeMode(input, mode);
	if (!ok) {
		m_lastScriptError = tr("Could not send command to yadi device!");
	}

	return ok;
}

bool ScriptExecuter::executeLoopExt(FxScriptLine *line)
{
	line->incLoopCount();

	QStringList paras = line->parameters().split(" ", QT_SKIP_EMPTY_PARTS);
	if (paras.isEmpty()) { // simple loop without parameters from beginning
		m_currentLineNum = -1;
		return true;
	}

	QString targetline_str = paras.takeFirst();
	int linenum = targetline_str.toInt();
	if (linenum == 0 || linenum > m_script.size()) {
		m_lastScriptError = tr("Target line number not valid");
		return false;
	}

	if (paras.isEmpty()) {
		m_currentLineNum = linenum - 2;
		return true;
	}

	// evaluate loop count;
	QString targetcnt_str = paras.takeFirst();
	int targetcount = targetcnt_str.toInt();
	if (targetcount == 0) {
		m_lastScriptError = tr("Target loop count not valid");
		return false;
	}

	if (line->loopCount() >= targetcount) {			// loop count reached
		line->clearLoopCount();
	} else {
		m_currentLineNum = linenum - 2;
	}

	return true;
}

bool ScriptExecuter::executeGrabScene(FxScriptLine *line)
{
	QString parastr = line->parameters();

	QString source = getFirstParaOfString(parastr).toLower();
	if (!source.startsWith("in") && !source.startsWith("out")) {
		m_lastScriptError = tr("Missing universe source parameter: [INPUT | OUTPUT]");
		return false;
	}

	FxItemList fxlist;
	parastr = getTargetFxItemFromPara(line, parastr, fxlist);
	if (fxlist.isEmpty()) {
		m_lastScriptError = tr("Missing target scene parameter");
		return false;
	}

	for (FxItem *fx : fxlist) {
		FxSceneItem *scene = dynamic_cast<FxSceneItem*>(fx);
		if (!scene) {
			LOGERROR(tr("Script '%1': GRAPSCENE must be applied on a SCENE! Line #%2")
					 .arg(m_fxScriptItem->name())
					 .arg(line->lineNumber()));
			continue;
		}

		int backActiveChannels = 0;
		myApp.unitLight->fillSceneFromInputUniverses(scene, &backActiveChannels);
		if (backActiveChannels == 0) {
			LOGTEXT(tr("Grab into scene '%1': No tube was active -> set last tube to 1!")
					.arg(scene->name()));
			scene->tube(scene->tubeCount()-1)->targetValue = 1;
		}
		scene->setModified(false);
	}

	return true;
}

bool ScriptExecuter::executeBlack(FxScriptLine *line)
{
	QString parastr = line->parameters();

	QString type = getFirstParaOfString(parastr).toLower();

	int timems = 0;
	bool black_video = false;
	bool black_scene = false;

	if (type == "video") {
		black_video = true;
	}
	else if (type == "scene" || type == "light") {
		black_scene = true;
	}
	else if (type == "all") {
		black_video = true;
		black_scene = true;
	}
	else {
		m_lastScriptError = tr("Unknown parameter type '%1' for BLACK command. (use 'scene','light', 'video' or 'all'").arg(type);
		return false;
	}

	if (parastr.size())
		timems = QtStaticTools::timeStringToMS(parastr);

	if (black_video) {
		if (myApp.unitVideo) {
			myApp.unitVideo->videoBlack(timems);
		}
	}
	if (black_scene) {
		myApp.unitLight->black(timems);
	}

	return true;
}

bool ScriptExecuter::executeRemote(FxScriptLine *line)
{
	QString parastr = line->parameters();
	QString host = getFirstParaOfString(parastr);

	myApp.connectToRemote(host, NET_TCP_PORT, parastr);

	return true;
}

bool ScriptExecuter::executeFadeVolume(FxScriptLine *line)
{
	QString parastr = line->parameters();

	int slotno = 0;
	int targetVol = 0;
	int fadeTimeMs = 0;

	QString type = getFirstParaOfString(parastr).toLower();
	if (type == "slot") {
		slotno = getFirstParaOfString(parastr).toInt();
	}
	else {
		m_lastScriptError = tr("Unknown parameter type '%1' for FADEVOL command: (use [slot])").arg(type);
		return false;
	}

	if (!slotno) {
		m_lastScriptError = tr("Missing slot number in FADEVOL command");
		return false;
	}
	slotno--;

	QString targetVolStr = getFirstParaOfString(parastr);
	if (targetVolStr.endsWith('%')) {
		targetVolStr.chop(1);
		targetVol = targetVolStr.toInt();
		if ((targetVol < 0) || (targetVol > 100)) {
			m_lastScriptError = tr("Volume target out of range: %1. Must be [0%:100%]").arg(targetVol);
			return false;
		}
		targetVol = targetVol * MAX_VOLUME / 100;
	} else {
		targetVol = targetVolStr.toInt();
		if ((targetVol < 0) || (targetVol > MAX_VOLUME)) {
			m_lastScriptError = tr("Volume target out of range: %1. Must be [0:%2]").arg(targetVol).arg(MAX_VOLUME);
			return false;
		}
	}

	if (!parastr.isEmpty())
		fadeTimeMs = QtStaticTools::timeStringToMS(parastr);

	myApp.unitAudio->fadeVolTo(slotno, targetVol, fadeTimeMs);

	return true;
}

bool ScriptExecuter::executeMode(FxScriptLine *line)
{
	const QStringList paras = line->parameters().split(',');
	bool ok = true;

	for (const QString &opt : paras) {
		QString lc = opt.toLower();
		if (lc == "breakoncancel") {
			m_breakOnCancel = true;
		}
		else if (lc == "disablemultistart") {
			m_disableMultiStart = true;
		}
		else if (lc == "enablemultistart") {
			m_disableMultiStart = false;
		}
		else {
			m_lastScriptError += tr("Unknown MODE option: %1\n").arg(lc);
			ok = false;
		}
	}

	if (!ok)
		m_lastScriptError.chop(1);
	return ok;
}

bool ScriptExecuter::executePause(FxScriptLine *line)
{
	Q_UNUSED(line)

	m_pausedAtMs = runTimeOne.elapsed();
	setPaused(true);
	runTime.stop();
	runTimeOne.stop();

	return true;
}

bool ScriptExecuter::executeDMX(FxScriptLine *line)
{
	QString parastr = line->parameters();
	QString dmxtarget = getFirstParaOfString(parastr);

	int uni = m_defUniverse;			// default DMX universe 1 (intern 0:MAX_UNIVERSE-1, user [1:MAX-UNIVERSE])
	int sceneNum = m_defStaticSceneNum;	// default static scene number

	// DMX target should look like this dmx <UNIVERSE>::<SCENE>:<DMXCHANNEL> e.g. 1::1:100 for Universe 1, Scene 1, DMX 100
	// universe or scene could be ommitted

	// first extract universe number, if there is one
	if (dmxtarget.contains("::")) {
		QString unistr = dmxtarget.section("::", 0, 0);
		uni = unistr.toInt() - 1;
		dmxtarget = dmxtarget.section("::", 1);

		if (uni < 0 || uni >= MAX_DMX_UNIVERSE) {
			m_lastScriptError += tr("Invalid DMX universe: %1\n").arg(unistr);
			return false;
		}
	}

	// second extract scene number
	if (dmxtarget.contains(':')) {
		QString scenestr = dmxtarget.section(':', 0, 0);
		sceneNum = scenestr.toInt() - 1;
		dmxtarget = dmxtarget.section(':', 1);

		if (sceneNum < 0 || sceneNum >= MAX_STATIC_SCENES)  {
			m_lastScriptError += tr("Invalid static scene number: %1\n").arg(scenestr);
			return false;
		}
	}

	// next must be the DMX channel number
	int dmxchan = dmxtarget.toInt() - 1;
	if (dmxchan < 0 || dmxchan > 511) {
		m_lastScriptError += tr("Invalid DMX channel number: %1\n").arg(dmxtarget);
		return false;
	}

	// now get DMX value from command
	QString dmxvaluestr = getFirstParaOfString(parastr);
	int dmxval = dmxvaluestr.toInt();
	if (dmxval < 0 || dmxval > 255) {
		m_lastScriptError += tr("Invalid DMX value: %1\n").arg(dmxvaluestr);
		return false;
	}

	// get fade time from command
	int fadems = m_defFadeTimeMs;
	if (parastr.size() > 2) {
		QString fadestr = getFirstParaOfString(parastr);
		fadems = QtStaticTools::timeStringToMS(fadestr);
	}

	// get scene instance.
	FxSceneItem *fxs = myApp.unitLight->staticScene(uni, sceneNum);
	DmxChannel *tube = fxs->tube(dmxchan);
	qint32 target_value = (float(dmxval) + 0.5) * tube->targetFullValue / 255;

	tube->targetValue = qMin(target_value, tube->targetFullValue);
	/*bool fadeactive = */tube->initFadeCmd(MIX_EXTERN, CMD_SCENE_FADETO, fadems, target_value);

	return true;
}

bool ScriptExecuter::executeDefault(FxScriptLine *line)
{
	QString parastr = line->parameters();

	bool ok = true;
	while (!parastr.isEmpty()) {
		bool paraok;
		QString subcmd = getFirstParaOfString(parastr).toLower();
		if (subcmd == "universe" || subcmd == "uni") {
			int val = getFrontIntOfString(parastr, &paraok);
			if (!paraok)
				return false;
			if ( (val < 1) || (val > MAX_DMX_UNIVERSE) ) {
				m_lastScriptError += tr("universe number out of valid range: %1\n").arg(val);
				return false;
			}

			m_defUniverse = val - 1;
		}
		else if (subcmd == "staticscene") {
			int val = getFrontIntOfString(parastr, &paraok);
			if (!paraok)
				return false;
			if ( (val < 1) || (val > MAX_STATIC_SCENES) ) {
				m_lastScriptError += tr("static scene number out of valid range: %1 (max is: %2)\n")
						.arg(val).arg(MAX_STATIC_SCENES);
				return false;
			}

			m_defStaticSceneNum = val - 1;
		}
		else if (subcmd == "fadetime") {
			QString timestr = getFirstParaOfString(parastr);
			if (timestr.isEmpty())  {
				m_lastScriptError += tr("Missing fadetime parameter");
				return false;
			}

			m_defFadeTimeMs = QtStaticTools::timeStringToMS(timestr);
		}
		else {
			m_lastScriptError += tr("Unknown option: %1").arg(subcmd);
			return false;
		}

		// remove commata from beginning of parastr
		/// @todo
	}

	return ok;
}

bool ScriptExecuter::executeFix(FxScriptLine *line)
{
	QString parastr = line->parameters();

	int sceneNum = m_defStaticSceneNum;	// default static scene number

	// get fixture name
	QString shortId = getFirstParaOfString(parastr);
	if (shortId == ':')
		shortId = m_defFixShortId;

	if (shortId.isEmpty()) {
		m_lastScriptError += tr("Fixture short ident missing\n");
		return false;
	}

	int universe = -1;
	if (shortId.contains(':')) {
		universe = shortId.section(':',0,0).toInt()-1;
		shortId = shortId.section(':',1);
	}
	else {
		universe = m_defUniverse;
	}

	qint32 dmxaddr = myApp.fixtureDmxAddrForShortIdent(shortId, universe);
	if (dmxaddr <= 0) {
		m_lastScriptError += tr("DMX address for short id %1 not found in universe %2")
				.arg(shortId, universe < 0 ? "all" : QString::number(universe + 1));
		return false;
	}
	qDebug() << "fix" << shortId << "dmx" << dmxaddr;


	// default values for possible parameters
	int w = -1;	// white
	int r = -1;	// red
	int g = -1;	// green
	int b = -1;	// blue

	bool flagDim = false;

	QStringList paras = parastr.split(' ');
	// get sub command
	while (!paras.isEmpty()) {
		QString subcmd = paras.takeFirst().toLower();

		if (paras.isEmpty()) {
			m_lastScriptError += tr("Missing option value for '%1'").arg(subcmd);
			return false;
		}
		if (subcmd == 'w') {
			w = convertToDmxVal(paras.takeFirst());
		}
		else if (subcmd == 'r') {
			r = convertToDmxVal(paras.takeFirst());
		}
		else if (subcmd == 'g') {
			g = convertToDmxVal(paras.takeFirst());
		}
		else if (subcmd == 'b') {
			b = convertToDmxVal(paras.takeFirst());
		}
	}

	if (w >= 0) {
		qDebug() << "white" << w;
	}
	if (r >= 0) {
		qDebug() << "red" << r;
	}
	if (g >= 0) {
		qDebug() << "green" << g;
	}
	if (b >= 0) {
		qDebug() << "blue" << b;
	}


	return true;
}

bool ScriptExecuter::executeSingleCmd(const QString &linestr)
{
	QString cmd = linestr.section(' ',0,0);
	QString paras = linestr.section(' ',1);
	FxScriptLine line(cmd, paras);
	ScriptExecuter exe(AppCentral::ref(), nullptr, nullptr);

	bool reexecdelayed = false;
	bool ok = exe.executeLine(&line, reexecdelayed);
	if (!ok) {
		LOGERROR(tr("Execute script command failed: <font color=darkOrange>Target parameter missing!</font>")
				 .arg(exe.lastScriptError()));
	}

	return ok;
}
