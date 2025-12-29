/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
************************************************************************************/

#include "prefvar.h"
#include "varset.h"
#include "log.h"
#include "configrev.h"
// #include "../config.h"

#include <QSettings>
#include <QtGlobal>

/// Statics für die Variablenverwaltung
int PrefVarCore::instance_cnt = 0;
MutexQList<PrefVarCore *> *PrefVarCore::glob_var_list = 0;
MutexQHash<QString,PrefVarCore::VarClass> *PrefVarCore::classMapping = 0;


// ---------------------------------------------------------------------------------------------
///  class PrefVarCore
// ---------------------------------------------------------------------------------------------
PrefVarCore::PrefVarCore(PrefVarType type, const QString & name)
{
	if (!instance_cnt) {
		glob_var_list = new MutexQList<PrefVarCore *>;
	}
	instance_cnt++;

	refcnt.ref();
	init();

	mytype = type;
	myname = name;

	// Var could be a Dummy for function
	if (type == PrefVarCore::VARSET_LIST) {
		function = PrefVarCore::FUNC_VARSET;
	}


	// Variable in Liste merken
	glob_var_list->lockAppend(this);

}


PrefVarCore::~PrefVarCore()
{
	// Falls Objekt Teil einer Liste war, muss es nun definitiv aus den Listen gelöscht werden

	for (int t=0; t<parent_var_sets.size(); t++) {
		// qDebug() << "test parent list" << t << myname;
		if (parent_var_sets.at(t)->contains(this)) {
			// qDebug("Deleted '%s' from parent list",myname.toLatin1().data() );
			parent_var_sets.at(t)->removeOne(this);
		}
	}

	glob_var_list->lockRemoveOne(this);

	if (refcnt.deref()) {
#if QT_VERSION >= 0x050f00
		int r = refcnt.loadRelaxed();
#elif QT_VERSION >= 0x050000
		int r = refcnt.load();
#else
		int r = refcnt;
#endif
		qWarning("Deleted '%s' with refs (%d)",myname.toLatin1().data(),r);
	}

	// Falls letzte Instanz gelöscht wurde, wird die globale Tracking-Liste der Instanzen auch gelöscht
	if (--instance_cnt == 0) {
		if (glob_var_list->size() > 0) {
			qFatal("PrefVarCore::~PrefVarCore(): Deleted last instance of PrefVarCore but tracking list is not empty!");
		}
		delete glob_var_list;
		glob_var_list = 0;
		delete classMapping;
		classMapping = 0;
	}
}

void PrefVarCore::registerVarClasses()
{
	if (classMapping) {
		qFatal("%s: this function must called only once at programm start!",Q_FUNC_INFO);
	}

	classMapping = new MutexQHash<QString,VarClass>;

	classMapping->insert("VC_NO_CLASS",NO_CLASS);
	classMapping->insert("VC_VAR_SET_CLASS",VAR_SET_CLASS);
	classMapping->insert("VC_SYSTEM_CONFIG",SYSTEM_CONFIG);
	classMapping->insert("VC_USER_CONFIG",USER_CONFIG);
	classMapping->insert("VC_PROJECT",PROJECT);
	classMapping->insert("VC_FX_ITEM",FX_ITEM);
	classMapping->insert("VC_DMX_CHANNEL",DMX_CHANNEL);
	classMapping->insert("VC_PLUGIN_MAPPING",PLUGIN_MAPPING);
	classMapping->insert("VC_PLUGIN_CONFIG",PLUGIN_CONFIG);
	classMapping->insert("VC_MESSAGE",MESSAGE);
	classMapping->insert("VC_MESSAGE_HUB",MESSAGE_HUB);
	classMapping->insert("VC_FX_AUDIO_ITEM",FX_AUDIO_ITEM);
	classMapping->insert("VC_FX_SCENE_ITEM",FX_SCENE_ITEM);
	classMapping->insert("VC_FX_PLAYLIST_ITEM",FX_PLAYLIST_ITEM);
	classMapping->insert("VC_FX_SEQUENCE",FX_SEQUENCE_ITEM);
	classMapping->insert("VC_FX_CLIP_ITEM",FX_CLIP_ITEM);
	classMapping->insert("VC_FX_SCRIPT_ITEM",FX_SCRIPT_ITEM);
	classMapping->insert("VC_FX_CUE",FX_CUE_ITEM);
	classMapping->insert("VC_FX_TIMELINE",FX_TIMELINE_ITEM);
	classMapping->insert("VC_TIMELINE_OBJ",TIMELINE_OBJ);
	classMapping->insert("VC_TIMELINE_TRACK",FX_TIMELINE_TRACK);
}

