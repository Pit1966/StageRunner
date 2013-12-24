/** @file dbquery.cpp */
/***************************************************************************
*   Copyright (C) 2008-2010 QASS GmbH                                     *
*                                                                         *
*   Ulrich Seuthe <useuthe@qass.net>                                      *
*   Peter Steinmeyer <steinmeyer@qass.net>                                *
*   Christian Günther <c.guenther@qass.net>                               *
*                                                                         *
*  This file is part of the OPTIMIZER project                             *
***************************************************************************/

#include "dbquery.h"
#include "database.h"
#include "../system/log.h"
#include "toolclasses.h"

extern Log *logThread;
extern QThread * main_thread;

QMutex DBquery::glob_mutex;
int DBquery::dbquery_cnt = 0;
QThread * DBquery::current_thread = 0;
QList <DBquery *> DBquery::dbquery_list;


DBquery::DBquery(Database * p_database, QueryMode p_mode, const QString & table)
{
	QThread * mythread = logThread->currentThread();
	if (debug > 4) DEVELTEXT("DBquery for '%s'",mythread->objectName().toLatin1().data());

	if (!(database = p_database)) {
		valid_f = false;
		ERRORNOP("DBquery::DBquery",ERR_DB_NULL_POINTER_QUERY);
	} else {

		int dbidx = p_database->getConnectionIndex(mythread);
		if (dbidx >= 0) {
			QSqlDatabase db = QSqlDatabase::database(connectionName(dbidx));
			query = new QSqlQuery(db);
			query_id = new QSqlQuery(db);
			record = new QSqlRecord();
			valid_f = database->isValid() && query && query_id && record;
		} else {
			valid_f = false;
		}


		init_mode(p_mode);

		switch (p_mode) {
		case INSERT:
			prepareInsert(table);
			break;
		case REPLACE:
			prepareReplace(table);
			break;
		case DELETE:
			prepareDelete(table);
			break;
		case SELECT:
			prepareSelect(table,"*");
			break;
		case UPDATE:
			prepareUpdate(table);
			break;
		default:
			break;
		}
	}
}

DBquery::DBquery(Database * p_database, const QString & table) : database(p_database)
{
	QThread * mythread = logThread->currentThread();
	if (debug > 4) DEVELTEXT("DBquery for '%s'",mythread->objectName().toLatin1().data());
	if (!(database = p_database)) {
		valid_f = false;
		ERRORNOP("DBquery::DBquery",ERR_DB_NULL_POINTER_QUERY);
	} else {
		int dbidx = p_database->getConnectionIndex(mythread);
		if (dbidx >= 0) {
			QSqlDatabase db = QSqlDatabase::database(connectionName(dbidx));
			query = new QSqlQuery(db);
			query_id = new QSqlQuery(db);
			record = new QSqlRecord();
			valid_f = database->isValid() && query && query_id && record;
		} else {
			valid_f = false;
		}

		init_mode(SELECT);
		fromtable = table;
	}
}


void DBquery::setName(const QString & txt)
{
	name = txt;
	if (!dbquery_list.contains(this)) {
		dbquery_list.append(this);
	}
}

inline const QString & DBquery::connectionName(uint idx)
{
	return database->connectionName(idx);
}

Database *DBquery::getGlobalDatabase()
{
	return Database::getGlobalInstance();
}

DBquery::~DBquery() {
	if (query) delete query;
	if (query_id) delete query_id;
	if (record) delete record;

	if (dbquery_cnt > 0) {
		dbquery_cnt--;
	}
	else {
		if (current_thread) DEBUGERROR("DBquery::~DBquery: DBquery count is %d ",dbquery_cnt);
		dbquery_cnt = 0;
	}

	// Aktiven DBquery für Thread austragen, wenn letztes Objekt zerstört wurde
	if (dbquery_cnt == 0) {
		current_thread = 0;
	}

	database->freeConnectionIndex(logThread->currentThread());

	dbquery_list.removeAll(this);
}


