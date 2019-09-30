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

#include "project.h"
#include "config.h"
#include "log.h"
#include "appcentral.h"
#include "fxitemtool.h"

#include "../fx/fxlist.h"
#include "../fx/fxitem.h"
#include "../fx/fxsceneitem.h"
#include "../fx/fxaudioitem.h"
#include "../fx/fxplaylistitem.h"

#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>



namespace FXM {
KEY_TXT keyTab[104] = {
	{0,"-=-",0},{65470,"F1",16777264},{65471,"F2",16777265},{65472,"F3",16777266},{65473,"F4",16777267},
	{65474,"F5",16777268},{65475,"F6",16777269},{65476,"F7",16777270},{65477,"F8",16777271},
	{65478,"F9",16777272},{65479,"F10",16777273},{65480,"F11",16777274},{65481,"F12",16777275},
	{49," 1 ",49},{50," 2 ",50},{51," 3 ",51},{52," 4 ",52},{53," 5 ",53},
	{54," 6 ",54},{55," 7 ",55},{56," 8 ",56},{57," 9 ",57},{48," 0 ",48},{223," ß ",223},
	{113," Q ",81},{119," W ",87},{101," E ",69},{114," R ",82},{116," T ",84},{122," Z ",90},
	{117," U ",85},{105," I ",73},{111," O ",79},{112," P ",80},{97," A ",65},{115," S ",83},
	{94," ^ ",94},{100," D ",68},{102," F ",70},{103," G ",71},{104," H ",72},{106," J ",74},
	{107," K ",75},{108," L ",76},{121," Y ",89},{120," X ",88},{99," C ",67},{118," V ",86},
	{98," B ",66},{110," N ",78},{109," M ",77},{44," , ",44},{46," . ",46},{45," - ",45},
	{16777222,"ZB0",0},{65436,"ZB1",16777233},{65433,"ZB2",16777237},{65435,"ZB3",16777239},{65430,"ZB4",16777234},
	{65437,"ZB5",16777227},{65432,"ZB6",16777236},{65429,"ZB7",16777232},{65431,"ZB8",16777235},{65434,"ZB9",16777238},
	{65455,"ZB/",47},{65450,"ZB*",42},{65453,"ZB-",45},{65451,"ZB+",43},{65421,"ENT",16777221},
	{65439,"ZBd",16777223},{65377,"PRI",0},{65300,"ROL",16777254},{65299,"PAU",16777224},
	{65379,"INS",16777222},{65360,"POS",16777232},{65365," UP",16777238},{65535,"DEL",16777223},{65367,"END",16777233},{65366,"DOW",16777239},
	{252," Ü ",110},{43," + ",43},{246," Ö ",214},{228," Ä ",196},{35," # ",35},{-1,"xxx",-1}
};
}



Project::Project()
	: QObject()
	, VarSet()
{
	init();
	clear();
}

Project::Project(const Project &o, bool keepAllFxIitemIDs)
	: QObject()
	, VarSet()
{
	init();
	cloneProjectFrom(o, keepAllFxIitemIDs);
}

Project::~Project()
{
	delete fxList;
}

bool Project::cloneProjectFrom(const Project &o, bool keepAllFxIitemIDs)
{
	pAutoProceedSequence = o.pAutoProceedSequence;
	curProjectFilePath = o.curProjectFilePath;
	loadErrorLineNumber = 0;  // o.loadErrorLineNumber;
	loadErrorLineString = QString(); // o.loadErrorLineString;
	loadErrorFileNotExisting = false;

	pProjectId = o.pProjectId;
	pProjectFormat = o.pProjectFormat;
	pProjectName = o.pProjectName;
	pComment = o.pComment;


	FxIdMap idmap;

	int mode = 1;
	if (keepAllFxIitemIDs)
		mode = 2;

	bool ok = fxList->copyFrom(*o.fxList, mode, &idmap);

	return ok;
}

void Project::clear()
{
	loadErrorFileNotExisting = false;
	loadErrorLineNumber = 0;
	loadErrorLineString.clear();

	pProjectName = "Default Project";
	pProjectId = QDateTime::currentDateTime().toTime_t();
	pProjectFormat = 0;

	if (fxList->size()) {
		fxList->clear();
		fxList->setModified(false);
	}

	fxList->showColumnKeyFlag = true;
}

