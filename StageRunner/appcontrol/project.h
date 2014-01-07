#ifndef PROJECT_H
#define PROJECT_H

#include "tool/varset.h"

class FxList;

class Project : public VarSet
{
public:
	pbool pAutoProceedSequence;
	QString curProjectFilePath;					///< Stores the location of the project file, is previously saved or loaded from disk (used for project-save)

	int loadErrorLineNumber;					///< this holds the line number after load project has failed due to a script error
	QString loadErrorLineString;
	bool loadErrorFileNotExisting;

protected:
	pint64 pProjectId;
	pint32 pProjectFormat;
	pstring pProjectName;
	pstring pComment;

private:
	FxList *fxList;

public:
	Project();
	~Project();

	void clear();

	bool saveToFile(const QString & path);
	bool loadFromFile(const QString & path);
	bool isModified();
	void setModified(bool state);

	void postLoadProcessFxList();
	bool postLoadResetFxScenes();

	inline FxList *mainFxList() {return fxList;}

private:
	void init();
};

#endif // PROJECT_H
