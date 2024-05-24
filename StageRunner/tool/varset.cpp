/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
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

#include "varset.h"
#include "varsetlist.h"
#include "config.h"
#include "configrev.h"
#include "database.h"
#include "dbquery.h"
#include "log.h"

#include "dmxchannel.h"
#include "pluginmapping.h"
#include "messagehub.h"
#include "fx/fxaudioitem.h"
#include "fx/fxsceneitem.h"
#include "fx/fxplaylistitem.h"
#include "fx/fxseqitem.h"
#include "fx/fxclipitem.h"
#include "fx/fxscriptitem.h"
#include "fx/fxcueitem.h"
#include "fx/fxtimelineitem.h"
#include "fx/fxtimelineobj.h"
#include "fx/fxtimelinetrack.h"
#include "dmxchanproperty.h"
#include "fx/fxlist.h"

#include "qt_versions.h"

#include <QSettings>
#include <QDebug>

#ifdef _MSC_VER
#define strdup _strdup
#endif

/// Statics

/// Das pvar -> database Variablen Typemapping. Unbedingt synchron halten mit PrefVarCore::PrefVarType
const char *VarSet::db_type_strings[5] = {
	"BIGINT",
	"INT",
	"varchar(512)",
	"BOOL",
	"INT"					///< Dummy
};

MutexQList<VarSet::RegVarItem*>*VarSet::var_registry = 0;
QAtomicInt VarSet::instance_cnt = 0;


/**
 * @brief VarSet::VarSet
 */
VarSet::VarSet()
{
	init();
	// This is the base class, that should be changed in derived classes
	myclass = PrefVarCore::VAR_SET_CLASS;
	modified_f = false;
}

VarSet::VarSet(const VarSet &other)
{
	init();
	modified_f = false;

	cloneFrom(other);
}

VarSet::~VarSet()
{
	// VarSet aus Registry austragen
	if (is_registered_f) {
		bool found = false;
		var_registry->lock();
		int i = var_registry->size()-1;
		while (i >= 0) {
			VarSet::RegVarItem * reg = var_registry->at(i);
			if (reg->object == this) {
				// qDebug() << "Deregister VarSet:" << myclassname << ", Type:" << reg->type << reg->coderComment << this;
				found = true;
				delete var_registry->takeAt(i);
			}
			i--;
		}
		var_registry->unlock();
		Q_ASSERT(found);
	}

	clear_var_list();

	if (! instance_cnt.deref()) {
		if (var_registry->size() != 0) {
			qDebug() << var_registry->size();
		}
		Q_ASSERT(var_registry->size() == 0);

		delete var_registry;
		var_registry = 0;
	}
}

void VarSet::init()
{
	instance_cnt.ref();
	if (!var_registry) {
		var_registry = new MutexQList<VarSet::RegVarItem*>();
	}

	is_registered_f = false;
	db_vset_projectid = 0;
	db_vset_index = 0;
	db_global_idx = 0;
	is_registered_f = false;
	is_db_table_registered_f = false;
	function_count = 0;
	curChildActive = false;
	cancel_file_analyze_on_empty_line = false;
	leave_child_level_on_empty_line = false;

}

/**
 * @brief VarSet::analyzeLine
 * @param read
 * @param varset
 * @param child_level
 * @param p_line_number
 * @return 0: ok, -1: Fehler, 1: Child level kann verlassen werden, 2: Canceled, aber kein Fehler
 */
