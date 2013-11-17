#ifndef PROJECT_H
#define PROJECT_H

#include "tool/varset.h"

class FxList;

class Project : public VarSet
{
public:
	FxList *fxList;

	pbool pAutoProceedSequence;

	QString curProjectFilePath;					///< Stores the location of the project file, is previously saved or loaded from disk (used for project-save)
private:

protected:
	pint64 pProjectId;
	pint32 pProjectFormat;
	pstring pProjectName;
	pstring pComment;

public:
	Project();
	~Project();

	void clear();

	bool saveToFile(const QString & path);
	bool loadFromFile(const QString & path);
	bool isModified();
	void setModified(bool state);

	bool postLoadProcessFxList();

private:
	void init();
};

#endif // PROJECT_H