PrefVarCore::VarClass PrefVarCore::getVarClass(const QString &className)
{
	if (!classMapping) {
		qFatal("%s: No VarClasses registered",Q_FUNC_INFO);
	}

	if (classMapping->contains(className)) {
		return classMapping->value(className);
	} else {
		return NO_CLASS;
	}
}

QString PrefVarCore::getVarClassName(PrefVarCore::VarClass varClass)
{
	if (!classMapping) {
		qFatal("%s: No VarClasses registered",Q_FUNC_INFO);
	}

	return classMapping->key(varClass);
}

void PrefVarCore::init()
{
	initialized_f = false;
	modified_f = false;
	doNotCopy = false;
	p_refvar = 0;
	contextClass = NO_CLASS;
	function = FUNC_NORMAL;
}

QVariant PrefVarCore::get_value() const
{
	if (debug > 1 || function == FUNC_NORMAL)
		qDebug() << Q_FUNC_INFO << "'get_value' not implemented by" << myname;
	return QVariant();
}

void PrefVarCore::set_value(QVariant val)
{
	if (debug > 1 || function == FUNC_NORMAL)
		qDebug() << Q_FUNC_INFO << "'set_value' not implemented by" << myname << " set:" << val;
}

bool PrefVarCore::writeToPrefGroup(QSettings * setting, const QString & group)
{
	bool ok = false;

	if (group.size()) {
		setting->beginGroup(group);
		setting->setValue(myname,get_value());
		setting->endGroup();
		ok = true;
	}

	if (contextClass == NO_CLASS) {
		setting->beginGroup("System");
		setting->setValue(myname,get_value());
		setting->endGroup();
		ok = true;
	}

	if (contextClass == USER_CONFIG) {
		setting->beginGroup("UserPrefs");
		setting->setValue(myname,get_value());
		setting->endGroup();
		ok = true;
	}

	return ok;
}

bool PrefVarCore::readFromPrefGroupSystem(QSettings * setting, const QString & group)
{
	if (group.size()) {
		setting->beginGroup(group);
	}
	else if (contextClass == NO_CLASS) {
		setting->beginGroup("System");
	}
	else if (contextClass == USER_CONFIG) {
		setting->beginGroup("UserPrefs");
	}
	else {
		return false;
	}

	if (setting->contains(myname)) {
		set_value(setting->value(myname));
	} else {
		// Falls Variable nicht existiert -> Default Wert setzen
		set_value(get_default());
	}
	setting->endGroup();

	return true;
}

/**
 * @brief Alle pXXX Variablen per QSetting aus ".config/QASS/analyzer.conf" lesen, die vom Class Type 'NO_CLASS' sind
 * @return true, bei Erfolg
 *
 * Die Funktion liest keine Variablen, die sich in einem VarSet befinden.
 */
bool PrefVarCore::readAllFromPref()
{
	bool ok = true;

	QSettings set(QSETFORMAT,APPNAME);

	for (int t=0; t<glob_var_list->size(); t++) {
		PrefVarCore *pvar = glob_var_list->at(t);
		// qDebug() << pvar->myname << pvar->myclass;
		if (!pvar->parent_var_sets.size() && pvar->contextClass == PrefVarCore::NO_CLASS) {
			pvar->readFromPrefGroupSystem(&set);
		}
	}

	return ok;
}

/**
 * @brief Alle pXXX Variablen per QSetting in ".config/QASS/analyzer.conf" schreiben, die vom Class Type 'NO_CLASS' sind
 * @return true, bei Erfolg
 *
 * Die Funktion schreibt keine Variablen, die sich in einem VarSet befinden.
 */
