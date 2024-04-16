#ifndef FXTIMELINEOBJ_H
#define FXTIMELINEOBJ_H

#include "tool/varset.h"

#include <varset.h>

class FxTimeLineObj : public VarSet
{
public:
	int trackId;				///< trackID for object
	int posMs;					///< position on timeline in milliseconds
	int lenMs;					///< length in milliseconds
	QString label;

public:
	FxTimeLineObj(int posMs = 0, int durationMs = 0, const QString &label = {}, int trackID = 1);
	FxTimeLineObj(const FxTimeLineObj &o);
	void clear();
	bool operator ==(const FxTimeLineObj &o);
	bool isEqual(const FxTimeLineObj *o);

private:
	void init();
};

#endif // FXTIMELINEOBJ_H
