/** @file dbquery.h */
/***************************************************************************
*   Copyright (C) 2008-2012 QASS GmbH                                     *
*                                                                         *
*   Ulrich Seuthe <useuthe@qass.net>                                      *
*   Peter Steinmeyer <steinmeyer@qass.net>                                *
*   Christian Günther <c.guenther@qass.net>                               *
*                                                                         *
*  This file is part of the OPTIMIZER project                             *
***************************************************************************/
#ifndef DBQUERY_H
#define DBQUERY_H

#include <QtSql>
#include "toolclasses.h"

#ifdef DELETE
#undef DELETE
#endif


class Database;

class DBquery {
public:
	enum QueryMode {
		NONE,
		INSERT,
		REPLACE,
		SELECT,
		DELETE,
		UPDATE
	};

	int error;
	bool valid_f;
	bool rec_valid_f;
	QString name;

private:
	Database * database;
	QSqlQuery * query;
	QSqlQuery * query_id;
	QSqlRecord * record;
	QueryMode mode;
	QString fromtable;
	QString select_string;
	QString where_string;
	QString order_string;
	quint64 where_id;
	int last_delete_count;
	int last_inserted;
	int last_updated;
	int last_id;
	bool exec_f;
	bool first_next_f;
	QList<QVariant>val_list;
	QStringList field_list;
	QList<QVariant>idx_val_list;
	QStringList idx_field_list;
	static SafeMutex glob_mutex;
	static QThread * current_thread;
	static int dbquery_cnt;             ///< Zähler für Anzahl der DBquery Objekte
	static QList <DBquery *> dbquery_list;

public:
	DBquery(Database * p_database, QueryMode p_mode = SELECT, const QString & table = "");
	DBquery(Database * p_database, const QString & table);
	~DBquery();
	bool prepareInsert(QString table = "");
	bool prepareReplace(QString table = "");
	bool prepareUpdate(QString table = "");
	bool prepareSelect(QString select);
	void prepareTable(const QString & tab) {fromtable = tab;}
	bool prepareSelect(QString table, QString select);
	bool prepareDelete(const QString & table = "");
	bool prepareWhere(int search_id);
	bool prepareWhere(const QString & where);
	bool prepareWhere(const QString & field, QVariant value);
	bool prepareValue(const QString & field, QVariant value);
	void prepareOrder(const QString & order);
	bool execPrepared();
	bool getValue(const QString & field, QString & paradest);
	bool getValue(const QString & field, QDateTime & paradest);
	bool getValue(const QString & field, qint64 & paradest);
	bool getValue(const QString & field, quint64 & paradest);
	bool getValue(const QString & field, int & paradest);
	bool getValue(const QString & field, uint & paradest);
	bool getValue(const QString & field, bool & paradest);
	bool getValue(const QString & field, double & paradest);
	bool getValue(const QString & field, float & paradest);
	bool getValue(const QString & field, QByteArray & paradest);
	bool getValue(const QString & field, QVariant &paradest);
	bool nextRecord();
	bool firstRecord();
	bool lastRecord();


	int size();
	bool valid();
	int testId(int p_id);
	int lastID() {return last_id;}
	inline int lastDeleteCount() {return last_delete_count;}
	inline int lastInserted() {return last_inserted;}
	inline int lastUpdated() {return last_updated;}
	int testWhere(const QString & field,const QVariant & val);
	QSqlQuery * sqlQuery() {return query;}
	QSqlQuery * select(const QString & select, const QString & where);
	void setName(const QString & txt);
	const QString & connectionName(uint idx);
	Database * getDatabase() {return database;}
	static Database * getGlobalDatabase();

private:
	void init_mode(QueryMode p_mode);
	bool exec_insert();
	bool exec_replace();
	bool exec_update();
	bool exec_get();
	bool exec_select();
	bool exec_delete();
	bool update_record();
	bool test_record();
	void assemble_where_string_from_list();
	int field_col(const QString & field);
	void set_error(const QString & sql);
};

#endif
