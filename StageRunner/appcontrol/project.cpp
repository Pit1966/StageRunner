#include "project.h"

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
	pProjectName = "Default Project";
	pProjectId = QDateTime::currentDateTime().toTime_t();

	if (fxList->size()) fxList->clear();
}

bool Project::saveToFile(const QString &path)
{
	bool ok = fileSave(path, false, true);

	if (ok) {
		curProjectFilePath = path;
		for (int t=0; t<fxList->size(); t++) {
			FxItem *fx = fxList->at(t);
			fx->setDatabaseReferences((qint64)pProjectId,t+1);
			// append to previously saved file
			ok = fx->fileSave(path, true, true);
		}

	}

	if (ok) {
		fxList->setModified(false);
		setModified(false);
	}


	return ok;
}

bool Project::loadFromFile(const QString &path)
{
	bool ok = fileLoad(path);

	// Now try to load FxItem VarSets (with classname "FxItem")
	clearCurrentVars();
	QFile file(path);
	FxItem *fx = 0;
	int line_number = 0;
	if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		QTextStream read(&file);
		while (!read.atEnd() && ok) {
			int child_level = 0;
			int ret = analyzeLine(read,fx,child_level,&line_number);
			if (ret < 0) {
				qDebug() << "AnalyzeLine failed while searching for fxItem";
				ok = false;
			}
			// Wait for ClassName matching FxItem Type
			if (curClassname == "FxItem") {
				// An opening bracket could mean a new instance will come
				if (curKey.startsWith('[') && !curKey.startsWith("[CHILDLIST]") ) fx = 0;
				// We have to wait for FxType to determine what kind of Fx we
				// have to initialize
				if (curKey == "FxType") {
					fx = fxList->addFx(curValue.toInt());
					if (debug > 2) DEBUGTEXT("Added FxItem Type:%d to Fx list",fx->fxType());
				}
			}
		}
		file.close();
		curProjectFilePath = path;
		fxList->setModified(false);
		setModified(false);
	} else {
		ok = false;
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

bool Project::postLoadProcessFxList()
{
	bool was_on_stage = false;
	for (int t=0; t<fxList->size(); t++) {
		FxItem *fx = fxList->at(t);
		if (fx->fxType() == FX_SCENE) {
			FxSceneItem *scene = static_cast<FxSceneItem*>(fx);
			/// @todo: Light restore on load funktioniert nicht
			bool scene_was_on_stage = scene->postLoadInitTubes(false);
			if (scene_was_on_stage) {
				was_on_stage = true;
				LOGTEXT(QObject::tr("Scene '%1' was on stage when currently loaded project was saved")
						.arg(scene->name()));
			}
		}
	}
	return was_on_stage;
}

void Project::init()
{
	fxList = new FxList;

	setClass(PrefVarCore::PROJECT,"Project settings");
	setDescription("This VarSet holds configuration data for a single project");

	addExistingVar(pProjectId,"ProjectId");
	addExistingVar(pProjectName,"ProjectName");
	addExistingVar(pComment,"Comment");
	addExistingVar(pAutoProceedSequence,"FxListAutoProceedSequence");

}