bool Project::saveToFile(const QString &path)
{
	pProjectFormat = PROJECT_FORMAT;
	QString modpath = path;
	if (modpath.toLower().endsWith(".fxm")) {
		modpath.chop(4);
		modpath.append(".srp");
	}

	bool ok = fileSave(modpath, false, true);

	if (ok) {
		curProjectFilePath = modpath;
		fxList->setModified(false);
		setModified(false);
		generateProjectNameFromPath();
		emit projectLoadedOrSaved(modpath, ok);
	}

	return ok;
}

bool Project::loadFromFile(const QString &path)
{
	LOGTEXT(tr("<font color=green>Load project</font> '%1'").arg(path));
	if (path.toLower().endsWith(".fxm")) {
		LOGTEXT(tr("<font color=orange> -> maybe fxMaster project file"));
		return loadFxMasterProject(path);
	}

	int line_number = 0;
	QString line_copy;
	bool file_exists;

	clearCurrentVars();
	setFileLoadCancelOnEmptyLine(false);

	bool ok = fileLoad(path,&file_exists,&line_number,&line_copy);

	if (pProjectFormat >= 2) {
		fxList->refAllMembers();
		if (!ok) {
			loadErrorLineNumber = line_number;
			loadErrorLineString = line_copy;
			loadErrorFileNotExisting = !file_exists;
		}
	}
	else {
		// this is old stuff for project file versions < 2 !!

		// Now try to load FxItem VarSets (with classname "FxItem")
		QFile file(path);
		FxItem *fx = 0;
		if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
			QTextStream read(&file);
			while (!read.atEnd() && ok) {
				int child_level = 0;
				int ret = analyzeLine(read,fx,child_level,&line_number,&line_copy);
				if (ret < 0) {
					qDebug() << "Project::loadFromFile: AnalyzeLine failed while searching for fxItem";
					ok = false;
				}
				// Wait for ClassName matching FxItem Type
				if (curClassname == "FxItem" && !curChildActive) {
					// An opening bracket could mean a new instance will come
					if (curKey.startsWith('[') && !curKey.startsWith("[CHILDLIST]") ) fx = 0;
					// We have to wait for FxType to determine what kind of Fx we
					// have to initialize
					if (curKey == "FxType") {
						fx = fxList->addFx(curValue.toInt());
						if (debug > 2) DEBUGTEXT("Added FxItem Type:%d to Fx list -> analyze sub class",fx->fxType());
					}
				}
			}
			file.close();

			fxList->refAllMembers();
		} else {
			ok = false;
			loadErrorLineNumber = line_number;
		}
	}

	if (ok) {
		curProjectFilePath = path;
		generateProjectNameFromPath();

		// check project consistance and health
		EXPORT_RESULT result;
		ok = checkFxItemList(fxList, result);
		if (!ok) {
			QString msg = QString("Project load was ok, but <font color=darkOrange>consistance check found some errors</font>:<br>");
			for (const QString &txt : result.errorMessageList) {
				msg += QString("- %1<br>").arg(txt);
			}
			POPUPINFOMSG("Load project",msg);
			ok = true;
		}
		else if (result.resultMessageList.size()) {
			QString msg = QString("<font color=green>Project loaded successfully!</font><br>");
			for (const QString &txt : result.resultMessageList) {
				msg += QString("- %1<br>").arg(txt);
			}
			POPUPINFOMSG("Load project",msg);
		}

		setModified(result.setModified);
		emit projectLoadedOrSaved(path, ok);
	}

	return ok;
}

bool Project::isModified()
{
	bool modified = false;
	if (fxList->isModified()) {
		qDebug("Project:: fxList is modified");
		modified = true;
	}
	if (VarSet::isModified()) {
		qDebug("Project:: Project is modified");
		modified = true;
	}

	return modified;
}

void Project::setModified(bool state)
{
	VarSet::setModified(state);
}


/**
 * @brief Initialize all FxItems after loading a project
 * @return
 */
void Project::postLoadProcessFxList()
{
	fxList->postLoadProcess();
}

/**
 * @brief Reset Scene output values after loading a project
 * @return  true, a scene was on stage
 *
 * When saving the project at the same time all current scene (tubes) output values are stored as well.
 * Loading this scenes will restore these states. That could cause problems on fading in the scene (cause
 * it maybe already faded to the target values)
 */
bool Project::postLoadResetFxScenes()
{
	return fxList->postLoadResetScenes();
}