QSqlQuery * DBquery::select(const QString & select, const QString & where)
{
	init_mode( SELECT );
	exec_f = false;
	if (!fromtable.size()) {
		ERRORPARA("DBquery::select",ERR_DB_QUERY_NOTABLE, select + where);
		return query;
	}
	QString sql = "SELECT " + select;
	sql += " FROM " + fromtable;
	if (where.size()) sql += " WHERE " + where;
	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());

	bool ok = query->exec(sql);
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::select",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	}
	exec_f = true;
	return query;
}

bool DBquery::prepareSelect(QString select)
{
	// mode setzen
	init_mode( SELECT );
	if (!fromtable.size()) {
		ERRORNOP("DBquery::prepareSelect",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	select_string = select;
	return true;
}

bool DBquery::prepareSelect(QString table, QString select)
{
	fromtable = table;
	return prepareSelect(select);
}

bool DBquery::prepareDelete(const QString & table)
{
	init_mode( DELETE );
	fromtable = table;
	if (!fromtable.size()) {
		ERRORNOP("DBquery::prepareDelete",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	return true;
}

bool DBquery::prepareWhere(int search_id)
{
	if (!fromtable.size()) {
		ERRORNOP("DBquery::prepareWhere(id)",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	where_string = "id = " + QString::number(search_id);
	where_id = search_id;
	return true;
}

bool DBquery::prepareWhere(const QString & where)
{
	if (!fromtable.size()) {
		ERRORNOP("DBquery::prepareWhere(String)",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	where_string = where;
	return true;
}

bool DBquery::prepareWhere(const QString & field, QVariant value)
{
	if (error) return false;
	if (!field.size()) {
		ERRORNOP("DBquery::prepareWhere(String,Variant)",ERR_DB_INVALID_FIELD);
		return false;
	}
	bool flag = false;
	if (exec_f) {
		for (int t=0;t<idx_field_list.size();t++) {
			if (idx_field_list.at(t) == field) {
				if (debug > 4) DEBUGTEXT ("DBquery::prepareWhere(String,Variant) -> replace field %s with %s",field.toUtf8().data(),value.toString().toUtf8().data());
				idx_val_list[t] = value;
				flag = true;
			}
		}
	}
	if (!flag) {
		// IndexFeldnamen merken
		idx_field_list.append(field);
		// und IndexValue merken
		idx_val_list.append(value);
	}
	return true;
}

bool DBquery::prepareValue(const QString & field, QVariant value)
{
	if (error) return false;
	if (!field.size()) {
		ERRORNOP("DBquery::prepareValue",ERR_DB_INVALID_FIELD);
		return false;
	}
	// Keine leeren Strings vorbereiten
	if (value.type() == QVariant::String && value.isNull() && (mode == INSERT)) {
		return true;
	}

	// Feldnamen merken
	field_list.append(field);
	// und Value merken
	val_list.append(value);

	return true;
}


bool DBquery::prepareInsert(QString table)
{
	init_mode(INSERT);

	if (!table.size()) {
		table = fromtable;
	} else {
		fromtable = table;
	}
	if (!table.size()) {
		ERRORNOP("DBquery::prepareInsert",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	return true;
}

bool DBquery::prepareReplace(QString table)
{
	// mode setzen
	init_mode( REPLACE );

	if (!table.size()) {
		table = fromtable;
	} else {
		fromtable = table;
	}
	if (!table.size()) {
		ERRORNOP("DBquery::prepareReplace",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	return true;
}

bool DBquery::prepareUpdate(QString table)
{
	// mode setzen
	init_mode( UPDATE );

	if (!table.size()) {
		table = fromtable;
	} else {
		fromtable = table;
	}
	if (!table.size()) {
		ERRORNOP("DBquery::prepareUpdate",ERR_DB_QUERY_NOTABLE);
		return false;
	}
	return true;
}

void DBquery::prepareOrder(const QString & order)
{
	order_string = order;
}

bool DBquery::execPrepared()
{
	if (error) return false;

	if (!(last_inserted || last_updated) && exec_f) {
		ERRORNOP("DBquery::execPrepared",ERR_DB_QUERY_EXECUTED);
		return false;
	} else {
		exec_f = true;
	}


	bool ok = true;

	switch (mode) {
	case INSERT:
		ok = exec_insert();
		break;
	case REPLACE:
		ok = exec_replace();
		break;
	case SELECT:
		ok = exec_select();
		break;
	case DELETE:
		ok = exec_delete();
		break;
	case UPDATE:
		ok = exec_update();
		break;
		case NONE:
				break;
	}

	return ok;
}


bool DBquery::getValue(const QString & field, QString & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) {
		paradest = "not found in db";
		return false;
	}
	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		paradest = query->value(col).toString();
		return true;
	}

	return false;
}

bool DBquery::getValue(const QString & field, QDateTime & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		paradest = query->value(col).toDateTime();
		return true;
	}

	return false;
}

bool DBquery::getValue(const QString & field, qint64 & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		bool ok = false;
		paradest = query->value(col).toLongLong(&ok);
		if (!ok) ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"qint64");
		return ok;
	}
	return false;
}

bool DBquery::getValue(const QString & field, quint64 & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		bool ok = false;
		paradest = query->value(col).toULongLong(&ok);
		if (!ok) ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"quint64");
		return ok;
	}
	return false;
}

bool DBquery::getValue(const QString & field, int & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		bool ok = false;
		paradest = query->value(col).toInt(&ok);
		if (!ok) ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"int");
		return ok;
	}
	return false;
}

bool DBquery::getValue(const QString & field, uint & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		bool ok = false;
		paradest = query->value(col).toUInt(&ok);
		if (!ok) ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"uint");
		return ok;
	}
	return false;
}

bool DBquery::getValue(const QString & field, bool & paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		paradest = query->value(col).toBool();
		return true;
	}
	return false;
}