bool PrefVarCore::writeAllToPref()
{
	bool ok = true;

	QSettings set(QSETFORMAT,APPNAME);
	glob_var_list->lock();
	for (int t=0; t<glob_var_list->size(); t++) {
		PrefVarCore *pvar = glob_var_list->at(t);
		// Wenn die Variable Teil eines VariablenSets ist, wird sie nicht in die Prefs gespeichert
		if (!pvar->parent_var_sets.size() && pvar->contextClass == PrefVarCore::NO_CLASS) {
			pvar->writeToPrefGroup(&set);
		}
	}
	glob_var_list->unlock();
	return ok;
}

void PrefVarCore::cloneFrom(const PrefVarCore &other)
{
	myname = other.myname;
	dispname = other.dispname;
	description = other.description;
	mytype = other.mytype;
	contextClass = other.contextClass;
	modified_f = false;
}


/**
 * \brief Debug: Alle bekannten Variablen auf der Konsole ausgeben [static]
 */
void PrefVarCore::debugDumpAllVars()
{
	for (int t=0; t<glob_var_list->size(); t++) {
		const PrefVarCore *var = glob_var_list->at(t);
		if (var->initialized_f) {
			qDebug() << "var" << t+1 << ":" << var->myname << ":" << var->get_value();
		} else {
			qDebug() << "var" << t+1 << ": not initialized. Value:" << var->get_value();
		}
	}
 }

// ---------------------------------------------------------------------------------------------
///  class pint32
// ---------------------------------------------------------------------------------------------
pint32::pint32()
: PrefVarCore(INT32)
{
	pmin = pmax = pvalue = pdefault = 0;
}

pint32::~pint32()
{
}

void pint32::initPara(const QString & name, qint32 p_min, qint32 p_max, qint32 p_default, const QString & descrip)
{
	myname = name;
	dispname = name;
	pmin = p_min;
	pmax = p_max;
	pvalue = p_default;
	pdefault = p_default;
	description = descrip;
	initialized_f = true;
	modified_f = false;
}

void pint32::cloneFrom(const PrefVarCore &other)
{
	if (other.pType() == pType()) {
		PrefVarCore::cloneFrom(other);
		pmin = static_cast<const pint32 &>(other).pmin;
		pmax = static_cast<const pint32 &>(other).pmax;
		pvalue = static_cast<const pint32 &>(other).pvalue;
		pdefault = static_cast<const pint32 &>(other).pdefault;
	}
}

void pint32::set_value(QVariant val)
{
	qint32 v = val.toInt();
	if (p_refvar) {
		if (v != *static_cast<qint32*>(p_refvar)) {
			*static_cast<qint32*>(p_refvar) = v;
			modified_f = true;
		}
	} else {
		if (pvalue != v) {
			pvalue = v;
			modified_f = true;
		}
	}
}

QVariant pint32::get_value() const
{
	if (p_refvar) {
		return  *static_cast<qint32 *>(p_refvar);
	} else {
		return pvalue;
	}
}

void pint32::setValue(int val)
{
	if (p_refvar) {
		if (*static_cast<qint32*>(p_refvar) != val) {
			*static_cast<qint32*>(p_refvar) = val;
			modified_f = true;
		}
	} else {
		*this = val;
	}
}



// ---------------------------------------------------------------------------------------------
///  class pint64
// ---------------------------------------------------------------------------------------------
pint64::pint64()
: PrefVarCore(INT64)
{
	pmin = pmax = pvalue = pdefault = 0;
}

pint64::~pint64()
{
}

void pint64::initPara(const QString & name, qint64 p_min, qint64 p_max, qint64 p_default, const QString & descrip)
{
	myname = name;
	dispname = name;
	pmin = p_min;
	pmax = p_max;
	pvalue = p_default;
	pdefault = p_default;
	description = descrip;
	initialized_f = true;
	modified_f = false;
}

void pint64::cloneFrom(const PrefVarCore &other)
{
	PrefVarCore::cloneFrom(other);
	pmin = static_cast<const pint64 &>(other).pmin;
	pmax = static_cast<const pint64 &>(other).pmax;
	pvalue = static_cast<const pint64 &>(other).pvalue;
	pdefault = static_cast<const pint64 &>(other).pdefault;
}