bool Project::consolidateToDir(const QString &exportProName, const QString &dirname, EXPORT_RESULT &result, QWidget *parentWid)
{
	QString where = tr("Consolidate");

	// First we clone the current project in order to have a playground without messing up the project.
	Project tpro(*this, result.forceKeepFxItemIDs);

	QString proname = exportProName;
	proname.remove(".srp");
	proname.remove(".SRP");

	tpro.pComment = QString("Consolidated from: %1, at: %2")
			.arg(curProjectFilePath).arg(QDateTime::currentDateTime().toString());
	tpro.pProjectId = QDateTime::currentDateTime().toTime_t();
	tpro.pProjectName = proname;


	QString exportbasedir = QString("%1/SRP_%2").arg(dirname, proname);
	if (QFile::exists(exportbasedir)) {
		POPUPERRORMSG(where
					  ,tr("There is already a consolidated project\n with the name \"%1\""
						  "in directory \"%2\"")
					  .arg(proname, dirname)
					  ,parentWid);
		return false;
	}

	tpro.pProjectBaseDir = exportbasedir;

	QDir dir(exportbasedir);
	if (!dir.mkpath(exportbasedir)) {
		POPUPERRORMSG(where
					  ,tr("Could not create target directory '%1'!")
					  .arg(exportbasedir)
					  ,parentWid);
		return false;
	}

	if (!dir.exists("audio"))
		dir.mkdir("audio");

	QString audiodir = dir.absolutePath() + QDir::separator() + "audio";
	bool ok = tpro.copyAllAudioItemFiles(tpro.fxList, audiodir, result);

	// save project file
	QString destProFilePath = QString("%1/%2.srp").arg(exportbasedir, proname);
	ok &= tpro.saveToFile(destProFilePath);

	QString msg;
	if (ok) {
		msg = tr("Project export <font color=green><b>successfully</b></font>!<br><br>");
	} else {
		msg = tr("<font color=darkOrange>An error occured</font> during export<br>");
	}

	msg += tr("Audio files consolidated: %1<br>").arg(result.audioFileCopyCount);
	msg += tr("Audio files references: %1<br>").arg(result.audioFileExistCount);

	msg += tr("Project file location: <font color=#8888ff>%1</font><br>").arg(destProFilePath);

	if (ok) {
		POPUPINFOMSG(where,msg,parentWid);
	} else {
		POPUPERRORMSG(where,msg,parentWid);
	}
	return ok;
}

bool Project::copyAllAudioItemFiles(FxList * srcFxList, const QString &destDir, EXPORT_RESULT &result)
{
	emit progressWanted(0, srcFxList->size());
	bool ok = true;
	for (int t=0; t<srcFxList->size(); t++) {
		emit progressed(t);
		FxItem *fx = srcFxList->at(t);
		if (fx->fxType() == FX_AUDIO) {
			FxAudioItem *fxa = dynamic_cast<FxAudioItem*>(fx);
			if (!fxa) continue;
			QString srcpath = fxa->filePath();

			qDebug() << "export audio" << fxa->filePath();

			QString basename = QFileInfo(fxa->fileName()).completeBaseName();
			QString suffix = QFileInfo(fxa->fileName()).suffix();

			QString destpath;
			bool done = false;
			bool copyme = false;
			int suffix_num = 0;
			while (!done) {
				destpath = QString("%1/%2%3.%4")
						.arg(destDir)
						.arg(basename)
						.arg(suffix_num>0 ? QString::number(suffix_num) : QString())
						.arg(suffix.toLower());

				if (QFile::exists(destpath)) {
					if (QFileInfo(destpath).size() == QFileInfo(srcpath).size()) {
						// file does already exist in target dir but size is the same
						// we assume that this file contains the same content.
						done = true;
						result.audioFileExistCount++;
					} else {
						// change target file name, since there is already a file with this name
						suffix_num++;
					}
				} else {
					copyme = true;
					done = true;
				}
			}

			// Change filepath to new location
			if (result.exportWithRelativeFxFilePaths) {
				QString basedir = pProjectBaseDir;
				QString relpath = destpath;
				if (basedir.size() && relpath.startsWith(basedir)) {
					relpath.remove(0,basedir.size());
					if (relpath.startsWith("/"))
						relpath.remove(0,1);
				}
				fxa->setFilePath(relpath);
			} else {
				fxa->setFilePath(destpath);
			}
			qDebug() << "  -->" << fxa->filePath();

			if (copyme) {
				QFile file(srcpath);
				if (!file.copy(destpath)) {
					ok = false;
					result.errorMessageList.append(tr("Could not create audio file %1 (%2)")
												   .arg(destpath)
												   .arg(file.errorString()));
				} else {
					result.audioFileCopyCount++;
				}
			}
		}
		else if (fx->fxType() == FX_AUDIO_PLAYLIST) {
			FxPlayListItem *fxp = dynamic_cast<FxPlayListItem*>(fx);
			if (!fxp) {
				ok = false;
				continue;
			}

			// call export function recursively for list in FxPlayListItem
			ok &= copyAllAudioItemFiles(fxp->fxPlayList, destDir, result);
		}
	}

	if (!ok)
		result.wasCompletelySuccessful = false;

	return ok;
}