bool DBquery::getValue(const QString & field, double & paradest) {

	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		bool ok = false;
		paradest = query->value(col).toDouble(&ok);
		if (!ok) ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"double");
		return ok;
	}
	return false;
}

bool DBquery::getValue(const QString & field, float & paradest) {

	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		bool ok = false;
		paradest = query->value(col).toDouble(&ok);
		if (!ok) ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"float");
		return ok;
	}
	return false;
}

bool DBquery::getValue(const QString & field, QByteArray & paradest) {

	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		paradest = query->value(col).toByteArray();
		if (paradest.isEmpty()) {
			ERRORPARA ("DBquery::getValue",ERR_DB_QUERY_CONVERT_FAILED,"BLOB");
			return false;
		}
		return true;
	}
	return false;
}

bool DBquery::getValue(const QString &field, QVariant &paradest)
{
	if (error) return false;
	if (!exec_f) execPrepared();
	if (!query->size()) return false;

	int col = field_col(field);
	if (query->isValid() && col >= 0) {
		paradest = query->value(col);
		return true;
	}
	return false;
}


int DBquery::field_col(const QString & field)
{
	int col = -1;
	if (test_record()) {
		col = record->indexOf(field);
		if (query->isValid() && col < 0) {
			ERRORPARA("DBquery::field_col",ERR_DB_INVALID_FIELD,database->databaseName + "." + fromtable + "." + field);
		}
	}
	return col;
}

bool DBquery::test_record()
{
	if (rec_valid_f) return true;
	return update_record();
}

bool DBquery::update_record()
{
	bool ok = false;
	if (valid_f) {
		if (query->size()) {
			*record = query->record();
			if (!record->isEmpty()) {
				int col = record->indexOf("id");
				if (col >= 0)
					last_id = query->value(col).toInt();
				else
					last_id = 0;

				ok = true;
				rec_valid_f = true;
			} else {
				ok = false;
				rec_valid_f = false;
				last_id = -1;
			}
		}
		else {
			ok = false;
			rec_valid_f = false;
			last_id = -1;
		}
	}
	return ok;
}

bool DBquery::nextRecord()
{
	if (first_next_f) {
		first_next_f = false;
		return firstRecord();
	}
	bool ok = query->next();
	if (ok)
		ok = update_record();

	return ok;
}

