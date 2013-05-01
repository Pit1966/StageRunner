/** @file prefvar.h */
#ifndef PREFVAR_H
#define PREFVAR_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>
#include <QDebug>

#include "toolclasses.h"

class QSettings;
class VarSet;


/**
	@author QASS GmbH <steinmeyer@qass.net>
*/
// -------------------------------------------------------------------------------------------------------------------
//  class PrefVarCore
// -------------------------------------------------------------------------------------------------------------------

class PrefVarCore : public QObject {

	Q_OBJECT

public:
	enum PrefVarType {			///< Die einzelnen Variablentypen. Unbedingt synchron halten mit VarSet::db_type_strings !!
		INT64,
		INT32,
		STRING,
		BOOL,
		VARSET,
		VARSET_LIST
	};

	enum VarClass {
		NO_CLASS,				///< Wird zur Zeit auch per Voreinstellung in ".config" gespeichert
		VAR_SET_CLASS,			///< Das ist die Default-Klasse für pVars, die in einem VarSet untergebracht sind
		SYSTEM_CONFIG,			///< Die Variable enthält System-Einstellungen, die später auch in ".config" gespeichert werden
		USER_CONFIG,			///< Wird in "./config" in der USER Sektion gespeichert
		PROJECT,				///< Globale Triggereinstellung
		FX_ITEM,
		DMX_CHANNEL
	};

	/**
	 * Function Type definitions of a PrefVarCore variable
	 */
	enum Function {
		FUNC_NORMAL,
		FUNC_FUNCTION,
		FUNC_VARSET,
		FUNC_VARSET_LIST_HEADER
	};

protected:
	QString myname;				///< Name der Variable wie er in Textdateien und in der Datenbank angelegt wird
	QString dispname;			///< Anzeigename der Variable auf der Oberfläche
	QString description;		///< Beschreibung der Variable
	PrefVarType mytype;			///< Der Type der Variable
	VarClass myclass;			///< Die Variablen Klasse (wo wird sie verwendet)
	QString myclassname;		///< Noch ein Bezeichner für die Klasse
	bool initialized_f;			///< true, falls die Variable initialisiert wurde
	bool modified_f;			///< Variable wurde modifiziert
	Function function;			///< zeigt an, dass es sich um eine Funktionsvariable handelt
	bool exists_in_file_f;		///< Falls Variable Teil eines VarSets ist, wird hier angezeigt, ob Variable im gelesenen File vorhanden war

	QAtomicInt refcnt;
	void * p_refvar;			///< Variable existiert nur als Referenz auf normale Variable

private:
	QList<VarSet *> parent_var_sets;						///< Variable ist Member dieser Variablensätze, bzw. Objecte

	static MutexQList<PrefVarCore *> *glob_var_list;		///< globale Liste mit allen Variablen
	static int instance_cnt;								///< Zähler über instanzierte Objekte

public:
	PrefVarCore(PrefVarType type, const QString & name = "");
	~PrefVarCore();

	inline bool isModified() {return modified_f;}
	inline void setModified(bool state) {modified_f = state;}
	bool writeToPrefGroup(QSettings * setting, const QString & group = "");
	bool readFromPrefGroupSystem(QSettings * setting, const QString & group = "");
	inline QString & pVarName() {return myname;}
	inline QString & pDisplayName() {return dispname;}
	inline QString & pDescription() {return description;}
	inline PrefVarType pType() {return mytype;}
	/**
	 * @brief Variable auslesen [virtual]
	 * @return QVariant mit Wert der Variable
	 *
	 * Die Funktion erkennt, ob es sich bei die Variable selbst den Wert speichert, oder ob
	 * es sich um eine Referenz auf eine andere existierende Std Variable handelt. (sofern
	 * die virtuelle Methode richtig implementiert wurde)
	 */
	inline QVariant pValue() const {return get_value();}
	inline QVariant pDefaultValue() const {return get_default();}
	inline void pSetValue(QVariant val) {set_value(val);}
	inline VarClass varClass() {return myclass;}
	inline const QString & varClassName() {return myclassname;}
	/**
	 * @brief Prüfen, ob Variable beim Laden aus File vorhanden war
	 * @return true, falls Variablenname im File steht
	 */
	inline bool existsInFile() {return exists_in_file_f;}
	inline bool isInitialized() {return initialized_f;}
	inline Function getFunction() {return function;}
	inline bool isFunction() {return function != FUNC_NORMAL;}

	static void debugDumpAllVars();
	static bool writeAllToPref();
	static bool readAllFromPref();
	static QList<PrefVarCore *> & globalVarList() {return *glob_var_list;}

protected:
	virtual void cloneFrom(const PrefVarCore & other);

private:
	void init();
	virtual QVariant get_value() const {return QVariant();}
	virtual void set_value(QVariant val) {Q_UNUSED(val)}
	virtual QVariant get_default() const {return QVariant();}

