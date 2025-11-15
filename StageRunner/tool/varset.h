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

#ifndef PREFVARSET_H
#define PREFVARSET_H

#include <QList>
#include <QAtomicInt>
#include <QStringList>


#include "prefvar.h"
#include "toolclasses.h"


class PrefVarCore;
class Database;
class DBfield;

template <class T> class VarSetList;

class VarSet : public QObject
{
	Q_OBJECT
public:
	enum RegVarType {
		REG__NONE,				///< keine Aktion
		REG_QSET_GLOBAL,		///< Ist für Gebrauch in Globaler analyzer4D.conf Datei in .config eingetragen
		REG_DB_GLOBAL,			///< Ist für Gebrauch in Database Tabelle 'globalsettings' eingetragen)
		REG_DB_TABLE			///< Ist für Gebrauch mit eigenständiger Databasetabelle registriert
	};

	class RegVarItem {
	public:
		bool isValid;
		RegVarType type;				///< Der Typ des registrierten Objekts
		PrefVarCore::VarClass varClass;
		QString className;				///< Klassenname und damit auch Tabellen oder Gruppenname (Tabellenname wird mit 'vset_' ergänzt)
		QString dbTableName;			///< Das wird der Tabellenname, falls VarSer als Datenbank Objekt (nicht global) registriert wird
		VarSet *object;					///< Pointer auf registriertes Objekt
		QString coderComment;			///< Kommentar, der auf den Ort der Benutzung hinweisen sollte
	public:
		RegVarItem();
		RegVarItem(RegVarType t, PrefVarCore::VarClass myclass, const QString & classname, VarSet *obj);
	};

protected:
	// Temp Vars fileLoad
	QString curClassname;					///< Der aktuell gelesene Klassenname (Gruppenname)
	QString curProjectid;					///< Die aktuell gelesene Projekt ID für das VarSet
	QString curIndex;						///< Die aktuell gelesene Index nummer (VarSet Zähler)
	QString curKey;
	QString curValue;
	QString curChildListName;
	QString curChildItemClass;
	QString curContextClassName;
	QString errorString;
	bool curChildActive;

	// Essential vars
	PrefVarCore::VarClass myclass;			///< Die Klasse des Sets, gibt an, wo die Variablen Verwendung finden
	QString myclassname;					///< The class name string
	MutexQList<PrefVarCore *>varList;		///< Die Liste mit allen Variablen, die zu diesem Set gehören

private:
	QString name;							///< Der Name des Sets
	QString description;					///< Beschreibung des Sets

	bool modified_f;						///< Eine der Variablen im Set wurde modifiziert
	int db_global_idx;						///< Index, der für das speichern der Settings in die globale Datenbanktabelle verwendet wird
	int db_vset_index;						///< Index, der für das speichern eines Eintrags in DB-Tabelle verwendet wird
	quint64 db_vset_projectid;				///< ProjectId, die für das speichern der Settings in die Datenbanktabelle verwendet wird (1 für global setting)
	bool is_registered_f;					///< VarSet ist mindestens einmal in var_registry vertreten
	bool is_db_table_registered_f;			///< Ist mit registerDatabaseTable registriert worden
	int function_count;						///< Zähler für spezielle Funktionsvariablen (Variablenname wird "function|{$function_count}")
	bool cancel_file_analyze_on_empty_line;	///< Die Analyse eines VarSetFiles wird ohne Fehler abgebrochen, wenn eine Leerzeile auftaucht
	bool leave_child_level_on_empty_line;

	// Varwaltung aller VarSets
	static QAtomicInt instance_cnt;					///< Soviele VarSet Instanzen gibt es
	static const char *db_type_strings[5];			///< Tabelle mit korrespondierenden MySQL Variablen Typen
	static MutexQList<RegVarItem*>*var_registry;	///< Liste, die alle registrierten Instanzen nachhält


public:
	VarSet();
	VarSet(const VarSet & other);
	virtual ~VarSet();

	VarSet & operator= (const VarSet & other);
	void cloneFrom(const VarSet &other);

	bool checkModified();
	void setModified(bool state = true, bool all = false);
	bool isModified() const {return modified_f;}

	inline PrefVarCore::VarClass classType() const {return myclass;}
	inline const QString & className() const {return myclassname;}

	/**
	 * @brief setName Den Namen des VarSets festlegen
	 * @param n String mit dem Namen
	 *
	 * Dieser String wird auch der Tabellenname, falls das VarSet in eine Datei gespeichert wird
	 */
	inline void setName(const QString & n) {name = n;}
	/**
	 * @brief Beschreibung des VarSets festlegen
	 * @param d String mit Text, der den Sinn dieses Sets beschreibt
	 */
	inline void setDescription(const QString & d) {description = d;}

	bool addExistingVar(pint32 & var, const QString & name, qint32 p_min = 0, qint32 p_max = 0x7FFFFFFF, qint32 p_default = 0, const QString & descrip = "");
	bool addExistingVar(pint64 & var, const QString & name, qint64 p_min = 0, qint64 p_max = 0x7FFFFFFFFFFFFFFFll, qint64 p_default = 0, const QString & descrip = "");
	bool addExistingVar(pstring & var, const QString & name, const QString & p_default = "", const QString & descrip = "");
	bool addExistingVar(pbool & var, const QString & name, bool p_default = false, const QString & descrip = "");