/**
 * @brief Check all FxItems belonging to the project for proper init state;
 * @param srcFxList
 * @param result
 * @return true, if ALL FxItems found to be healthy and functional
 *
 */
bool Project::checkFxItemList(FxList *srcFxList, Project::EXPORT_RESULT &result)
{
	bool ok = true;
	bool always_open_find_file_dialog = false;
	bool never_open_find_file_dialog = false;

	for (int t=0; t<srcFxList->size(); t++) {
		FxItem *fx = srcFxList->at(t);
		if (fx->fxType() == FX_AUDIO) {

			// check if media file exists
			QString filepath = fx->filePath();
			bool foundfile = QFile::exists(filepath);
			if (!foundfile && pProjectBaseDir.size() && !filepath.startsWith("/")) {
				QFileInfo fi(curProjectFilePath);
				QString pbasedir = fi.absoluteDir().path();
				QString fullpath = QString("%1/%2").arg(pbasedir, filepath);
				foundfile = QFile::exists(fullpath);
				if (foundfile) {
					fx->setFilePath(fullpath);
					result.setModified = true;
				}
			}

			if (!foundfile) {
				// try to find in media dirs first.
				for (const QString &dirname : m_mediaFileSearchDirs) {
					QString newpath = QString("%1/%2").arg(dirname, fx->fileName());
					if (QFile::exists(newpath)) {
						foundfile = true;
						fx->setFilePath(newpath);
						break;
					}
				}

				if (foundfile) {
					result.resultMessageList.append(tr("Auto corrected file path for FX: %1 to %2")
													.arg(fx->name(),fx->filePath()));
				}
				else if (always_open_find_file_dialog) {
					foundfile = showFindFileDialog(fx);
				}
				else if (never_open_find_file_dialog) {

				}
				else {
					int ret = 0;
					foundfile = askForFindFileDialog(fx, &ret);
					if (ret == QMessageBox::YesToAll)
						always_open_find_file_dialog = true;
					else if (ret == QMessageBox::NoToAll)
						never_open_find_file_dialog = true;
				}

				if (!foundfile) {
					ok = false;
					result.errorMessageList.append(tr("%1: Could not found media file '%2' ")
												   .arg(fx->fxNamePath())
												   .arg(filepath));
				} else {
					result.setModified = true;
				}
			}
		}
		else if (fx->fxType() == FX_AUDIO_PLAYLIST) {
			FxPlayListItem *fxp = dynamic_cast<FxPlayListItem*>(fx);
			if (!fxp) {
				ok = false;
				continue;
			}

			// call check function recursively for list in FxPlayListItem
			ok &= checkFxItemList(fxp->fxPlayList, result);
		}
	}

	return ok;
}

bool Project::askForFindFileDialog(FxItem *fx, int *ret)
{

	QMessageBox dialog(AppCentral::ref().mainwinWidget);
	dialog.setTextFormat(Qt::RichText);
	dialog.setText(tr("Could not find media file: <font color=orange>%1</font>").arg(fx->fileName()));
	dialog.setInformativeText(tr("Do you want to search for it now?"));
	dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
	dialog.setDetailedText(tr("File is referenced by:\n"
							  "%1\n"
							  "Full media file path is:\n"
							  "%2\n").arg(fx->name(),fx->filePath()));

	int retval = dialog.exec();

	if (ret)
		*ret = retval;

	switch (retval) {
	case QMessageBox::Yes:
	case QMessageBox::YesAll:
		return showFindFileDialog(fx);

	default:
		break;
	}

	return false;
}

