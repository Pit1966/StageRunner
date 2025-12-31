#ifndef FIXTURELAYOUT_H
#define FIXTURELAYOUT_H

#include "config.h"

#include <QObject>

class AppCentral;
class LightControl;
class SR_FixtureList;
class SR_Fixture;

class FixtureLayout : public QObject
{
	Q_OBJECT
private:
	AppCentral & myApp;
	LightControl & unitLight;
	SR_FixtureList *m_layouts[MAX_DMX_UNIVERSE];		///< fixture lists and layouts for universes

public:
	explicit FixtureLayout(AppCentral &appcentral, LightControl &unitlight);
	~FixtureLayout();

	bool loadFixtureLayouts(const QString &filePath = {});

signals:

};

#endif // FIXTURELAYOUT_H