bool DBquery::firstRecord()
{
	bool ok = query->first();
	first_next_f = false;

	if (ok)
		ok = update_record();

	return ok;
}

bool DBquery::lastRecord()
{
	bool ok = query->last();
	first_next_f = false;

	if (ok)
		ok = update_record();

	return ok;
}



void DBquery::assemble_where_string_from_list()
{
	for (int t=0;t<idx_field_list.size();t++) {
		if (t) where_string += " AND ";
		where_string += idx_field_list.at(t) + " '" + idx_val_list.at(t).toString() +"'";
	}
}

bool DBquery::exec_select()
{
	bool ok = true;
	if (!select_string.size()) select_string = "*";
	if (!(select_string == "*") && !select_string.contains("id")) select_string += ",id";
	QString sql = "SELECT " + select_string;
	sql += " FROM " + fromtable;
	if (!where_string.size()) assemble_where_string_from_list();

	if (where_string.size()) sql += " WHERE " + where_string;

	if (order_string.size()) sql += " ORDER BY " + order_string;

	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
	ok = query->exec(sql);
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::exec_select",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	} else {
		if (query->size() && query->first()) {
			update_record();
		} else {
			rec_valid_f = false;
			last_id = -1;
		}
	}

	return ok;
}

bool DBquery::exec_delete()
{
	bool ok = true;
	if (!where_string.size()) assemble_where_string_from_list();
	if (!where_string.size()) {
		ERRORPARA("DBquery::exec_delete",ERR_DB_DELETE_WITHOUT_WHERE,fromtable);
		return false;
	}

	QString sql = "DELETE";
	sql += " FROM " + fromtable;
	sql += " WHERE " + where_string;

	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
	ok = query->exec(sql);
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::exec_delete",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	} else {
		last_delete_count = query->numRowsAffected();
		if (debug > 4) DEBUGTEXT("DB->exec: %d records deleted",last_delete_count);
	}

	return ok;
}

bool DBquery::exec_insert()
{
	bool ok = true;
	QString sql;
	// falls der inserted Zähler nicht null ist, ist das query noch "prepared", es müssen
	// also nur noch die neuen Werte gebunden werden
	if (!last_inserted) {
		sql += "INSERT INTO " + database->databaseName + "." + fromtable + " (";
		for (int t=0; t<field_list.size(); t++) {
			if (t) sql += ",";
			sql += field_list.at(t);
		}
		sql += ") VALUES (";
		for (int t=0; t<field_list.size(); t++) {
			if (t) sql += ",";
			sql += "?";
		}
		sql += ")";

		if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
		ok = query->prepare(sql);
	}

	if (ok) {
		for (int t=0 ;t < val_list.size(); t++) {
			if (debug > 5) {
				LOGTEXT(QString("DB->exec: Bind value #%1: %2").arg(t+1).arg(val_list.at(t).toString()));
			}
			if ( val_list.at(t).type() == QVariant::String &&  val_list.at(t).isNull() ) {
				if (debug > 1) DEBUGTEXT("DBquery::exec_insert substitute NULL value to '' in field %s",field_list.at(t).toLatin1().data());
				query->addBindValue("");
			} else {
				query->addBindValue(val_list.at(t));
			}
		}
		ok = query->exec();
	}
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::exec_insert",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	} else {
		field_list.clear();
		val_list.clear();
		last_inserted++;
		last_id = query->lastInsertId().toInt();
		if (debug > 4) DEBUGTEXT("DB->exec: Insert record %d",last_inserted);

	}
	return ok;
}