bool Project::showFindFileDialog(FxItem *fx)
{
	QFileInfo fi(fx->filePath());
	QStringList filters;
	filters << QString("Media file (*.%1)").arg(fi.suffix());
	filters << QString("All (*)");

	QFileDialog dialog(AppCentral::ref().mainwinWidget);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setNameFilters(filters);
	dialog.setDirectory(fi.absoluteDir());
	dialog.selectFile(fx->fileName());
	dialog.setOption(QFileDialog::ReadOnly);
	dialog.setWindowTitle(tr("Locate file: %1").arg(fx->fileName()));
	dialog.setLabelText(QFileDialog::Accept,tr("Apply to FX"));

	dialog.exec();

	QStringList sel = dialog.selectedFiles();
	if (sel.size()) {
		QString newpath = sel.first();
		if (QFile::exists(newpath)) {
			fx->setFilePath(newpath);
			QString newdir = QFileInfo(newpath).absolutePath();
			if (!m_mediaFileSearchDirs.contains(newdir))
				m_mediaFileSearchDirs.append(newdir);
			return true;
		}
	}

	return false;
}

bool Project::loadFxMasterProject(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		LOGERROR(tr("Failed to open file %1").arg(path));
		return false;
	}

	QDataStream in(&file);
	in.setByteOrder(QDataStream::LittleEndian);

	// Read version chung and test if this is an fxm file
	quint32 vers[4];
	for (int i=0; i<4; i++) {
		in >> vers[i];
	}
	if ((vers[0] != FXM::SAVE_CHUNK_TYPE_VERSION && vers[0] != FXM::old_SAVE_CHUNK_TYPE_VERSION) || vers[1] != sizeof(FXM::SAVE_CHUNK_VERSION)) {
		LOGERROR(tr("Not a fxMaster project file: %1").arg(path));
		return false;
	}

	// Read all chunks
	bool ok = true;
	while (!in.atEnd() && ok) {
		quint32 type;
		quint32 size;
		in >> type >> size;
		size -= 2 * sizeof(quint32);

		if (size > 2000) {
			LOGERROR("Chunk size too big!");
			ok = false;
			break;
		}

		switch (type) {
		case FXM::SAVE_CHUNK_TYPE_NULL:
		case FXM::old_SAVE_CHUNK_TYPE_NULL:
			break;
		case FXM::SAVE_CHUNK_TYPE_FX_AUDIO:
		case FXM::old_SAVE_CHUNK_TYPE_FX_AUDIO:
			ok = fxmLoadChunkAudio(in, size);
			break;
		case FXM::SAVE_CHUNK_TYPE_USERNAME:
		case FXM::old_SAVE_CHUNK_TYPE_USERNAME:
			ok = fxmLoadChunkUserName(in, size);
			break;
		case FXM::SAVE_CHUNK_TYPE_AUDIO_SETTING:
		case FXM::old_SAVE_CHUNK_TYPE_AUDIO_SETTING:
			ok = fxmLoadChunkAudioSetting(in, size);
			break;
		case FXM::SAVE_CHUNK_TYPE_DMX_SETTING:
		case FXM::old_SAVE_CHUNK_TYPE_DMX_SETTING:
			ok = fxmLoadChunkDMXSetting(in, size);
			break;
		default:
			break;
		}
	}

	return true;
}

bool Project::fxmLoadChunkName(QDataStream &in, QString &name)
{
	quint32 type;
	quint32 size;
	quint32 ext;
	in >> type >> size >> ext;

	if (type != FXM::SAVE_CHUNK_TYPE_FILENAME && type != FXM::old_SAVE_CHUNK_TYPE_FILENAME) {
		LOGERROR(QString("FX: SAVE_CHUNK_FILENAME expected while loading FX"));
		return false;
	}

	int namesize = size - (3*sizeof(quint32));
	QByteArray namedat(256,0);
	in.readRawData(namedat.data(),namesize);
	name = QString::fromLocal8Bit(namedat);
	return true;
}

