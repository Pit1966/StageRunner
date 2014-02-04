#ifndef FXLISTVARSET_H
#define FXLISTVARSET_H

#include "varset.h"

class FxList;

class FxListVarSet : public VarSet
{
private:
	FxList *m_fxList;

public:
	FxListVarSet();
	~FxListVarSet();
	FxList *fxList();
	bool isModified();

private:
	void init();

};

#endif // FXLISTVARSET_H