bool DBquery::exec_replace()
{
	bool ok = true;
	if (idx_field_list.size() == 0 && where_string.size() == 0) {
		ERRORPARA("DBquery::exec_replace(1)",ERR_DB_REPLACE_WITHOUT_WHERE,"no index fields defined by 'prepareWhere'");
		return false;
	}

	if (!where_string.size()) assemble_where_string_from_list();

	// ID des Dateneintrags ermitteln, der aktualisiert werden soll
	QString idsql = "SELECT id FROM " + fromtable ;
	idsql += " WHERE " + where_string;

	if (debug > 4) DEBUGTEXT("DB->exec: %s",idsql.toUtf8().data());
	ok = query_id->exec(idsql);
	if (!ok) {
		QSqlError err = query_id->lastError();
		ERRORPARA("DBquery::exec_replace(1)",ERR_DB_QUERY_FAILED,err.text());
		set_error(idsql);
		return false;
	}
	// Antwort auswerten
	if (query_id->size() == 0) {
		if (debug > 4) DEBUGTEXT("Replace -> Insert (WHERE %s",where_string.toUtf8().data());
		// Kein Datensatz gefunden -> Dann als neu einfügen
		for (int t=0; t<idx_val_list.size(); t++) {
			field_list.prepend(idx_field_list.at(t).section(' ',0,0));
			val_list.prepend(idx_val_list.at(t));
		}
		last_id = 0;
		where_string.clear();
		return exec_insert();
	}
	else if (query_id->size() > 1) {
		ERRORPARA("DBquery::exec_replace(2)",ERR_DB_REPLACE_AMBIGUOUS,where_string);
		return false;
	}

	// Query für Datensatz Update vorbereiten
	QString sql;
	sql = "UPDATE " + fromtable + " SET ";
	for (int t=0;t<field_list.size();t++) {
		if (t) sql += ",";
		if (val_list.at(t).type() == QVariant::Bool)
			sql += field_list.at(t) + " = " + QString::number(val_list.at(t).toInt());
		else
			sql += field_list.at(t) + " = '" + val_list.at(t).toString() + "'";;
	}
	sql += " WHERE " + where_string;

	if (!where_string.size()) {
		ERRORNOP("DBquery::exec_replace",ERR_DB_REPLACE_WITHOUT_WHERE);
		return false;
	}
	// Und ausführen
	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
	ok = query->exec(sql);
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::exec_insert",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	} else {
		field_list.clear();
		val_list.clear();
		// last_inserted++;
		last_updated++;
		where_string.clear();
		if (debug > 4) DEBUGTEXT("DB->exec: Update record %d",last_updated);
		query_id->first();
		last_id = query_id->value(0).toInt();
	}
	return ok;
}

bool DBquery::exec_update()
{
	bool ok = true;
	if (idx_field_list.size() == 0 && where_string.size() == 0) {
		ERRORPARA("DBquery::exec_update(1)",ERR_DB_UPDATE_WITHOUT_WHERE,"no index fields defined by 'prepareWhere'");
		return false;
	}

	if (!where_string.size()) assemble_where_string_from_list();

	// Query für Datensatz Update vorbereiten
	QString sql;
	sql = "UPDATE " + fromtable + " SET ";
	for (int t=0;t<field_list.size();t++) {
		if (t) sql += ",";
		if (val_list.at(t).type() == QVariant::Bool)
			sql += field_list.at(t) + " = " + QString::number(val_list.at(t).toInt());
		else
			sql += field_list.at(t) + " = '" + val_list.at(t).toString() + "'";;
	}
	sql += " WHERE " + where_string;

	if (!where_string.size()) {
		ERRORNOP("DBquery::exec_update",ERR_DB_UPDATE_WITHOUT_WHERE);
		return false;
	}
	// Und ausführen
	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
	ok = query->exec(sql);
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::exec_update",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	} else {
		field_list.clear();
		val_list.clear();
		last_updated++;
		where_string.clear();
		if (debug > 4) DEBUGTEXT("DB->exec: Update record %d",last_updated);
		if (query_id->first()) {
			last_id = query_id->value(0).toInt();
		} else {
			last_id = -1;
		}
	}
	return ok;
}