int VarSet::analyzeLine(QTextStream &read, VarSet *varset, int child_level, int *p_line_number, QString *lineCopy)
{
	int seek = read.pos();

	QString line = read.readLine();
	if (lineCopy)
		*lineCopy = line;
	(*p_line_number)++;
	if (debug > 2) qDebug() << "line" << *p_line_number << "child level:" << child_level << (varset?varset->className():QString());

	if (line.contains("start 5"))
		qDebug() << "start5";

	if (line.size() < 2) {
		if (cancel_file_analyze_on_empty_line) {
			return 2;
		}
		else if (leave_child_level_on_empty_line && !curChildActive) {
			leave_child_level_on_empty_line = false;
			// End Recursion
			read.seek(seek);
			(*p_line_number)--;
			return 1;
		}
		return 0;
	}

	if (!line.startsWith("#")) {
		curKey = line.section('=',0,0).simplified();
		curValue = line.section('=',1);
	} else {
		return 0;
	}

	QString & key = curKey;
	QString & val = curValue;

	if (curKey.startsWith('[')) {
		// Analyzing a new sub class
		// Line format: [class[className]][id][index]
		QString b1;
		QString b2;
		QString b3;
		QString b1a;
		QStringList parts = key.split("][", QT_SKIP_EMPTY_PARTS);
		switch (parts.size()) {
		case 3:
			curIndex = parts[2].remove(QChar(']')).trimmed();
			b3 = curIndex;
			if (debug > 2) qDebug() << "VarSet:: Bracket Header 3:" << curIndex;
			// FALL THROUGH
		case 2:
			curProjectid = parts[1].remove(QChar(']')).trimmed();
			b2 = curProjectid;
			if (debug > 2) qDebug() << "VarSet:: Bracket Header 2:" << curProjectid;
			// FALL THROUGH
		case 1:
			curClassname = parts[0].remove(QChar(']')).trimmed().mid(1);
			if (curClassname.contains(QChar('['))) {
				b1 = curClassname.section(QChar('['),0,0);
				b1a = curClassname.section(QChar('['),1,1);
				curClassname = b1;
				if (debug > 1) qDebug() << "ClassName" << b1a << "level:" << child_level << "childactive:" << curChildActive << "varset" << varset;
			}

			b1 = curClassname;
			if (debug > 2) qDebug() << "VarSet:: Bracket Header 1:" << curClassname;

			if (b1 == "CHILDLISTEND") {
				if (curChildActive) {
					curChildActive = false;
					if (debug > 2) qDebug() << "Child list end:" << curChildListName << "class:" << curChildItemClass;
					curChildListName.clear();
					curChildItemClass.clear();
					curContextClassName.clear();
					return 0;
				} else {
					if (debug > 2) qDebug() << "Child list parent end";
					// End Recursion
					read.seek(seek);
					(*p_line_number)--;
					return 1;
				}
			}

			if (varset) {
				/// @todo Here we have to implement new code for each new VarSet-Class to load it in a list
				if (curChildActive) {
					curChildItemClass = b1;
					curContextClassName = b1a;
					if (debug > 2) qDebug() << "Search for child list with class name:" << b1 << "CPP class" << curContextClassName <<  "list num:" << b3;

					// Liste finden (This means: finding the address of the VarSetList)
					for (int t=0; t<varset->varList.size(); t++) {
						PrefVarCore *var = varset->varList.at(t);
						if (var->myname == curChildListName) {
							/// implement new VarSetList Types here
							if (var->contextClass == PrefVarCore::DMX_CHANNEL) {
								VarSetList<DmxChannel*> *varsetlist = reinterpret_cast<VarSetList<DmxChannel*>*>(var->p_refvar);
								DmxChannel *item = new DmxChannel;
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::TIMELINE_OBJ) {
								VarSetList<FxTimeLineObj*> *varsetlist = reinterpret_cast<VarSetList<FxTimeLineObj*>*>(var->p_refvar);
								FxTimeLineObj* item = new FxTimeLineObj;
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::FX_TIMELINE_TRACK) {
								VarSetList<FxTimeLineTrack*> *varsetlist = reinterpret_cast<VarSetList<FxTimeLineTrack*>*>(var->p_refvar);
								FxTimeLineTrack* item = new FxTimeLineTrack(nullptr);
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::PLUGIN_CONFIG) {
								VarSetList<PluginConfig*> *varsetlist = reinterpret_cast<VarSetList<PluginConfig*>*>(var->p_refvar);
								PluginConfig *item = new PluginConfig;
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::MESSAGE) {
								VarSetList<VMsg*> *varsetlist = reinterpret_cast<VarSetList<VMsg*>*>(var->p_refvar);
								VMsg *item = new VMsg;
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::FX_AUDIO_ITEM) {
								VarSetList<FxAudioItem*> *varsetlist = reinterpret_cast<VarSetList<FxAudioItem*>*>(var->p_refvar);
								FxList *fxlist = reinterpret_cast<FxList*>(varsetlist->parentVoid);
								FxAudioItem *item = new FxAudioItem(fxlist);
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::FX_ITEM) {
								VarSetList<FxItem*> *varsetlist = reinterpret_cast<VarSetList<FxItem*>*>(var->p_refvar);
								FxList *fxlist = reinterpret_cast<FxList*>(varsetlist->parentVoid);
								FxItem *item = nullptr;
								PrefVarCore::VarClass contextclass = PrefVarCore::getVarClass(curContextClassName);
								switch (contextclass) {
								case PrefVarCore::FX_AUDIO_ITEM:
									item = new FxAudioItem(fxlist);
									break;
								case PrefVarCore::FX_SCENE_ITEM:
									item = new FxSceneItem(fxlist);
									break;
								case PrefVarCore::FX_PLAYLIST_ITEM:
									item = new FxPlayListItem(fxlist);
									break;
								case PrefVarCore::FX_SEQUENCE_ITEM:
									item = new FxSeqItem(fxlist);
									break;
								case PrefVarCore::FX_CLIP_ITEM:
									item = new FxClipItem(fxlist);
									break;
								case PrefVarCore::FX_SCRIPT_ITEM:
									item = new FxScriptItem(fxlist);
									break;
								case PrefVarCore::FX_CUE_ITEM:
									item = new FxCueItem(fxlist);
									break;
								case PrefVarCore::FX_TIMELINE_ITEM:
									item = new FxTimeLineItem(fxlist);
									break;
								default:
									break;
								}
								if (item) {
									varsetlist->append(item);
									int err = item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy);
									FxItem *oitem = FxItem::checkItemUniqueId(item);
									if (oitem) {
										LOGERROR(QObject::tr("Loaded Fx '%1' has already used ID (%2) -> Generate new ID for already existing item: '%3'")
												 .arg(item->name())
												 .arg(item->id())
												 .arg(oitem->name()));
										oitem->generateNewID();
									}
									// qDebug() << "Loaded item with ID" << item->id() << "init with" << item->initID();
									if (err == -1)
										return err;
									return 0;
								}
							}
							else if (var->contextClass == PrefVarCore::DMX_CHANNEL_PROPERTY) {
								VarSetList<DmxChanProperty*> *varsetlist = reinterpret_cast<VarSetList<DmxChanProperty*>*>(var->p_refvar);
								DmxChanProperty *item = new DmxChanProperty();
								varsetlist->append(item);
								if (-1 == item->analyzeLoop(read,item,child_level+1,p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
						}
					}
				}
				else if (b1 != "CHILDLIST") {
					// Test if bracket1 matches a VarSet member name.
					for (int t=0; t<varset->varList.size(); t++) {
						PrefVarCore *var = varset->varList.at(t);
						if (var->mytype == PrefVarCore::VARSET) {
							VarSet *targetset = reinterpret_cast<VarSet*>(var->p_refvar);
							if (targetset->myclassname == b1) {
								if (debug > 2) qDebug() << "Found bracket1 as myclassname:" << b1;
								targetset->leave_child_level_on_empty_line = true;
								if (-1 ==  targetset->analyzeLoop(read,targetset,child_level+1, p_line_number,lineCopy)) {
									return -1;
								} else {
									return 0;
								}
							}
						}
					}
				}
			}

			if (b1 == "CHILDLIST") {
				curChildActive = true;
				curChildListName = b2;
				if (debug > 2) qDebug() << "Child list found:" << b2;
				return 0;
			}

			if (child_level > 0 && !curChildActive) {
				// End Recursion
				read.seek(seek);
				(*p_line_number)--;
				return 1;
			}

			return 0;

		default:
			break;
		}
	}

	if (!varset) return 0;

	if (curClassname.size() && curClassname != varset->myclassname) {
		if (debug > 3) qDebug() << "VarSet:: ClassName does not match: "
								<< curClassname << "read key:" << key << "val:" << val;
		return 0;
	} else {
		if (debug > 3) qDebug() << "VarSet:: read key:" << key << "val:" << val;
	}

	if (curClassname.isEmpty() || curClassname == varset->myclassname) {
		int i = 0;
		while (i<varset->varList.size()) {
			PrefVarCore *var = varset->varList.at(i);
			if (key == var->pVarName()) {
				var->set_value(val);
				var->exists_in_file_f = true;
				return 0;
			}
			i++;
		}
	}
	qDebug() << "VarSet:: FAILED: read key:" << key << "not found!!";
	// Key nicht in VarSet gefunden
	return -1;
}


void VarSet::clearCurrentVars()
{
	curClassname.clear();
	curIndex.clear();
	curProjectid.clear();
	curKey.clear();
	curValue.clear();
	curChildActive = false;
	curChildItemClass.clear();
	curChildListName.clear();
}

VarSet &VarSet::operator =(const VarSet &other)
{
	cloneFrom(other);
	return *this;
}

void VarSet::cloneFrom(const VarSet &other)
{
	// qDebug() << "cloneFrom" << varList.size() << other.varList.size();
	for (int t=0; t<other.varList.size(); t++) {
		const PrefVarCore *o = other.varList.at(t);
		if (o->doNotCopy)
			continue;

		int idx = find(o);
		if (idx >= 0) {
			// Variable wurde in eigener Liste gefunden -> Wert ändern
			varList.at(idx)->set_value(o->get_value());
			modified_f |= varList.at(idx)->isModified();
		}
		else {
			switch (o->mytype) {
			case PrefVarCore::BOOL:
				{
					pbool *var = addDynamicPbool(o->myname);
					if (var) {
						var->cloneFrom(*o);
					}
				}
				break;
			case PrefVarCore::INT32:
				{
					pint32 *var = addDynamicPint32(o->myname);
					if (var) {
						var->cloneFrom(*o);
					}
				}
				break;
			case PrefVarCore::INT64:
				{
					pint64 *var = addDynamicPint64(o->myname);
					if (var) {
						var->cloneFrom(*o);
					}
				}
				break;
			case PrefVarCore::STRING:
				{
					pstring *var = addDynamicPstring(o->myname);
					if (var) {
						var->cloneFrom(*o);
					}
				}
				break;
			case PrefVarCore::VARSET:
			case PrefVarCore::VARSET_LIST:
				DEBUGERROR("%s: Implement me: Line %d PrefVar '%s' not cloned"
						   ,Q_FUNC_INFO,__LINE__,o->myname.toLocal8Bit().data());
				break;

			}
			modified_f = true;
		}
	}
	myclass = other.myclass;
}

bool VarSet::checkModified()
{
	for (int t=0; t<varList.size(); t++) {
		if (varList.at(t)->isModified()) {
			modified_f = true;
			return true;
		}
	}
	modified_f = false;
	return false;
}

void VarSet::setModified(bool state)
{
	modified_f = state;
	for (int t=0; t<varList.size(); t++) {
		varList.at(t)->setModified(state);
	}
}

bool VarSet::addExistingVar(pint32 &var, const QString &name, qint32 p_min, qint32 p_max, qint32 p_default, const QString &descrip)
{
	PrefVarCore * prefvar = (PrefVarCore *)&var;
	if (varList.contains(prefvar)) return false;

	prefvar->refcnt.ref();
	varList.lockAppend(prefvar);

	var.initPara(name, p_min, p_max, p_default, descrip);
	var.parent_var_sets.append(this);
	var.contextClass = myclass;

	return true;
}

bool VarSet::addExistingVar(pint64 &var, const QString &name, qint64 p_min, qint64 p_max, qint64 p_default, const QString &descrip)
{
	PrefVarCore * prefvar = (PrefVarCore *)&var;
	if (varList.contains(prefvar)) return false;
	// qDebug() << "add" << var.myname << "ref" << prefvar->refcnt;

	prefvar->refcnt.ref();
	varList.lockAppend(prefvar);

	var.initPara(name, p_min, p_max, p_default, descrip);
	var.parent_var_sets.append(this);
	var.contextClass = myclass;

	return true;
}

bool VarSet::addExistingVar(pstring &var, const QString & name, const QString & p_default, const QString & descrip)
{
	PrefVarCore * prefvar = (PrefVarCore *)&var;
	if (varList.contains(prefvar)) return false;

	prefvar->refcnt.ref();
	varList.lockAppend(prefvar);

	var.initPara(name, p_default, descrip);
	var.parent_var_sets.append(this);
	var.contextClass = myclass;

	return true;
}

bool VarSet::addExistingVar(pbool &var, const QString &name, bool p_default, const QString &descrip)
{
	PrefVarCore * prefvar = (PrefVarCore *)&var;
	if (varList.contains(prefvar)) return false;

	prefvar->refcnt.ref();
	varList.lockAppend(prefvar);

	var.initPara(name, p_default, descrip);
	var.parent_var_sets.append(this);
	var.contextClass = myclass;

	return true;
}


bool VarSet::addExistingVar(qint32 &var, const QString &name, qint32 p_min, qint32 p_max, qint32 p_default, const QString &descrip)
{
	if (exists(name)) return false;
	pint32 *newvar = new pint32;
	newvar->initPara(name,p_min,p_max,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	newvar->p_refvar = (void*)&var;
	varList.lockAppend(newvar);

	// Default Wert in existierender Variable setzen
	var = p_default;
	return true;
}

bool VarSet::addExistingVar(qint64 &var, const QString &name, qint64 p_min, qint64 p_max, qint64 p_default, const QString &descrip)
{
	if (exists(name)) return false;
	pint64 *newvar = new pint64;
	newvar->initPara(name,p_min,p_max,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	newvar->p_refvar = (void*)&var;
	varList.lockAppend(newvar);

	// Default Wert in existierender Variable setzen
	var = p_default;
	return true;
}

bool VarSet::addExistingVar(bool &var, const QString &name, bool p_default, const QString &descrip)
{
	if (exists(name)) return false;
	pbool *newvar = new pbool;
	newvar->initPara(name,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	newvar->p_refvar = (void*)&var;
	varList.lockAppend(newvar);

	// Default Wert in existierender Variable setzen
	var = p_default;
	return true;
}

bool VarSet::addExistingVar(QString & var, const QString &name, const QString &p_default, const QString &descrip)
{
	if (exists(name)) return false;
	pstring *newvar = new pstring;
	newvar->initPara(name,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	newvar->p_refvar = (void *) &var;
	varList.lockAppend(newvar);
	// Default Wert in existierender Variable setzen
	var = p_default;
	return true;
}

bool VarSet::addExistingVar(VarSet &var, const QString &name, const QString &descrip)
{
	if (exists(name)) return false;
	PrefVarCore *newvar = new PrefVarCore(PrefVarCore::VARSET,name);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = var.myclass;
	newvar->p_refvar = (void *) &var;
	newvar->function = PrefVarCore::FUNC_VARSET;
	newvar->description = descrip;
	varList.lockAppend(newvar);
	return true;
}


pint32 * VarSet::addDynamicPint32(const QString &name, qint32 p_min, qint32 p_max, qint32 p_default, const QString &descrip)
{
	if (exists(name)) return 0;
	pint32 *newvar = new pint32;
	newvar->initPara(name,p_min,p_max,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	varList.lockAppend(newvar);

	return newvar;
}

pint64 * VarSet::addDynamicPint64(const QString &name, qint64 p_min, qint64 p_max, qint64 p_default, const QString &descrip)
{
	if (exists(name)) return 0;
	pint64 *newvar = new pint64;
	newvar->initPara(name,p_min,p_max,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	varList.lockAppend(newvar);

	return newvar;
}

pstring * VarSet::addDynamicPstring(const QString &name, const QString &p_default, const QString &descrip)
{
	if (exists(name)) return 0;
	pstring *newvar = new pstring;
	newvar->initPara(name,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	varList.lockAppend(newvar);

	return newvar;
}

pbool * VarSet::addDynamicPbool(const QString &name, bool p_default, const QString &descrip)
{
	if (exists(name)) return 0;
	pbool *newvar = new pbool;
	newvar->initPara(name,p_default,descrip);
	newvar->parent_var_sets.append(this);
	newvar->contextClass = myclass;
	varList.lockAppend(newvar);

	return newvar;
}

pstring *VarSet::addDynamicFunction(const QString &func_name, const QString &func_para)
{
	QString varname = QString("function|%1").arg(++function_count);

	pstring *newvar = new pstring;
	newvar->initPara(varname, func_name, func_para);
	newvar->function = PrefVarCore::FUNC_FUNCTION;
	newvar->contextClass = myclass;
	varList.lockAppend(newvar);

	return newvar;
}

bool VarSet::removeOne(PrefVarCore *varcore)
{
	if (varList.contains(varcore)) {
		varcore->refcnt.deref();
		return varList.lockRemoveOne(varcore);
	}
	return false;
}

/**
 * @brief Prüfen, ob Variable mit diesem Namen schon im Varset existiert
 * @param name Variablenname
 * @return true, falls existent
 */
bool VarSet::exists(const QString &name)
{
	bool found = false;
	for (int t=0; t<varList.size(); t++) {
		if (varList[t]->pVarName() == name) {
			found = true;
			break;
		}
	}
	// if (!found) qDebug("VarSet::exists: pVar '%s' not found!",name.toLatin1().data());
	return found;
}

/**
 * @brief bestimmte Variable im Set finden
 * @param name Der Name der Variable
 * @param Pointer auf Function Type Variable, in der der Funktionstyp zurückgegeben wird
 * @return Pointer auf Variable
 */
PrefVarCore *VarSet::getVar(const QString &name, PrefVarCore::Function *function_type_p)
{
	PrefVarCore *var = 0;
	for (int t=0; t<varList.size(); t++) {
		if (varList[t]->pVarName() == name) {
			var = varList[t];
			if (function_type_p) {
				if (var->function) {
					*function_type_p = var->function;
				}
			}
			break;
		}
	}
	return var;
}

/**
 * @brief bestimmte Variable im Set finden
 * @param name Der Name der Variable
 * @param type Der Typ der Variable
 * @return Pointer auf Variable
 *
 * Die suche vergleicht sowohl den Namen, als auch den Typ und beides muss der Anforderung entsprechen.
 * Achtung! Nicht benutzen, wenn man nicht sicher ist, dass die Variable existiert, da eine
 * Exception geschmissen wird, falls nicht gefunden.
 * Stattdessen könnte man @see getVar() benutzen
 */
PrefVarCore *VarSet::find(const QString &name, PrefVarCore::PrefVarType type)
{
	PrefVarCore * var = 0;
	for (int t=0; t<varList.size(); t++) {
		if (varList[t]->pVarName() == name && varList[t]->mytype == type) {
			var = varList[t];
			break;
		}
	}
	if (!var) qFatal("pVar '%s' not found!",name.toLatin1().data());
	return var;
}

int VarSet::find(const PrefVarCore *other)
{
	bool found = false;
	int idx = 0;
	while (!found && idx<varList.size()) {
		PrefVarCore * here = varList.at(idx);
		if (other->contextClass == here->contextClass && other->mytype == here->mytype && other->myname == here->myname) {
			found = true;
		} else {
			idx++;
		}
	}
	if (found) {
		return idx;
	} else {
		return -1;
	}
}

bool VarSet::getPbool(const QString &name)
{
	PrefVarCore * var = find(name,PrefVarCore::BOOL);
	if (var) {
		return var->get_value().toBool();
	} else {
		if (!var) qFatal("VarSet::getPbool: pVar '%s' not found!",name.toLatin1().data());
		return false;
	}
}

qint32 VarSet::getPint32(const QString &name)
{
	PrefVarCore * var = find(name,PrefVarCore::INT32);
	if (var) {
		return var->get_value().toInt();
	} else {
		if (!var) qFatal("VarSet::getPint32: pVar '%s' not found!",name.toLatin1().data());
		return 0;
	}
}

qint64 VarSet::getPint64(const QString &name)
{
	PrefVarCore * var = find(name,PrefVarCore::INT64);
	if (var) {
		return var->get_value().toLongLong();
	} else {
		if (!var) qFatal("VarSet::getPint64: pVar '%s' not found!",name.toLatin1().data());
		return 0;
	}
}

QString VarSet::getPstring(const QString &name)
{
	pstring * var = (pstring *)find(name,PrefVarCore::STRING);
	if (var) {
		if (var->p_refvar) {
			return *(QString *)var->p_refvar;
		} else {
			return var->pvalue;
		}
	} else {
		if (!var) qFatal("VarSet::getPstring: pVar '%s' not found!",name.toLatin1().data());
		return "";
	}
}

/// @fixme: blödsinn, Objekt wird vor Rückgabe gelöscht
char *VarSet::getPstringAscii(const QString &name)
{
	return getPstring(name).toLatin1().data();
}


QVariant VarSet::pValue(const QString &name)
{
	PrefVarCore * var = 0;
	for (int t=0; t<varList.size(); t++) {
		if (varList[t]->pVarName() == name) {
			var = varList[t];
			break;
		}
	}
	if (var) {
		return var->get_value();
	} else {
		return QVariant();
	}
}




void VarSet::setVar(const QString &name, bool val)
{
	PrefVarCore * var = find(name,PrefVarCore::BOOL);
	if (var) {
		var->set_value(val);
	}

}

void VarSet::setVar(const QString &name, qint32 val)
{
	PrefVarCore * var = find(name,PrefVarCore::INT32);
	if (var) {
		var->set_value(val);
	}

}

void VarSet::setVar(const QString &name, qint64 val)
{
	PrefVarCore * var = find(name,PrefVarCore::INT64);
	if (var) {
		var->set_value(val);
	}
}

void VarSet::setVar(const QString &name, const QString &str)
{
	PrefVarCore * var = find(name,PrefVarCore::STRING);
	if (var) {
		var->set_value(str);
	}
}

void VarSet::setClass(PrefVarCore::VarClass classtype, const QString &classname)
{
	myclass = classtype;
	myclassname = classname;
}

void VarSet::debugListVars()
{
	for (int t=0; t<varList.size(); t++) {
		PrefVarCore * varcore = varList.at(t);
		qDebug() << varcore->myname << varcore->mytype << varcore->contextClass;
	}
}

/**
 * @brief Alle Variablen des Sets in QSettings in das .config Verzeichnis in entsprechende Datei schreiben
 * @return true, bei Erfolg
 *
 * Der Klassenname @see myclassname wird der Name der Gruppe in der QSettingsdatei
 * Es macht also keinen Sinn mehrere Instanzen mit dem selben Klassennamen zu schreiben.
 */
bool VarSet::writeToPref()
{

	bool ok = true;
	bool group_open_f = false;

	QSettings set(QSETFORMAT,APPNAME);
	varList.lock();

	if (myclassname.size()) {
		set.beginGroup(myclassname);
		group_open_f = true;
	}
	for (int t=0; t<varList.size(); t++) {
		PrefVarCore *var = varList.at(t);

		if (var->function == PrefVarCore::FUNC_FUNCTION) {
			// spezielle virtuelle Funktionsvariablen untersuchen
			QString func_name = var->get_value().toString();
			QString func_para = var->pDescription();

			// Gruppe
			if (func_name == "group") {
				if (group_open_f) set.endGroup();
				set.beginGroup(func_para);
				group_open_f = true;
			}
		} else {
			// wegschreiben
			set.setValue(var->myname,var->get_value());
		}
	}
	if (group_open_f) {
		set.endGroup();
	}

	varList.unlock();
	return ok;
}

/**
 * @brief Alle Variablen des Sets mit Inhalt aus QSettings .config Verzeichnis füllen
 * @return falls, falls Gruppe (Klassenname) nicht existierte
 *
 * Der Klassenname @see myclassname wird als Name der Gruppe in der QSettingsdatei angenommen
 * Falls der Klassenname in der QSettingsdatei nicht gefunden wird, werden alle Variablen des Sets
 * mit ihren Default-Werten gefüllt.
 */
bool VarSet::readFromPref()
{
	bool ok = true;
	bool group_open_f = false;

	QSettings set(QSETFORMAT,APPNAME);
	varList.lock();

	if (myclassname.size()) {
		if (!set.childGroups().contains(myclassname)) ok = false;
		set.beginGroup(myclassname);
		group_open_f = true;
	}
	for (int t=0; t<varList.size(); t++) {
		PrefVarCore *var = varList.at(t);
		if (var->function == PrefVarCore::FUNC_FUNCTION) {
			// spezielle virtuelle Funktionsvariablen untersuchen
			QString func_name = var->get_value().toString();
			QString func_para = var->pDescription();

			// Gruppe
			if (func_name == "group") {
				if (group_open_f) set.endGroup();
				set.beginGroup(func_para);
				group_open_f = true;
			}
		} else {
			// Setzt Variable auf Wert aus QSettings-Datei. Falls Key nicht vorhanden auf Default-Wert
			var->set_value( set.value(var->myname, var->get_default()) );
		}
	}
	if (group_open_f) {
		set.endGroup();
	}

	varList.unlock();
	return ok;
}

/**
 * @brief VarSet in Datei speichern
 * @param path Der Pfad zur Datei
 * @param append falls true, werden die Daten an eine existierende Datei angehangen
 * @return true, falls kein Fehler auftrat
 */
bool VarSet::fileSave(const QString &path, bool append, bool empty_line)
{
	bool ok = true;

	QFlags<QIODevice::OpenModeFlag> append_flag;
	if (append) {
		append_flag = QIODevice::Append;
	} else {
		append_flag = QIODevice::NotOpen;
		QFile::remove(path + "~");
		QFile::rename(path, path + "~");
	}

	QFile file(path);
	if ( file.open(QIODevice::ReadWrite | QIODevice::Text | append_flag) ) {
		QTextStream write(&file);
		write.setCodec("UTF-8");

		int child_level = 0;
		ok = file_save_append(write,child_level,empty_line);


		file.close();
		if (file.error()) {
			ok = false;
		}

	} else {
		ok = false;
	}
	return ok;
}

bool VarSet::file_save_append(QTextStream &write, int child_level, bool append_empty_line)
{
	bool ok = true;

	// First Write the Header tag which is the classname
	// In addition the projectID and index will be written if projectId > 0
	if (myclassname.size()) {
		// Insert an indent depending on the recursion deep
		write << child_indent_str(child_level);
		write << "[" << myclassname << "[" << PrefVarCore::getVarClassName(myclass) << "]]";
		if (db_vset_projectid > 0 || db_vset_index > 0) {
			if (db_vset_projectid > 0) {
				write << "[" << db_vset_projectid << "]";
			} else {
				write << "[]";
			}
			if (db_vset_index > 0) {
				write << "[" << db_vset_index << "]";
			} else {
				write << "[]";
			}
		}
		write << "\n";
	}
	for (int t=0; t<varList.size(); t++) {
		PrefVarCore *var = varList.at(t);
		if (var->function != PrefVarCore::FUNC_NORMAL) {
			// spezielle virtuelle Funktionsvariablen untersuchen
			QString func_name = var->get_value().toString();
			QString func_para = var->pDescription();
			if (var->mytype == PrefVarCore::VARSET) {
				VarSet *set = reinterpret_cast<VarSet*>(var->p_refvar);
				ok = set->file_save_append(write,child_level+1,append_empty_line);
			}
			else if (var->mytype == PrefVarCore::VARSET_LIST) {
				VarSetList<VarSet*>*childlist = reinterpret_cast<VarSetList<VarSet*>*>(var->p_refvar);
				// Insert an indent depending on the recursion deep
				write << child_indent_str(child_level);
				write << "[CHILDLIST]";
				write << "[" << var->myname << "]" << "\n";
				// Recursivly call every member of the VarSetList as a VarSet
				for (int i=0; i<childlist->size(); i++) {
					VarSet *set = childlist->at(i);
					// Put list index into variable
					set->setDatabaseReferences(db_vset_projectid,i+1);
					ok = set->file_save_append(write,child_level+1,append_empty_line);
				}
				// Insert an indent depending on the recursion deep
				write << child_indent_str(child_level);
				write << "[CHILDLISTEND]" << "\n";
			}
			else if (func_name == "group") { 				// Gruppe
				// Insert an indent depending on the recursion deep
				write << child_indent_str(child_level);
				write << "[" << func_para << "]" << "\n";
			}
		} else {
			if (var->pValue() != var->pDefaultValue()) {
				// Insert an indent depending on the recursion deep
				write << child_indent_str(child_level+1);
				// Write a simple date from the varset
				write << var->pVarName() << "=" << var->pValue().toString() << "\n";
			}
		}
	}

	if (append_empty_line) {
		write << "\n";
	}

	return ok;
}

QString VarSet::child_indent_str(int childLevel)
{
	QString indent;
	for (int i=0; i<childLevel; i++) {
		 indent += "  ";
	}
	return indent;
}


/**
 * @brief VarSet aus Datei laden
 * @param path Pfad zur Datei
 * @param exists Pointer auf Bool Variable, in der abgefragt werden kann, ob Datei überhaupt existiert hat (oder 0, falls nicht gewünscht)
 * @return true, falls ok;
 */
bool VarSet::fileLoad(const QString &path, bool *exists, int *lineNumber, QString *lineCopy)
{
	clearCurrentVars();

	if (exists) {
		*exists = QFile::exists(path);
	}

	bool ok = true;
	int line_number = 0;

	QFile file(path);
	if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		QTextStream read(&file);
		int child_level = 0;
		int ret = analyzeLoop(read,this,child_level,&line_number,lineCopy);
		if (ret < 0) ok = false;
		file.close();
	} else {
		ok = false;
	}
	if (lineNumber) {
		*lineNumber = line_number;
	}

	return ok;
}


int VarSet::analyzeLoop(QTextStream &read, VarSet *varset, int child_level, int *p_line_number, QString *lineCopy)
{
	while (!read.atEnd()) {
		int ret = analyzeLine(read, varset, child_level,p_line_number,lineCopy);
		if (ret < 0) {
			qDebug() << Q_FUNC_INFO << "Exit with failure!";
			return -1;
		}
		else if (ret == 2) {
			if (debug > 2) qDebug() << Q_FUNC_INFO << "Canceled File analyzing due to empty line (This is no error)";
			return 0;
		}
		else if (ret > 0) {
			if (debug > 2) qDebug() << Q_FUNC_INFO << "Leave Child level" << child_level;
			return 1;
		}
	}
	if (debug > 2) qDebug() << Q_FUNC_INFO << "File end at Child level " << child_level;
	return 0;
}


bool VarSet::registerDatabaseGlobal(const QString &desc, int index)
{
	if (isRegistered(VarSet::REG_DB_GLOBAL) || myclassname.isEmpty()) {
		DEBUGERROR ("VarSet: '%s'' is already registered or has insufficient settings",myclassname.toLatin1().data());
		return false;
	}

	db_vset_projectid = 1;
	db_global_idx = index;

	VarSet::RegVarItem *reg = new VarSet::RegVarItem(VarSet::REG_DB_GLOBAL, myclass, myclassname, this);
	reg->coderComment = desc;
	add_to_registry(reg);
	return true;
}

bool VarSet::registerDatabaseTable(const QString &desc)
{
	if (isRegistered(VarSet::REG_DB_TABLE) || myclassname.isEmpty()) {
		DEBUGERROR ("VarSet: '%s'' is already registered or has insufficient settings",myclassname.toLatin1().data());
		return false;
	}

	db_vset_projectid = 0;
	db_vset_index = 0;

	VarSet::RegVarItem *reg = new VarSet::RegVarItem(VarSet::REG_DB_TABLE, myclass, myclassname, this);
	reg->coderComment = desc;
	reg->dbTableName = "vset_" + myclassname;

	add_to_registry(reg);
	is_db_table_registered_f = true;

	return true;
}

bool VarSet::registerQSetGlobal(const QString &desc)
{
	if (isRegistered(VarSet::REG_QSET_GLOBAL) || myclassname.isEmpty()) {
		return false;
	}

	VarSet::RegVarItem *reg = new VarSet::RegVarItem(VarSet::REG_QSET_GLOBAL, myclass, myclassname, this);
	reg->coderComment = desc;
	add_to_registry(reg);

	return true;
}

void VarSet::add_to_registry(VarSet::RegVarItem* reg)
{
	var_registry->lockAppend(reg);
	is_registered_f = true;
	qDebug() << "register" << "type:" << reg->type << myclassname << reg->coderComment << this  << var_registry->size();
}

/**
 * @brief VarSet für Speicherung in eignene Tabelle der Database registrieren
 * @param projectid ProjectID, unter der der Eintrag in die DB gespeichert wird (WHERE Spalte)
 * @param index Identifier, der zur Kennzeichnung der Instanz in der DB verwendet wird (WHERE Spalte)
 * @return
 *
 * Achtung! Die Funktion muss für jede Instanz des VarSets aufgerufen werden (wichtig bei abgeleiteten Klassen, MeasureConfig)
 *
 * In der Database wird eine Tabelle erzeugt, den den Klassenname als Tabellenname verwendet.
 * Die einzelnen Instanzen des VarSets müssen später unterschiedliche 'projectid' oder 'index'
 * Werte haben, um sie eindeutig zuzuordnen.
 * Der Index könnte hierbei z.B. die Prozessnummer sein.
 */
bool VarSet::setDatabaseReferences(quint64 projectid, int index)
{
	db_vset_projectid = projectid;
	db_vset_index = index;
	return true;
}

void VarSet::clear_var_list()
{
	while (varList.size()) {
		PrefVarCore * varcore = varList.lockTakeFirst();
		// qDebug() << "var" << varcore->myname << varcore->refcnt;
#if QT_VERSION >= 0x050e00
		if (varcore->refcnt.loadRelaxed() == 1) {
			// qDebug() << "delete" << varcore->myname;
			delete varcore;
		}
#elif QT_VERSION >= 0x050000
		if (varcore->refcnt.load() == 1) {
			// qDebug() << "delete" << varcore->myname;
			delete varcore;
		}
#else
		if (varcore->refcnt == 1) {
			// qDebug() << "delete" << varcore->myname;
			delete varcore;
		}
#endif
	}
}

/**
 * @brief VarSet in "globalsetting" Tabelle der Datenbank schreiben
 * @param db Pointer auf gültiges Datenbankobjekt.
 * @return true, bei Erfolg
 *
 * bevor diese Funktion erfolgreich ausgeführt werden kann, muss man das VarSet mit
 * registerDatabaseGlobal(int index) anmelden. Dabei kann der index Wert verwendet werden, um
 * verschiedene Objekte dieses VarSets zu deklarieren.
 *
 * Voraussetzung für all das ist:
 * myclass muss initialisiert sein und darf nicht NO_CLASS oder VAR_SET_CLASS sein.
 * Am besten man definiert in system/prefvarcore.h im VarClass enum einen neuen Typ
 */
bool VarSet::dbSaveGlobal(Database *db)
{
	if (!db->isValid()) return true;

	if (myclass == PrefVarCore::NO_CLASS || myclass == PrefVarCore::VAR_SET_CLASS || myclassname.isEmpty()) {
		// qFatal("%s bad configured",__func__);
		return false;
	}

	DBquery q(db,"vset_global");
	bool ok = true;

	int i = 0;
	while (i<varList.size() && ok) {
		PrefVarCore &var = *varList.at(i);
		QString value = var.get_value().toString();

		q.prepareReplace();
		q.prepareWhere("class_type =",myclass);
		q.prepareWhere("idx =",db_global_idx);
		q.prepareWhere("key_name =",var.myname);
		q.prepareValue("class_name",myclassname);
		q.prepareValue("key_value",value);
		ok = q.execPrepared();

		i++;
	}

	return ok;
}

bool VarSet::dbLoadGlobal(Database *db)
{
	if (!db->isValid()) return true;

	if (myclass == PrefVarCore::NO_CLASS || myclass == PrefVarCore::VAR_SET_CLASS || myclassname.isEmpty()) {
		// qFatal("%s bad configured",__func__);
		return false;
	}

	if (debug) {
		LOGTEXT(QObject::tr("Load global VarSet '%1' from database").arg(myclassname));
	}

	DBquery q(db,DBquery::SELECT,"vset_global");
	bool ok = true;

	q.prepareWhere("class_type =",myclass);
	q.prepareWhere("idx =",db_global_idx);

	ok = q.execPrepared();

	while (ok && q.nextRecord()) {
		// Ein Datensatz entspricht einer Member-Variable des VarSets
		QString name;
		QVariant value;
		ok = q.getValue("key_name",name);
		ok &= q.getValue("key_value",value);

		PrefVarCore *var = getVar(name);
		if (var) {
			var->set_value(value);
		} else {
			// qDebug("%s Varname:'%s' not found.",__func__,name.toLatin1().data());
		}
	}

	return ok;
}

bool VarSet::dbSave(Database *db)
{
	if (!db->isValid()) return true;

	VarSet::RegVarItem item = getRegisterItemCopy(VarSet::REG_DB_TABLE,this);
	if (!item.isValid) {
		// DEVELTEXT("%s bad configured, VarSet not registered for database",__func__);
		return false;
	}

	if (db_vset_projectid <= 0) {
		// DEVELTEXT("%s bad configured, db_ProjectId for VarSet not defined",__func__);
		return false;
	}

	DBquery q(db, DBquery::REPLACE, item.dbTableName);
	q.prepareWhere("vset_projectid =",db_vset_projectid);
	q.prepareWhere("vset_index =",db_vset_index);

	int i=0;
	while (i<varList.size()) {
		PrefVarCore *var = varList.at(i);
		q.prepareValue(var->pVarName(), var->pValue());
		i++;
	}
	return q.execPrepared();
}


bool VarSet::dbLoad(Database *db, bool *exists)
{
	if (exists) *exists = false;

	if (!db->isValid()) return true;

	VarSet::RegVarItem item = getRegisterItemCopy(VarSet::REG_DB_TABLE,this);
	if (!item.isValid) {
		// DEVELTEXT("%s bad configured, VarSet not registered for database",__func__);
		return false;
	}

	if (db_vset_projectid <= 0) {
		// DEVELTEXT("%s bad configured, db_ProjectId for VarSet not defined",__func__);
		return false;
	}

	DBquery q(db, DBquery::SELECT, item.dbTableName);
	q.prepareWhere("vset_projectid =",db_vset_projectid);
	q.prepareWhere("vset_index =",db_vset_index);

	if (q.execPrepared()) {
		if (q.size() > 1) {
			DEBUGERROR("VarSet::dbLoad: Search was ambiguous in '%s', projectid:%llu, index:%d"
					   ,item.dbTableName.toLatin1().data(), db_vset_projectid, db_vset_index);
			return false;
		}
		else if (q.size() == 0) {
			if (debug > 2) DEBUGTEXT("VarSet::dbLoad: No record found in '%s' for projectid:%llu, index:%d"
									 ,item.dbTableName.toLatin1().data(), db_vset_projectid, db_vset_index);
			return true;
		}
		else {
			if (exists) *exists = true;
			int i=0;
			while (i<varList.size()) {
				PrefVarCore *var = varList.at(i);
				QVariant variant;
				q.getValue(var->pVarName(), variant);
				var->pSetValue(variant);
				i++;
			}
			return (q.error == 0);
		}
	} else {
		return false;
	}
}



/**
 * @brief Dynamische DBfield TableDefinition erzeugen
 * @return Pointer auf DBField Array mit allen Einträgen, die für Databasetabellenbearbeitung nötig sind
 *
 * Achtung! Der hier benutzte Speicherbereich wird dynamisch allokiert und muss nach
 * Verwendung mit clearDbTableDefinitionQuery(DBfield * addresse) wieder freigegeben werden.
 */
DBfield *VarSet::getDynamicDbTableDefinition()
{
	int max_fields = varList.size()+20;

	DBfield *field = new DBfield[max_fields];
	memset((void*)field,0,max_fields * sizeof(DBfield));

	int f_idx = 0;

	field[f_idx].com = strdup("_COM");
	field[f_idx].para = strdup("_TABLENAME");
	field[f_idx++].attr = strdup(QString("vset_%1").arg(myclassname).toLatin1().data());

	field[f_idx].com = strdup("id");
	field[f_idx].para = strdup("INT");
	field[f_idx++].attr = strdup("DEF AUTO_INCREMENT");

	field[f_idx].com = strdup("vset_projectid");
	field[f_idx].para = strdup("BIGINT UNSIGNED");
	field[f_idx++].attr = strdup("DEF");

	field[f_idx].com = strdup("vset_index");
	field[f_idx].para = strdup("INT");
	field[f_idx++].attr = strdup("DEF");

	field[f_idx].com = strdup("vset_date_modified");
	field[f_idx].para = strdup("TIMESTAMP");
	field[f_idx++].attr = strdup("DEF DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP");

	for (int t=0; t<varList.size(); t++) {
		PrefVarCore *var = varList.at(t);
		const char *dbtype = 0;
		// Variablentyp bestimmen und entsprechend in der Database setzen
		switch (var->pType()) {
		case PrefVarCore::INT64:
			dbtype = db_type_strings[0];
			break;
		case PrefVarCore::INT32:
			dbtype = db_type_strings[1];
			break;
		case PrefVarCore::STRING:
			dbtype = db_type_strings[2];
			break;
		case PrefVarCore::BOOL:
			dbtype = db_type_strings[3];
			break;
		case PrefVarCore::VARSET:
		case PrefVarCore::VARSET_LIST:
			dbtype = db_type_strings[4];
			DEBUGERROR("%s: Implement me: Line %d",Q_FUNC_INFO,__LINE__);
			break;
		}

		// Field zusammenbauen
		field[f_idx].com = strdup(var->pVarName().toLatin1().data());
		field[f_idx].para = strdup(dbtype);
		field[f_idx++].attr = strdup("DEF");

		Q_ASSERT(f_idx < max_fields-10 );
	}

	field[f_idx].com = strdup("_COM");
	field[f_idx].para = strdup("_PRIMARY");
	field[f_idx++].attr = strdup("id");

	field[f_idx].com = strdup("_COM");
	field[f_idx].para = strdup("_INDEX");
	field[f_idx++].attr = strdup("vset_projectid,vset_index");

	field[f_idx].com = strdup("_COM");
	field[f_idx].para = strdup("_TABLECOMMENT");
	field[f_idx++].attr = strdup(description.toLatin1().data());

	field[f_idx].com = strdup("_COM");
	field[f_idx].para = strdup("ENGINE");
	field[f_idx++].attr = strdup("MYISAM");

	field[f_idx].com = strdup("_COPY_RULE_PROJECT");
	field[f_idx].para = strdup("vset_projectid");
	field[f_idx++].attr = strdup("");

	field[f_idx].com = strdup("");
	field[f_idx].para = strdup("");
	field[f_idx].attr = strdup("");

	return field;
}

void VarSet::clearDbTableDefinitionQuery(DBfield *field)
{
	int idx = 0;
	bool del = true;
	while (del) {
		// Abbruchbedingung leerer com string -> muss aber noch deleted werden
		del = (strlen(field[idx].com) > 0);

		free((void*)field[idx].com);
		free((void*)field[idx].para);
		free((void*)field[idx].attr);

		idx++;
	}

	delete[] field;
}

bool VarSet::isRegistered(VarSet::RegVarType reg_type)
{
	return isRegistered(reg_type, this);
}


bool VarSet::isRegistered(VarSet::RegVarType reg_type, VarSet *obj, RegVarItem **rvi)
{
	bool registered = false;
	if (rvi) *rvi = 0;

	var_registry->lock();
	int i = 0;
	while (i<var_registry->size() && !registered) {
		VarSet::RegVarItem *reg = var_registry->at(i);
		if (reg->object == obj && reg->type == reg_type) {
			registered = true;
			if (rvi) *rvi = reg;
		} else {
			i++;
		}
	}
	var_registry->unlock();

	return registered;
}

bool VarSet::unRegister(VarSet::RegVarType reg_type, VarSet *obj)
{
	VarSet::RegVarItem *reg;
	if (isRegistered(reg_type, obj, &reg)) {
		if (!var_registry->lockRemoveOne(reg)) {
			// qFatal("%s: unregister failed: %s",__func__,obj->myclassname.toLatin1().data());
		}
		delete reg;
		return true;
	} else {
		// DEBUGERROR("%s: VarSet was not registered: %s",__func__,obj->myclassname.toLatin1().data());
		return false;
	}
}

VarSet::RegVarItem VarSet::getRegisterItemCopy(VarSet::RegVarType reg_type, VarSet *obj)
{
	var_registry->lock();
	int i = 0;
	while (i<var_registry->size()) {
		VarSet::RegVarItem *reg = var_registry->at(i);
		if (reg->object == obj && reg->type == reg_type) {
			VarSet::RegVarItem regcopy = *reg;
			var_registry->unlock();
			return regcopy;
		}
		i++;
	}
	var_registry->unlock();
	return VarSet::RegVarItem();
}


/**
 * @brief Liste mit VarSets zurückgeben, die mit dem Type DatabaseTable (REG_DB_TABLE) registriert sind
 * @return QList mit Pointern auf entsprechende VarSets
 */
QList<VarSet *> VarSet::getDatabaseTableVarLists()
{
	QList<VarSet*>list;
	QStringList table_names;

	var_registry->lock();
	int i = 0;
	while (i<var_registry->size()) {
		VarSet::RegVarItem *reg = var_registry->at(i);
		if (reg->type == VarSet::REG_DB_TABLE && !table_names.contains(reg->dbTableName)) {
			list.append(reg->object);
			table_names.append(reg->dbTableName);
		}
		i++;
	}
	var_registry->unlock();
	return list;
}



QStringList VarSet::getRegistryInfo()
{
	QStringList out;
	if (var_registry->isEmpty()) {
		out.append("No VarSet is registered");
	} else {
		for (int t=0; t<var_registry->size(); t++) {
			VarSet::RegVarItem *var = var_registry->at(t);
			QString line = var->className;
			switch (var->type) {
			case VarSet::REG__NONE:
				line += ", REG_NONE";
				break;
			case VarSet::REG_QSET_GLOBAL:
				line += ", REG_QSET";
				break;
			case VarSet::REG_DB_GLOBAL:
				line += ", REG_DB_GLOBAL";
				break;
			case VarSet::REG_DB_TABLE:
				line += ", REG_DB_TABLE";
				break;

			}
			line.append(QString(", 0x%1, ").arg((quint64)var->object,0,16));
			line.append(var->coderComment);

			out.append(line);
		}
	}

	std::sort(out.begin(), out.end());
	for (int t=0; t<out.size();t++) {
		qDebug() << out.at(t);
	}

	return out;
}


VarSet::RegVarItem::RegVarItem()
{
	isValid = false;
	type = VarSet::REG__NONE;
	varClass = PrefVarCore::NO_CLASS;
	object = 0;
}

VarSet::RegVarItem::RegVarItem(VarSet::RegVarType t, PrefVarCore::VarClass myclass, const QString &classname, VarSet *obj)
{
	isValid = true;
	type = t;
	className = classname;
	varClass = myclass;
	object = obj;
}
