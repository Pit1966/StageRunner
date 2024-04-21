#ifndef FXTIMELINEOBJ_H
#define FXTIMELINEOBJ_H

#include "tool/varset.h"
#include "fx/fx_defines.h"

#include <varset.h>

class FxTimeLineObj : public VarSet
{
public:
	int trackId;				///< trackID for object
	int posMs;					///< position on timeline in milliseconds
	int lenMs;					///< length in milliseconds
	QString label;

protected:
	int m_linkedObjType	= LINKED_UNDEF;
	int m_fxID			= 0;

public:
	FxTimeLineObj(int posMs = 0, int durationMs = 0, const QString &label = {}, int trackID = 1);
	FxTimeLineObj(const FxTimeLineObj &o);
	void clear();
	bool operator ==(const FxTimeLineObj &o);
	bool isEqual(const FxTimeLineObj *o);

	int beginMs() const {return posMs;}
	int endMs() const {return posMs + lenMs;}
	int durationMs() const {return lenMs;}

	int fxID() const {return m_fxID;}
	LINKED_OBJ_TYPE type() const {return LINKED_OBJ_TYPE(m_linkedObjType);}

private:
	void init();

	friend class FxTimeLineEditWidget;
	friend class ExtTimeLineWidget;
};

#endif // FXTIMELINEOBJ_H