bool Project::fxmLoadChunkAudio(QDataStream &in, quint32 size)
{
	FXM::FX mem;
	FXM::FX32BIT mem32;
	char *raw_p = reinterpret_cast<char*>(&mem) + 2*sizeof(quint32);
	char *raw_p32 = reinterpret_cast<char*>(&mem32) + 2*sizeof(quint32);

	fprintf(stderr, "chunk audio: size of mem: %lu, mem32: %lu, size of chunk: %d\n",sizeof(mem),sizeof(mem32),size);

	if (size > sizeof(mem)) {
		LOGERROR("FXM: Audio chunk size greater as expected!");
		return false;
	}

	mem.sc_type = FXM::SAVE_CHUNK_TYPE_FX_AUDIO;
	mem.sc_len = size;

	if (size <= 80) { // Maybe this was saved on a 32bit system
		if (in.readRawData(raw_p32, size) != int(size)) {
			LOGERROR("FXM: Audio could not read audio chunk from 32bit system");
			return false;
		}
		if (mem32.fx_name) {
			QString name;
			if (fxmLoadChunkName(in, name)) {
				qDebug() << "Loaded audio chunk" << name << "id" << mem32.fx_id;

				FxAudioItem *fxa = fxList->addFxAudioSimple(name.toLocal8Bit().data());
				fxa->setMyId(mem32.fx_id+1);
				fxa->initialVolume = mem32.fx_Audio_Vol * MAX_VOLUME / 128;
				fxa->loopTimes = mem32.fx_Loop;
				fxa->setKeyCode(fxmKeyToQtKey(mem32.fx_key));
			}
		}
	} else {
		// load data saved by 64bit system
		if (in.readRawData(raw_p, size) != int(size)) {
			LOGERROR("FXM: Audio could not read audio chunk");
			return false;
		}
		if (mem.fx_name) {
			QString name;
			if (fxmLoadChunkName(in, name)) {
				qDebug() << "Loaded audio chunk" << name << "id" << mem.fx_id;

				FxAudioItem *fxa = fxList->addFxAudioSimple(name.toLocal8Bit().data());
				fxa->setMyId(mem.fx_id+1);
				fxa->initialVolume = mem.fx_Audio_Vol * MAX_VOLUME / 128;
				fxa->loopTimes = mem.fx_Loop;
				fxa->setKeyCode(fxmKeyToQtKey(mem.fx_key));
			}
		}
	}

	return true;
}

bool Project::fxmLoadChunkUserName(QDataStream &in, quint32 size)
{
	int id;
	in >> id;

	size -= sizeof(quint32);
	QByteArray read(size+1, 0);
	in.readRawData(read.data(), size);
	qDebug() << "Found CHUNK_USERNAME:" << read.data() << "id:" << id;

	return true;
}

bool Project::fxmLoadChunkAudioSetting(QDataStream &in, quint32 size)
{
	QByteArray read(2000,0);
	in.readRawData(read.data(), size);


	return true;
}

bool Project::fxmLoadChunkDMXSetting(QDataStream &in, quint32 size)
{
	QByteArray read(2000,0);
	in.readRawData(read.data(), size);

	return true;
}

int Project::fxmKeyToQtKey(int keyidx)
{
	if (keyidx > 0 && keyidx < 104) {
		return FXM::keyTab[keyidx].qtkey;
	}
	return 0;
}

int Project::glibKeyToQtKey(int key)
{
	int i = 0;
	while (1) {
		int gkey = FXM::keyTab[i].glibkey;
		int qtkey = FXM::keyTab[i].qtkey;
		if (gkey == -1)
			break;

		if (gkey == key)
			return qtkey;

		i++;
	}

	return 0;
}


void Project::init()
{
	fxList = new FxList;

	setClass(PrefVarCore::PROJECT,"Project settings");
	setDescription("This VarSet holds configuration data for a single project");

	addExistingVar(pProjectId,"ProjectId");
	addExistingVar(pProjectFormat,"ProjectFormat");
	addExistingVar(pProjectName,"ProjectName");
	addExistingVar(pComment,"Comment");
	addExistingVar(pProjectBaseDir,"ProjectBaseDir");
	addExistingVar(pAutoProceedSequence,"FxListAutoProceedSequence");
	addExistingVar(fxList->showColumnIdFlag,"FxListShowId");
	addExistingVar(fxList->showColumnPredelayFlag,"FxListShowPreDelay");
	addExistingVar(fxList->showColumnFadeinFlag,"FxListShowFadeInTime");
	addExistingVar(fxList->showColumnHoldFlag,"FxListShowHoldTime");
	addExistingVar(fxList->showColumnFadeoutFlag,"FxListShowFadeOutTime");
	addExistingVar(fxList->showColumnPostdelayFlag,"FxListShowPostDelay");


	addExistingVarSetList(fxList->nativeFxList(),"MainFxList",PrefVarCore::FX_ITEM);
}

bool Project::generateProjectNameFromPath()
{
	if (!QString(pProjectName).startsWith("Default Project"))
		return false;

	if (curProjectFilePath.isEmpty())
		return false;

	QFileInfo fi(curProjectFilePath);
	QString name = fi.completeBaseName();
	pProjectName = name;
	return true;
}