void pint64::set_value(QVariant val)
{
	qint64 v = val.toLongLong();
	if (p_refvar) {
		if (v != *static_cast<qint64*>(p_refvar)) {
			*static_cast<qint64*>(p_refvar) = v;
			modified_f = true;
		}
	} else {
		if (pvalue != v) {
			pvalue = v;
			modified_f = true;
		}
	}
}

QVariant pint64::get_value() const
{
	if (p_refvar) {
		return *static_cast<qint64*>(p_refvar);
	} else {
		return pvalue;
	}
}

void pint64::setValue(int val)
{
	if (p_refvar) {
		if (*static_cast<qint64*>(p_refvar) != val) {
			*static_cast<qint64*>(p_refvar) = val;
			modified_f = true;
		}
	} else {
		*this = val;
	}
}

void pint64::setValue(qint64 val)
{
	if (p_refvar) {
		if (*static_cast<qint64*>(p_refvar) != val) {
			*static_cast<qint64*>(p_refvar) = val;
			modified_f = true;
		}
	} else {
		*this = val;
	}
}


// ---------------------------------------------------------------------------------------------
///  class pstring
// ---------------------------------------------------------------------------------------------
pstring::pstring()
: PrefVarCore(STRING)
{
}

pstring::~pstring()
{
}

void pstring::initPara(const QString & name, const QString & p_default, const QString & descrip)
{
	myname = name;
	dispname = name;
	pvalue = p_default;
	pdefault = p_default;
	description = descrip;
	initialized_f = true;
	modified_f = false;
	function = PrefVarCore::FUNC_NORMAL;
	exists_in_file_f = false;
}

void pstring::cloneFrom(const PrefVarCore &other)
{
	PrefVarCore::cloneFrom(other);
	pvalue = static_cast<const pstring &>(other).pvalue;
	pdefault = static_cast<const pstring &>(other).pvalue;
}

void pstring::set_value(QVariant val)
{
	QString v=val.toString();
	if (p_refvar) {
		if (v != *static_cast<QString*>(p_refvar)) {
			*static_cast<QString*>(p_refvar) = v;
			modified_f = true;
		}
	} else {
		if (pvalue != v) {
			pvalue = v;
			modified_f = true;
		}
	}
}

QVariant pstring::get_value() const
{
	if (p_refvar) {
		return *static_cast<QString*>(p_refvar);
	} else {
		return pvalue;
	}
}

void pstring::setValue(const QString &text)
{
	if (p_refvar) {
		if (*static_cast<QString*>(p_refvar) != text) {
			*static_cast<QString*>(p_refvar) = text;
			modified_f = true;
		}
	} else {
		*this = text;
	}
}

// ---------------------------------------------------------------------------------------------
///  class pbool
// ---------------------------------------------------------------------------------------------

pbool::pbool()
	: PrefVarCore(BOOL)
{
	pvalue = false;
	pdefault = false;
}

pbool::~pbool()
{
}


void pbool::initPara(const QString &name, bool p_default, const QString &descrip)
{
	myname = name;
	dispname = name;
	pvalue = p_default;
	pdefault = p_default;
	description = descrip;
	initialized_f = true;
	modified_f = false;
}

void pbool::cloneFrom(const PrefVarCore &other)
{
	PrefVarCore::cloneFrom(other);
	pvalue = static_cast<const pbool&>(other).pvalue;
	pdefault = static_cast<const pbool&>(other).pdefault;
}

void pbool::set_value(QVariant val)
{
	bool v=val.toBool();
	if (p_refvar) {
		if (v != *static_cast<bool*>(p_refvar)) {
			*static_cast<bool*>(p_refvar) = v;
			modified_f = true;
		}
	} else {
		if (pvalue != v) {
			pvalue = v;
			modified_f = true;
		}
	}
}

QVariant pbool::get_value() const
{
	if (p_refvar) {
		return *static_cast<bool*>(p_refvar);
	} else {
		return pvalue;
	}
}

void pbool::setValue(bool val)
{
	if (p_refvar) {
		if (*static_cast<bool*>(p_refvar) != val) {
			*static_cast<bool*>(p_refvar) = val;
			modified_f = true;
		}
	} else {
		*this = val;
	}
}
