#include "project.h"
#include "config.h"
#include "log.h"

#include "../fx/fxlist.h"
#include "../fx/fxitem.h"
#include "fxsceneitem.h"

#include <QDateTime>
#include <QFile>

Project::Project()
	: VarSet()
{
	init();
	clear();
}

Project::~Project()
{
	delete fxList;
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

	bool ok = fileSave(path, false, true);

	if (ok) {
		curProjectFilePath = path;
		fxList->setModified(false);
		setModified(false);
	}


	return ok;
}

bool Project::loadFromFile(const QString &path)
{
	LOGTEXT(QObject::tr("<font color=green>Load project</font> '%1'").arg(path));

	int line_number = 0;
	QString line_copy;
	bool file_exists;

	clearCurrentVars();
	setFileLoadCancelOnEmptyLine(false);

	bool ok = fileLoad(path,&file_exists,&line_number,&line_copy);

	if (pProjectFormat >= 2) {
		if (!ok) {
			loadErrorLineNumber = line_number;
			loadErrorLineString = line_copy;
			loadErrorFileNotExisting = !file_exists;
		} else {
			setModified(false);
		}
		return ok;
	}

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
		curProjectFilePath = path;
		fxList->setModified(false);
		setModified(false);
	} else {
		ok = false;
		loadErrorLineNumber = line_number;
	}

	// FxSceneItem *scene = (FxSceneItem*)fxList->at(9);

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



void Project::init()
{
	fxList = new FxList;

	setClass(PrefVarCore::PROJECT,"Project settings");
	setDescription("This VarSet holds configuration data for a single project");

	addExistingVar(pProjectId,"ProjectId");
	addExistingVar(pProjectFormat,"ProjectFormat");
	addExistingVar(pProjectName,"ProjectName");
	addExistingVar(pComment,"Comment");
	addExistingVar(pAutoProceedSequence,"FxListAutoProceedSequence");
	addExistingVar(fxList->showColumnIdFlag,"FxListShowId");
	addExistingVar(fxList->showColumnPredelayFlag,"FxListShowPreDelay");
	addExistingVar(fxList->showColumnFadeinFlag,"FxListShowFadeInTime");
	addExistingVar(fxList->showColumnHoldFlag,"FxListShowHoldTime");
	addExistingVar(fxList->showColumnFadeoutFlag,"FxListShowFadeOutTime");
	addExistingVar(fxList->showColumnPostdelayFlag,"FxListShowPostDelay");


	addExistingVarSetList(fxList->nativeFxList(),"MainFxList",PrefVarCore::FX_ITEM);
}

