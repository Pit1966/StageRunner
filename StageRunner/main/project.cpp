#include "project.h"

#include "../fx/fxlist.h"
#include "../fx/fxitem.h"

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
		for (int t=0; t<fxList->size(); t++) {
			FxItem *fx = fxList->at(t);
			qDebug("Save: %d",fx->fxID());
			fx->setDatabaseReferences((qint64)pProjectId,t+1);
			// append to previously saved file
			fx->fileSave(path, true, true);
		}
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
	if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		QTextStream read(&file);
		while (!read.atEnd() && ok) {
			QString line = read.readLine();
			ok = analyzeLine(line,fx);
			// Wait for ClassName matching FxItem Type
			if (curClassname == "FxItem") {
				if (curKey.startsWith('[')) fx = 0;
				// We have to wait for FxType to determine what kind of Fx we
				// have to initialize
				if (curKey == "FxType") {
					fx = fxList->addFx(curValue.toInt());
					if (debug > 1) DEBUGTEXT("Added FxItem Type:%d to Fx list",fx->fxType());
				}
			}
		}
		file.close();
	} else {
		ok = false;
	}

	return ok;
}

void Project::init()
{
	fxList = new FxList;

	setClass(PrefVarCore::PROJECT,"Project settings");
	setDescription("This VarSet holds configuration data for a single project");

	addExistingVar(pProjectId,"ProjectId");
	addExistingVar(pProjectName,"ProjectName");
	addExistingVar(pComment,"Comment");

}