	bool addExistingVar(qint32 & var, const QString & name, qint32 p_min = 0, qint32 p_max = 0x7FFFFFFF, qint32 p_default = 0, const QString & descrip = "");
	bool addExistingVar(qint64 & var, const QString & name, qint64 p_min = 0, qint64 p_max = 0x7FFFFFFFFFFFFFFFll, qint64 p_default = 0, const QString & descrip = "");
	bool addExistingVar(bool & var, const QString & name, bool p_default = false, const QString & descrip = "");
	bool addExistingVar(QString &var, const QString & name, const QString & p_default = "", const QString & descrip = "");
	bool addExistingVar(VarSet &var, const QString &name, const QString & descrip = "");
	template <typename T> bool addExistingVarSetList(VarSetList<T> &var, const QString &name, PrefVarCore::VarClass p_class) {
			if (exists(name)) return false;
			PrefVarCore *newvar = new PrefVarCore(PrefVarCore::VARSET_LIST,name);
			newvar->parent_var_sets.append(this);
			newvar->contextClass = p_class;
			newvar->p_refvar = (void *) &var;
			newvar->function = PrefVarCore::FUNC_VARSET_LIST_HEADER;
			varList.lockAppend(newvar);
			return true;
	}



	pint32 * addDynamicPint32(const QString & name, qint32 p_min=0, qint32 p_max=0x7fffffff, qint32 p_default=0, const QString & descrip = "");
	pint64 * addDynamicPint64(const QString & name, qint64 p_min=0, qint64 p_max=0x7fffffffffffffffll, qint64 p_default=0, const QString & descrip = "");
	pstring * addDynamicPstring(const QString & name, const QString & p_default = "", const QString & descrip = "");
	pbool * addDynamicPbool(const QString & name, bool p_default = false, const QString & descrip = "");
	pstring * addDynamicFunction(const QString & func_name, const QString & func_para);


	inline bool contains(PrefVarCore * varcore) {return varList.lockContains(varcore);}
	bool removeOne(PrefVarCore * varcore);

	bool exists(const QString & name);
	PrefVarCore *getVar(const QString & name, PrefVarCore::Function * function_type_p = 0);
	PrefVarCore * find(const QString & name, PrefVarCore::PrefVarType type);
	int find(const PrefVarCore *other);

	bool getPbool(const QString & name);
	qint32 getPint32(const QString & name);
	qint64 getPint64(const QString & name);
	QString getPstring(const QString & name);
	char * getPstringAscii(const QString & name);

	QVariant pValue(const QString & name);

	void setVar(const QString & name, bool);
	void setVar(const QString & name, qint32 val);
	void setVar(const QString & name, qint64 val);
	void setVar(const QString & name, const QString & str);

	int elements() {return varList.size();}

	/**
	 * @brief Den Klassentyp und -namen des VarSets festlegen
	 * @param classtype Type @see PrefVarCore::VarClass
	 * @param classname Klassenname als String
	 *
	 * Dieser String wird auch der Tabellenname, falls das VarSet in eine DB gespeichert wird
	 * bzw. der Groupname beim Speichern in QSettings
	 */
	void setClass(PrefVarCore::VarClass classtype, const QString & classname);
	void debugListVars();
	bool writeToPref();
	bool readFromPref();

	bool fileSave(const QString & path, bool append = false, bool empty_line = false);
	int fileLoad(const QString & path, bool *exists = 0, int *lineNumber = 0, QString *lineCopy = 0);
	void setFileLoadCancelOnEmptyLine(bool state) {cancel_file_analyze_on_empty_line = state;}

	bool registerDatabaseGlobal(const QString &desc, int index = 1);
	bool registerDatabaseTable(const QString &desc);
	bool registerQSetGlobal(const QString &desc);
	bool setDatabaseReferences(quint64 projectid, int index = 1);
	bool dbSaveGlobal(Database *db);
	bool dbLoadGlobal(Database *db);
	bool dbSave(Database *db);
	bool dbLoad(Database *db, bool *exists = 0);

	DBfield * getDynamicDbTableDefinition();

	bool isRegistered(RegVarType reg_type);
	static void clearDbTableDefinitionQuery(DBfield *field);
	static QStringList getRegistryInfo();
	static bool isRegistered(RegVarType reg_type, VarSet *obj, RegVarItem **rvi = 0);
	static bool unRegister(RegVarType reg_type, VarSet *obj);
	static RegVarItem getRegisterItemCopy(RegVarType reg_type, VarSet *obj);
	static QList<VarSet *>getDatabaseTableVarLists();

	static inline bool isVarSet() {return true;}

protected:
	int analyzeLoop(QTextStream &read, VarSet *varset, int child_level, int *p_line_number, QString *lineCopy);
	int analyzeLine(QTextStream &read, VarSet *varset, int child_level, int * p_line_number, QString *lineCopy);
	void clearCurrentVars();

private:
	void add_to_registry(RegVarItem* reg);
	void clear_var_list();
	void init();
	bool file_save_append(QTextStream &write, int child_level, bool append_empty_line);
	QString child_indent_str(int childLevel);

};

#endif // PREFVARSET_H
