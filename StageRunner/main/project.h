#ifndef PROJECT_H
#define PROJECT_H

#include "tool/prefvarset.h"

class FxList;

class Project : public VarSet
{
public:
	FxList *fxList;

protected:
	pint64 pProjectId;
	pstring pProjectName;
	pstring pComment;

public:
	Project();
	~Project();

	void clear();

	bool saveToFile(const QString & path);
	bool loadFromFile(const QString & path);

private:
	void init();
};

#endif // PROJECT_H