	friend class VarSet;
};


// -------------------------------------------------------------------------------------------------------------------
//  class pbool
// -------------------------------------------------------------------------------------------------------------------

class pbool : public PrefVarCore {
	Q_OBJECT;
private:
	bool pvalue;
	bool pdefault;

public:
	pbool();
	~pbool();
	void initPara(const QString & name, bool p_default, const QString & descrip = "");
	inline operator bool () { return pvalue; }								///< Zuweisung (Type conversion):  qint64 val = pint64
	inline operator QObject * () { return this; }
	inline bool value() {return pvalue;}
	/// Zuweisung: pbool val = bool
	inline pbool & operator= (bool val) {
		if (val!=pvalue) {pvalue=val;modified_f=true;}
		return *this;
	}
	void cloneFrom(const PrefVarCore &other);

private:

	void set_value(QVariant val);									///< Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_value() const;										///< Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_default() const {return pdefault;} 				///< Implementierung der virtuellen Methode aus der Basisklasse

public slots:
	void setValue(bool val);

	friend class VarSet;
};



// -------------------------------------------------------------------------------------------------------------------
//  class pint64
// -------------------------------------------------------------------------------------------------------------------

class pint64 : public PrefVarCore {
	Q_OBJECT;
private:
	qint64 pmin;
	qint64 pmax;
	qint64 pvalue;
	qint64 pdefault;

public:
	pint64();
	~pint64();
	void initPara(const QString & name, qint64 p_min, qint64 p_max, qint64 p_default, const QString & descrip = "");
	inline operator qint64 () { return pvalue; }								///< Zuweisung (Type conversion):  qint64 val = pint64
	inline operator QObject * () { return this; }
	inline qint64 value() {return pvalue;}
	/// Zuweisung: pint64 val = qint64
	inline pint64 & operator = (qint64 val) {
		if (pvalue!=val) {pvalue=val;modified_f=true;}
		return *this;
	}
	inline qint64 minimum() {return pmin;}
	inline qint64 maximum() {return pmax;}
	void cloneFrom(const PrefVarCore &other);

private:
	void set_value(QVariant val);								/// Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_value() const;									///< Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_default() const {return pdefault;} 			///< Implementierung der virtuellen Methode aus der Basisklasse

public slots:
	void setValue(qint64 val);
	void setValue(int val);

	friend class VarSet;
};

// -------------------------------------------------------------------------------------------------------------------
//  class pint32
// -------------------------------------------------------------------------------------------------------------------

class pint32 : public PrefVarCore {
	Q_OBJECT;
private:
	qint32 pmin;
	qint32 pmax;
	qint32 pvalue;
	qint32 pdefault;

public:
	pint32();
	~pint32();
	void initPara(const QString & name, qint32 p_min, qint32 p_max, qint32 p_default, const QString & descrip = "");
	inline operator qint32 () { return pvalue; }				///< Zuweisung (Type conversion):  qint32 val = pint32
	inline operator QObject * () { return this; }
	inline qint32 value() {return pvalue;}
	/// Zuweisung: pint64 val = qint32
	inline pint32 & operator = (qint32 val) {
		if (pvalue != val) {pvalue=val;modified_f=true;}
		return *this;
	}
	inline qint32 minimum() {return pmin;}
	inline qint32 maximum() {return pmax;}
	void cloneFrom(const PrefVarCore &other);

private:
	void set_value(QVariant val);							/// Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_value() const;								///< Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_default() const {return pdefault;}				///< Implementierung der virtuellen Methode aus der Basisklasse

public slots:
	void setValue(int val);

	friend class VarSet;
};

// -------------------------------------------------------------------------------------------------------------------
//  class pstring
// -------------------------------------------------------------------------------------------------------------------

class pstring : public PrefVarCore {
	Q_OBJECT;
private:
	QString pvalue;
	QString pdefault;

public:
	pstring();
	~pstring();
	void initPara(const QString & name, const QString & p_default, const QString & descrip = "");
	inline QString value() {return pvalue;}
	inline int size() {return pvalue.size();}
	inline operator QString () {return pvalue;}
	inline pstring & operator = (const QString val) {
		if (pvalue != val) {pvalue=val;modified_f=true;}
		return *this;
	}
	void cloneFrom(const PrefVarCore &other);

private:

	void set_value(QVariant val);								///< Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_value() const;									///< Implementierung der virtuellen Methode aus der Basisklasse
	QVariant get_default() const {return pdefault;}				///< Implementierung der virtuellen Methode aus der Basisklasse

public slots:
	void setValue(const QString & text);

	friend class VarSet;
};

#endif