bool DBquery::exec_get()
{
	bool ok = true;
	QString sql = "SELECT ";
	for (int t=0; t<field_list.size(); t++) {
		if (t) sql += ",";
		sql += field_list.at(t);
	}
	sql += " FROM " + fromtable;
	sql += " WHERE id = ";
	sql += QString::number(where_id);
	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());

	ok = query->exec(sql);
	if (!ok) {
		QSqlError err = query->lastError();
		ERRORPARA("DBquery::exec_get",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
	} else {
		if (debug > 4) DEBUGTEXT("DB->exec: Query->size(): %d",query->size());
		if (query->size() > 0 && query->first())
			last_id = query->value(0).toInt();
		else
			last_id = 0;
	}

	return ok;
}


void DBquery::init_mode(QueryMode p_mode)
{
	// mode setzen
	mode = p_mode;
	exec_f = false;
	error = ERR_NONE;
	val_list.clear();
	field_list.clear();
	idx_val_list.clear();
	idx_field_list.clear();
	rec_valid_f = false;
	last_delete_count = 0;
	last_inserted = 0;
	last_updated = 0;
	where_string.clear();
	order_string.clear();
	last_id = 0;
	first_next_f = true;
}


/**
 * @brief DBquery::testId Prüft, ob ID aus Feld 'id' vorhanden ist
 * @param p_id Die ID auf die in der Datenbank Tabelle getestet werden soll
 * @return -1: bei Fehler; 0: ID nicht vorhanden; 1: ID vorhanden
 *
 * Es wird in der Tabelle, die durch die DBquery-Instanz zur Zeit vorgegeben wird gesucht.
 * z.B.
 * @code
 * DBquery q(database,"projects");
 * int erg = q.testId(17);
 */
int DBquery::testId(int p_id)
{
	if (fromtable.size() == 0) {
		DEBUGERROR("DBquery::testId(%d): You have to select a table first.!",p_id);
		return false;
	}
	QString sql = "SELECT id FROM " + fromtable + " WHERE id = " + QString::number(p_id);
	bool ok = query_id->exec(sql);
	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
	if (!ok) {
		QSqlError err = query_id->lastError();
		ERRORPARA("DBquery::testId",ERR_DB_QUERY_FAILED,err.text());
		set_error(sql);
		return -1;
	} else {
		if (query_id->size() > 0 && query_id->first())
			last_id = query_id->value(0).toInt();
		else
			last_id = 0;
	}
	if (debug > 4) DEBUGTEXT("Id: %llu exists %d times",p_id, query_id->size());
	return query_id->size();
}

/**
 * \brief Prüfen, wieviele Datensätze einen bestimmten Wert in einem bestimmten Feld haben
 * @param field Datensatzfeld (string)
 * @param val Der Vergleichswert
 * @return Anzahl der gefundenen Datensätze, oder -1 bei Fehler
 *
 * Diese Funktion eignet sich in erster Linie, um zu testen, ob eindeutige Datensätze existieren,
 * wie der Test auf eine bestimmte ProjektID
 */
int DBquery::testWhere(const QString & field,const QVariant & val)
{
	QString sql = "SELECT id FROM " + fromtable + " WHERE " + field + " = " + val.toString();
	bool ok = query_id->exec(sql);
	if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
	if (!ok) {
		QSqlError err = query_id->lastError();
		ERRORPARA("DBquery::testId",ERR_DB_QUERY_FAILED,err.text());
		last_id = -1;
		return -1;
	} else {
		if (query_id->size() > 0 && query_id->first())
			last_id = query_id->value(0).toInt();
		else
			last_id = 0;
	}
	if (debug > 4) DEBUGTEXT("'%s = %s' results in %d records",field.toUtf8().data(),val.toString().toUtf8().data(), query_id->size());
	return query_id->size();
}

bool DBquery::valid()
{
	if (!valid_f) return false;
	return query->isValid();
}

int DBquery::size()
{
	if (!valid_f) return 0;
	return query->size();
}

void DBquery::set_error(const QString & sql)
{
	last_id = -1;
	rec_valid_f = false;
	last_inserted = -1;
	last_updated = -1;
	last_delete_count = -1;
	if (sql.size()) {
		LOGERROR(QString("   ERROR : Last SQL query was: %1").arg(sql));
	}
}
