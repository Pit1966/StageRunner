//=======================================================================
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
//=======================================================================

#include "database.h"
#include "toolclasses.h"
#include "varset.h"
#include "log.h"

#include <string.h>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QTranslator>
#include <QProgressDialog>
#include <QApplication>
#include <QElapsedTimer>

//=========================================================================
// Database
//-------------------------------------------------------------------------

/// Statics
Database * Database::globalInstance = 0;
int Database::connection_name_idx_cnt = 0;
QList<DBfield*> *Database::table_list = 0;
QList<DBfield*> *Database::dyn_table_list = 0;

int Database::instance_cnt = 0;


DBfield projects_table[] = {
	{"_COM","_TABLENAME","projects"}, // Table Name (muss immer in die erste Zeile)
	{"id","INT","DEF AUTO_INCREMENT"},
	{"projectid","BIGINT UNSIGNED",""},
	{"locked","BOOL",""},
	{"project_version","INT","DEF"},
	{"date_creation","DATETIME","DEF"},
	{"date_modified"," TIMESTAMP","DEF DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP"},
	{"project_name","varchar(50)","DEF"},
	{"project_name_prefix","varchar(50)","DEF"},
	{"project_file_directory","varchar(512)","DEF"},
	{"project_file_host","varchar(50)","DEF"},

	{"_COM","_PRIMARY","id"},		//Primary Key
	{"_COM","_INDEX","projectid"},  //Indizierte Spalte
	{"_COM","_TABLECOMMENT","Tabelle speichert alle Projekt / Werkstuecktyp Informationen"},
	{"_COM","ENGINE","MyISAM"},

	{"_COPY_RULE_PROJECT","projectid",""},
	{"","",""},			// Als Abschlusselement
};


Database::Database()
{
	// Das erste instanzierte Datenbankobjekt initialisiert die Tabellen-liste
	// Hier müssen neue Tabellen von Hand eingetragen werden
	if (!instance_cnt) {
		table_list = new QList<DBfield*>;

		// Statische Tabellen
		table_list->append(projects_table);

		// Dynamische Tabellen aus den VarSets hinzufügen
		dyn_table_list = new QList<DBfield*>;
		QList<VarSet*>varsets = VarSet::getDatabaseTableVarLists();
		for (int t=0; t<varsets.size(); t++) {
			DBfield *table = varsets.at(t)->getDynamicDbTableDefinition();
			table_list->append(table);
			dyn_table_list->append(table);
		}

	}


	for (int t=0; t<MAX_DATABASE_CONNECTIONS; t++) {
		s_valid_f[t] = false;
		used_by_thread[t] = 0;
		used_cnt[t] = 0;
	}
	instance_cnt++;

	error = ERR_NONE;
	is_remote_db = false;
	max_connect_tries = 50;
}


Database::~Database() {
	instance_cnt--;

	if (!instance_cnt) {
		delete table_list;
		table_list = 0;
		for (int t=0; t<dyn_table_list->size(); t++) {
			VarSet::clearDbTableDefinitionQuery(dyn_table_list->at(t));
		}
		delete dyn_table_list;
		dyn_table_list = 0;
	}
}

void Database::closeConnection()
{
	for (int t=0; t<MAX_DATABASE_CONNECTIONS; t++) {
		QSqlDatabase db = QSqlDatabase::database(connection_names[t]);
		if (db.isOpen()) {
			LOGTEXT(QString("Close database connection #%1").arg(connection_names[t]));
			db.close();
		}
	}
	for (int t=0; t<MAX_DATABASE_CONNECTIONS; t++) {
		LOGTEXT(QObject::tr("Remove database connection #%1").arg(connection_names[t]));
		QSqlDatabase::removeDatabase(connection_names[t]);
		connection_names[t].clear();
		s_valid_f[t] = false;
	}
	is_remote_db = false;
}

/**
 * @brief Datenbank opti und user opti für die Datenbank anlegen
 * @param root_passwd
 * @return
 *
 * Die Funktion setzt eine laufende Datenbank voraus, die mit dem user "root" connected
 * wird
 */
bool Database::createDatabase(const QString &root_passwd)
{
	// Connection zur Datenbank öffnen
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","connectcreate");

	bool ok = db.open("root",root_passwd);

	if (ok) {
		QSqlQuery q(db);
		QString sql = QString("SHOW DATABASES LIKE '%1';").arg(databaseName);
		LOGTEXT(sql);
		ok = q.exec(sql);
		if ( ok && q.size() == 0) {
			sql = QString("CREATE DATABASE `%1` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;").arg(databaseName);
			LOGTEXT(sql);
			ok = q.exec(sql);
			if (ok) {
				sql = QString("CREATE USER '%1'@'%' IDENTIFIED BY '%2'").arg(userName,password);
				LOGTEXT(sql);
				ok = q.exec(sql);
			}
			if (ok) {
				sql = QString("GRANT USAGE ON * . * TO '%1'@'%' IDENTIFIED BY '%2'").arg(userName,password);
				LOGTEXT(sql);
				ok = q.exec(sql);
			}
			if (ok) {
				sql = QString("GRANT ALL PRIVILEGES ON `%1` . * TO '%2'@'%'").arg(databaseName,userName);
				LOGTEXT(sql);
				ok = q.exec(sql);
			}
		}
		else if (ok) {
			LOGTEXT("Database does exist already!");
		}
	}

	return ok;
}

/**
 * @brief Datenbankparameter setzen und verbinden
 * @param p_hostName Der Hostname, auf dem die Datenbank läuft (normalerweise "localhost")
 * @param p_databaseName Der Name des Schemas (normalerweise "opti")
 * @param p_userName Der User mit Zugriff auf das Schema (normalerweise "opti")
 * @param p_password Das Zugriffspasswort
 * @param remote Die Datenbank im Remote-Modus öffnen
 * @return
 */

bool Database::set(QString p_hostName, QString p_databaseName, QString p_userName, QString p_password, bool remote) {
	hostName = p_hostName;
	databaseName = p_databaseName;
	userName = p_userName;
	password = p_password;
	is_remote_db = remote;

	return connectToDatabase();
}

/**
 * @brief MySQL Treiber prüfen
 * @return true, wenn Qt-Treiber für den Zugriff auf die MySQL Datenbank vorhanden sind
 */
bool Database::hasMySqlDriver()
{
	QStringList driver = QSqlDatabase::drivers();
	bool mysql_found = false;
	for (int t=0; t<driver.size(); t++) {
		if (driver.at(t) == "QMYSQL") mysql_found = true;
	}
	return mysql_found;
}

/**
 * @brief Einen Pointer auf die globale DatabaseInstanz holen bzw. diese erzeugen, falls noch nicht geschehen
 * @return
 */
Database *Database::getGlobalInstance()
{
	if (!Database::globalInstance) {
		Database::globalInstance = new Database;
	}
	return Database::globalInstance;
}


bool Database::connectToDatabase() {
	QSqlError err;

	for (int t=0; t<MAX_DATABASE_CONNECTIONS; t++) {
		if (s_valid_f[t]) {
			DEBUGTEXT("Database::connectToDatabase(): Reconnect connection #%s -> cancel old connection and try to establish a new"
					  ,connection_names[t].toLatin1().data());
			QSqlDatabase::database(connection_names[t]).close();
			QSqlDatabase::removeDatabase(connection_names[t]);
			connection_names[t].clear();
			s_valid_f[t] = false;
		}
	}

	// Erste Datenbank schon mal initialisieren
	bool open_ok = false;
	QString con_name = QString("connect%1").arg(connection_name_idx_cnt++);
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL",con_name);
		db.setHostName(hostName);
		db.setDatabaseName(databaseName);
		db.setUserName(userName);
		db.setPassword(password);
		db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

		int max_trys = max_connect_tries;
		bool cancel = false;

		QProgressDialog progress(QObject::tr("Connect to database ..."), QObject::tr("Cancel"), 1, max_trys, 0);
		progress.setWindowModality(Qt::WindowModal);
		int loop = 0;
		while (!open_ok && !cancel && ++loop <= max_trys) {
			if (debug > 3) LOGTEXT(QString("Try to connect to database '%1:%2' as user '%3' connection:%4")
					.arg(hostName).arg(databaseName).arg(userName).arg(con_name));
			progress.setValue(loop);
			open_ok = db.open();
			if (!open_ok) {
				err = db.lastError();
				progress.setLabelText(QObject::tr("Connect to database ...\n\n%1").arg(err.text()));
				QElapsedTimer wait;
				wait.start();
				while (wait.elapsed() < 500 && !cancel) {
					if (logThread->isMainThread()) QApplication::processEvents();
					if (progress.wasCanceled()) cancel = true;
				};
			}
		}
	}
	if (open_ok) {
		connection_names[0] = con_name;

		LOGTEXT(QString("Connected to MySQL database '%1:%2' as user '%3' connection:%4")
				.arg(hostName).arg(databaseName).arg(userName).arg(con_name));
		if (debug)
			LOGTEXT(QObject::tr("Found %1 tables in database table list").arg(table_list->size()-dyn_table_list->size()));
		if (debug)
			LOGTEXT(QObject::tr("Found %1 dynamic tables in VarSets").arg(dyn_table_list->size()));

		s_valid_f[0] = true;
		error = ERR_NONE;
		// die anderen Verbindungen öffnen
		for (int t=1;t<MAX_DATABASE_CONNECTIONS; t++) {
			con_name = QString("connect%1").arg(connection_name_idx_cnt++);
			QSqlDatabase db = QSqlDatabase::cloneDatabase(QSqlDatabase::database(connection_names[0]), con_name);
			if (db.open()) {
				s_valid_f[t] = true;
				connection_names[t] = con_name;
				if (debug > 1)
					LOGTEXT(QString("Additional connection %4 to MySQL database '%1:%2' as user '%3' connection:%5")
							.arg(hostName).arg(databaseName).arg(userName).arg(t).arg(con_name));
			} else {
				LOGERROR(QString("Additional connection %4 to MySQL database '%1:%2' as user '%3' failed").arg(hostName).arg(databaseName).arg(userName).arg(t));
				s_valid_f[t] = false;
			}
		}
		return true;
	} else {
		QMessageBox::critical(0, QObject::tr("Database Error"),
							  QObject::tr("Could not establish connection to database!\n\n%1").arg(err.text()));
		ERRORNOP("Database::connectToDatabase",ERR_DB_OPEN_FAILED);
		LOGERROR(QString("Connection to MySQL database '%1:%2' as user '%3' failed! Error: '%4'").arg(hostName).arg(databaseName).arg(userName).arg(err.text()));
		s_valid_f[0] = false;
		return false;
	}
}

/**
 * @brief Prüfen, ob Tabelle in Datenbankschema existiert
 * @param table Name der Tabelle
 * @return true, falls Tabelle bereits angelegt wurde
 */
bool Database::tableExists(QString table) {
	if (!isValid()) return false;

	QSqlDatabase db = QSqlDatabase::database(connection_names[0]);

	QString sql = "SHOW TABLES LIKE '"+table+"';";
	QSqlQuery query(db);
	query.prepare(sql);
	query.exec();
	if (query.size()>0) return true;
	else return false;
}

/**
 * @brief Alle benötigten Datenbanktabellen überprüfen und anlegen
 * @return true, falls fehlerfrei
 */
bool Database::checkExists() {
	if (!isValid()) return false;
	// Sind alle Tables angelegt?

	bool ok = true;

	int num = 0;
	while (num <table_list->size() && ok) {
		DBfield *table = table_list->at(num);
		if (!tableExists(table[0].attr)) {
			ok = createNewTable(table);
		}
		num++;
	}

	return ok;
}

bool Database::updateAllTables() {
	if (!isValid()) return false;
	// Sind alle Tables angelegt?

	bool ok = true;

	int num = 0;
	while (num <table_list->size() && ok) {
		DBfield *table = table_list->at(num);
		if (tableExists(table[0].attr)) {
			ok = updateTable(table);
		}
		num++;
	}

	return ok;
}

bool Database::checkAllTables()
{
	if (!isValid()) return false;
	// Sind alle Tables ok?


	bool ok = true;

	int num = 0;
	while (num <table_list->size()) {
		DBfield *table = table_list->at(num);
		ok &= checkTable(table);
		num++;
	}

	return ok;
}

bool Database::repairAllTables()
{
	if (!isValid()) return false;


	bool ok = true;
	int num = 0;
	while (num <table_list->size()) {
		DBfield *table = table_list->at(num);
		ok &= repairTable(table);
		num++;
	}

	return ok;
}

bool Database::createNewTable(DBfield *entrys) {
	if (!isValid()) return false;
	// Default Parameter
	QString def = " NOT NULL ";
	QString def_str = " ";
	QString engine = "MyISAM";

	bool ok = true;

	// Scan Tabelle nach Feldnamen, Tabellenname usw.
	QString tablename;
	QList<DBfield>postcom;
	QList<DBfield>keys;
	QList<DBfield>trigger;

	int idx = 0;
	while (strlen(entrys[idx].com) > 0) {
		QString com = entrys[idx].com;
		QString para = entrys[idx].para;
		QString attr = entrys[idx].attr;
		if (com == "_COM") {
			if (para == "_TABLENAME")
				tablename = attr;
			else if (para == "_PRIMARY")
				keys.append(entrys[idx]);
			else if (para == "_INDEX" || para == "_KEY" || para.left(5) == "INDEX")
				keys.append(entrys[idx]);
			else if (para == "_TABLECOMMENT")
				postcom.append(entrys[idx]);
			else if (para == "_UPDATETRIGGER" || para == "_INSERTTRIGGER" || para == "_DELETETRIGGER")
				trigger.append(entrys[idx]);
			else if (para == "ENGINE" || para == "_ENGINE")
				engine = attr;

		}
		else if (com.startsWith("_")) {
			// Ignorieren
		}
		idx++;
	}
	if (!tablename.size()) {
		ERRORPARA("Database::createNewTable",ERR_DB_CREATE_TABLE_FAILED,"tablename missing");
		return false;
	}
	if (!keys.size()) {
		ERRORPARA("Database::createNewTable",ERR_DB_CREATE_TABLE_FAILED,"primary key missing");
		return false;
	}

	// Tabelle erzeugen
	//   Datenbank und Tabellenname
	QString sql = "CREATE TABLE IF NOT EXISTS " + databaseName + "." + tablename + " (";
	//   Die einzelnen Spalten
	idx = 0;
	while (strlen(entrys[idx].com) > 0) {
		QString com = entrys[idx].com;
		QString para = entrys[idx].para;
		QString attr = entrys[idx].attr;
		if (! com.startsWith("_")) {
			// Spaltenname + Type
			sql += com + " " + para;
			// Spalten Optionen
			if (attr.size() == 0) {
				// Default Attribute / Extra hinzufügen
				if (para.toLower().contains("varchar")) {
					sql += def_str;
				} else {
					sql += def;
				}
			}
			else if (attr.toLower().left(3) == "def" && (attr.size() == 3 || attr.mid(3,1) == " ")) {
				// Default Attribute / Extra hinzufügen
				if (para.toLower().contains("varchar")) {
					sql += def_str;
				} else {
					sql += def;
				}
				attr.remove(0,3);
			}
			if (attr.size() > 1) sql += attr;
			// Spalte fertig
			sql += ",";
		}
			idx++;
	}

	// Keys anhängen
	for (int t=0; t<keys.size(); t++) {
		// QString com = keys[t].com;
		QString para = keys[t].para;
		QString attr = keys[t].attr;
		if (para == "_PRIMARY")
			sql += " PRIMARY KEY (" + attr + ")";
		else if (para == "_INDEX")
			sql += " INDEX (" + attr + ")";
		else if (para == "_KEY")
			sql += " KEY (" + attr + ")";
		else if (para.left(5) == "INDEX")
			sql += para + " (" + attr + ")";


		if (t+1 < keys.size()) sql += ",";
	}
	// Spalten abschließen
	sql += ")";

	// Weitere Befehle anhängen
	for (int t=0; t<postcom.size(); t++) {
		// QString com = postcom[t].com;
		QString para = postcom[t].para;
		QString attr = postcom[t].attr;
		if (para == "_TABLECOMMENT")
			sql += " COMMENT = '" + attr + "'";
		else
			sql += " " + para + " = " + attr;
	}
	sql += " ENGINE = " + engine;

	sql += " CHARACTER SET utf8 COLLATE utf8_general_ci;";

	QSqlDatabase db = QSqlDatabase::database(connection_names[0]);
	QSqlQuery query(db);

	if (debug > 4) LOGTEXT(QString("DB->exec: <font color=brown>%1</font>").arg(sql));
	ok = query.exec(sql);
	if (!ok) {
		QSqlError err = query.lastError();
		ERRORPARA("Database::createNewTable",ERR_DB_CREATE_TABLE_FAILED,tablename + ":" + err.text());
	}
	if (ok && trigger.size()) {
		for (int t=0; t<trigger.size(); t++) {
			sql.clear();
			QString para = trigger[t].para;
			if (para == "_UPDATETRIGGER") {
				sql = "CREATE TRIGGER trigger_" + tablename + QString::number(t) + " BEFORE UPDATE ON " + tablename;
				sql += " FOR EACH ROW " + QString(trigger[t].attr) + ";";
			}
			else if (para == "_INSERTTRIGGER") {
				sql = "CREATE TRIGGER trigger_" + tablename + QString::number(t) + " BEFORE INSERT ON " + tablename;
				sql += " FOR EACH ROW " + QString(trigger[t].attr) + ";";
			}
			else if (para == "_DELETETRIGGER") {
				sql = "CREATE TRIGGER trigger_" + tablename + QString::number(t) + " BEFORE DELETE ON " + tablename;
				sql += " FOR EACH ROW " + QString(trigger[t].attr) + ";";
			}
			else {
				ERRORPARA("Database::createTable (trigger1)",ERR_DB_CREATE_TABLE_FAILED,"trigger unknown");
				ok = false;
			}

			if (sql.size()) {
				if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());
				ok = query.exec(sql);
				if (!ok) {
					QSqlError err = query.lastError();
					ERRORPARA("Database::createNewTable (trigger2)",ERR_DB_CREATE_TABLE_FAILED,tablename + ":" + err.text());
					sql = "DROP TABLE " + tablename;
					query.exec(sql);

					break;
				}
			}
		}
	}
	if (ok)
		LOGTEXT(QString("MySQL table '%1' created in '%2'").arg(tablename).arg(databaseName));
	return ok;
}

bool Database::updateTable(DBfield *entrys) {
	if (!isValid()) return false;

	// Default Parameter
	QString def = "  NOT NULL ";
	QString def_str = " ";

	bool ok = true;

	// Tabelle
	QString tablename;
	QList<DBfield>cols;
	QList<int>addflags;
	QStringList index_cols;

	// Tabellendaten vorbereiten
	int idx = 0;
	while (strlen(entrys[idx].com) > 0) {
		QString com = entrys[idx].com;
		QString para = entrys[idx].para;
		QString attr = entrys[idx].attr;
		if (com == "_COM") {
			if (para == "_TABLENAME") {
				tablename = attr;
			}
			else if (para == "_INDEX" || para == "_PRIMARY") {
				index_cols.append(attr);
			}
		}
		else if (com.startsWith("_")) {
			// Ignorieren
		}
		else {
			cols.append(entrys[idx]);
			addflags.append(0);
		}
		idx++;
	}
	if (!tablename.size()) {
		ERRORPARA("Database::updateTable",ERR_DB_CREATE_TABLE_FAILED,"tablename missing");
		return false;
	}

	// Alle Spalten der Tabelle in Datenbank selektieren
	int not_present_cnt = 0;
	QSqlDatabase db = QSqlDatabase::database(connection_names[0]);
	QSqlQuery qq(db);
	QString sql = "SELECT * FROM " + tablename;
	ok = qq.exec(sql);
	if (!ok) {
		ERRORNOP("Database::updateTable",ERR_DB_UPDATE_TABLE_FAILED);
	} else {
		LOGTEXT(QTranslator::tr("Begin update of database table '%1' which yet has %2 records").arg(tablename).arg(qq.size()));

		// Jetzt prüfen, ob neue Spalten dazugekommen sind
		for (int col=0; col<cols.size(); col++) {
			// IndexNummer des Spaltennamens ermittlen. Falls nicht existent, wird hier -1 zurückgegeben
			QString field = cols[col].com;
			int col_no = qq.record().indexOf(field);
			if (col_no < 0) {
				LOGERROR(QTranslator::tr("  Column '%1' not available in table").arg(field));
				addflags[col] = 1;
				not_present_cnt++;
			} else {
				if (debug > 4) {
					DEBUGTEXT("  Column '%s' exists",field.toUtf8().data());
				}
			}
		}
	}

	if (not_present_cnt) {
		int insert_cnt = 0;
		int col = 0;
		while (col<cols.size() && ok) {
			QString com = cols[col].com;
			QString para  = cols[col].para;
			QString attr = cols[col].attr;

			if (addflags[col] == 1) {
				// SQL Kommando zusammenbauen
				// TYPE und FLAGS
				QString type = para;
				// Spalten Optionen
				if (attr.size() == 0) {
					// Default Attribute / Extra hinzufügen
					if (para.toLower().contains("varchar")) {
						type += def_str;
					} else {
						type += def;
					}
				}
				else if (attr.toLower().left(3) == "def" && (attr.size() == 3 || attr.mid(3,1) == " ")) {
					// Default Attribute / Extra hinzufügen
					if (para.toLower().contains("varchar")) {
						type += def_str;
					} else {
						type += def;
					}
					attr.remove(0,3);
				}
				if (attr.size() > 1) type += attr;

				// Wo einfügen?
				QString wheretoins;
				if (col == 0) {
					// Am Anfang
					wheretoins = " FIRST";
				} else {
					// Hinter dem vorhergehendem einfügen
					wheretoins = " AFTER `" + QString(cols[col-1].com) + "`";
				}
				LOGTEXT(QTranslator::tr("    Insert database table '%1' of type '%2' %3").arg(com).arg(type).arg(wheretoins));

				QString sql = "ALTER TABLE `" + tablename + "` ADD `" + com + "` " + type + " " + wheretoins;
				if (debug > 4) DEBUGTEXT("DB->exec: %s",sql.toUtf8().data());

				ok = qq.exec(sql);
				if (!ok) {
					QSqlError err = qq.lastError();
					ERRORPARA("DBquery::exec_select",ERR_DB_QUERY_FAILED,err.text());
				} else {
					insert_cnt++;
				}
			}
			col++;
		}
		LOGTEXT(QTranslator::tr("  <font color=darkblue>%1 new fields inserted in table</font> '%2'").arg(insert_cnt).arg(tablename));
	} else {
		LOGTEXT(QTranslator::tr("  Table '%1' <font color=darkgreen>is up to date</font>").arg(tablename));
	}

	// Index Spalten prüfen
	if (ok) {
		QStringList key_cols;
		QStringList key_names;
		sql = "SHOW INDEX FROM " + tablename;
		ok = qq.exec(sql);

		// Index Tabellen auslesen
		while (qq.next() && ok) {
			QString column_name = qq.value(4).toString();
			QString key_name = qq.value(2).toString();
			int idx = key_cols.indexOf(key_name);
			if (idx >= 0) {
				key_names[idx] += ',';
				key_names[idx] += column_name;
			} else {
				key_cols.append(column_name);
				key_names.append(column_name);
			}

		}

		if (ok) {
			// Jetzt schmeißen wir alle Indexspalten wieder aus 'index_cols' Liste, die in der Datenbank gefunden wurden.
			for (int t=0; t<key_names.size(); t++) {
				int idx = index_cols.indexOf(key_names.at(t));
				if (idx >= 0) {
					// gefunden -> super
					if (debug > 1) {
						LOGTEXT(QString("   Index column: '%1' ok!").arg(key_names.at(t)));
					}
					// und austragen
					index_cols.removeAt(idx);
				} else {
					LOGERROR(QString("   Index column: '%1' is not an index in default table layout in 'database.cpp'")
							 .arg(key_names.at(t)));
					sql = "ALTER TABLE `" + tablename + "` DROP INDEX `" + key_cols.at(t) + "`";
					qDebug() << sql;
					ok = qq.exec(sql);
					if (!ok) {
						QSqlError err = qq.lastError();
						ERRORPARA("DBquery::exec_select",ERR_DB_QUERY_FAILED,err.text());
						break;
					} else {
						LOGTEXT("     <font color=darkgreen>Removed!</font>");
					}

				}
			}

			// In index_cols stehen jetzt alle Spalten, die als Indexspalte vorgesehen sind, in der Datenbank diesen Status aber nicht haben
			for (int t=0; t<index_cols.size(); t++) {
				LOGERROR(QString("    Column %1 should be index").arg(index_cols.at(t)));
				sql = "ALTER TABLE `" + tablename + "` ADD INDEX (" + index_cols.at(t) + ")";
				qDebug() << sql;
				ok = qq.exec(sql);
				if (!ok) {
					QSqlError err = qq.lastError();
					ERRORPARA("DBquery::exec_select",ERR_DB_QUERY_FAILED,err.text());
					break;
				} else {
					LOGTEXT("     <font color=darkgreen>Created!</font>");
				}
			}
		}
	}

	return ok;
}

bool Database::checkTable(DBfield *entrys)
{
	if (!isValid()) return false;

	QString tablename = entrys[0].attr;

	QSqlDatabase db = QSqlDatabase::database(connection_names[0]);
	QSqlQuery q(db);

	QString sql = "CHECK TABLE " + tablename;

	bool ok = q.exec(sql);

	if (!ok) {
		LOGERROR(QTranslator::tr("Check database table '%1': %2").arg(tablename).arg(q.lastError().text()));
	} else {
		bool is_ok = false;
		while (q.next()) {
			QString msg_type = q.value(2).toString();
			QString msg_text = q.value(3).toString();
			if (debug > 1) LOGTEXT(QString("Table:%1 '%2' '%3'").arg(tablename).arg(msg_type).arg(msg_text));
			if (msg_type.toLower().contains("status") && msg_text.toLower() == "ok") {
				LOGTEXT(QTranslator::tr("Check database table '%1': Status: %2").arg(tablename).arg(q.value(3).toString()));
				is_ok = true;
			} else {
				// Tabelle ist scheinbar nicht in Ordnung
				LOGERROR(QTranslator::tr("Bad check database table '%1': Status: %2").arg(tablename).arg(q.value(3).toString()));
			}
			ok = is_ok;
		}
	}


	return ok;
}

bool Database::repairTable(DBfield *entrys)
{
	if (!isValid()) return false;

	QString tablename = entrys[0].attr;

	QSqlDatabase db = QSqlDatabase::database(connection_names[0]);
	QSqlQuery q(db);

	QString sql = "REPAIR TABLE " + tablename;

	bool ok = q.exec(sql);

	if (!ok) {
		LOGERROR(QTranslator::tr("Repair database table '%1': %2").arg(tablename).arg(q.lastError().text()));
	} else {
		while (q.next()) {
			QString msg_type = q.value(2).toString();
			QString msg_text = q.value(3).toString();
			if (msg_type.toLower() == "status" && msg_text.toLower() != "ok") {
				// Tabelle ist scheinbar nicht in Ordnung
				LOGERROR(QTranslator::tr("Repair database table '%1': Status: %2").arg(tablename).arg(q.value(3).toString()));
				ok = false;
			} else {
				LOGTEXT(QTranslator::tr("<font color=darkgreen>Repair database table '%1': Status: %2</font>").arg(tablename).arg(q.value(3).toString()));
			}

		}
	}


	return ok;
}

void Database::debugPrintTableDefinition(DBfield *entrys)
{
	// Tabellendaten ausgeben
	int idx = 0;
// 	int col_cnt = 0;
	while (strlen(entrys[idx].com) > 0) {
		QString com = entrys[idx].com;
		QString para = entrys[idx].para;
		QString attr = entrys[idx].attr;
		qDebug() << com << para << attr;
		idx++;
	}
}

bool Database::copyProjectToOtherDb(Database *target_db, quint64 src_projid, quint64 dest_projid)
{
	if (!dest_projid) dest_projid = src_projid;

	bool ok = true;


	int t = 0;
	while (t < table_list->size() && ok) {
		DBfield *table = table_list->at(t);
		QList<Database::DbCopyRule>rules;
		// Zunächst die Kopierregeln aus der Tabellenbeschreibung auslesen
		int line = 0;
		while (strlen(table[line].com) > 0) {
			if (strcmp("_COPY_RULE_PROJECT",table[line].com) == 0) {
				Database::DbCopyRule r;
				r.type = Database::CP_RULE_PROJECT_SRC;
				r.field = table[line].para;
				r.srcValue = src_projid;
				r.destValue = dest_projid;
				rules.append(r);
			}
			else if (strcmp("_COPY_RULE_PROJECT_DEST",table[line].com) == 0) {
				Database::DbCopyRule r;
				r.type = Database::CP_RULE_PROJECT_DEST;
				r.field = table[line].para;
				rules.append(r);
			}
			else if (strcmp("_COPY_RULE_CHILD",table[line].com) == 0) {
				Database::DbCopyRule r;
				r.type = Database::CP_RULE_CHILD;
				r.field = table[0].attr;
				r.childTable = table[line].para;
				r.childForeignField = table[line].attr;
				rules.append(r);
			}

			line++;
		}

		// Jetzt den Kopiervorgang starten mit den gefundenen Kopierregeln
		if (rules.size()) {
			copyTableToRemoteDb(target_db, table, rules);
		}
		t++;
	}
	return ok;
}

bool Database::copyTableToRemoteDb(Database *target_db, DBfield *entrys, QList<Database::DbCopyRule>rules)
{
	if (target_db == this) return false;

	if (debug) {
		qDebug() << "Copy from table:" << entrys->attr;
		for (int t=0; t<rules.size(); t++) {
			qDebug() << "  Field:" << rules.at(t).field << rules.at(t).srcValue << rules.at(t).destValue;
		}
	}

	QString table = entrys[0].attr;

	// Hier kommen die Regeln rein, die das kopieren der Child Objekte übernehmen
	QList<const Database::DbCopyRule *>child_rules;

	int entries_read = 0;
	int entries_written = 0;

	// Das AbfrageObjekt für die Quelldatenbank
	DBquery qsrc(this, DBquery::SELECT, table);
	// Aus den CopyRules die Where-Bedingungen erstellen
	for (int t=0; t<rules.size(); t++) {
		const Database::DbCopyRule & r = rules.at(t);
		switch (r.type) {
		case Database::CP_RULE_PROJECT_SRC:
			qsrc.prepareWhere(r.field + " = ", r.srcValue);
			break;
		case Database::CP_RULE_CHILD:
			child_rules.append(&rules.at(t));
			break;
		default:
			break;
		}
	}
	bool ok = qsrc.execPrepared();

	while (qsrc.nextRecord() && ok) {
		entries_read++;
		qDebug() << table << "entries read:" << entries_read;

		DBquery qdest(target_db, DBquery::REPLACE, table);
		QStringList wherefields;
		// Aus den CopyRules die Destination Where-Bedingungen erstellen
		for (int t=0; t<rules.size(); t++) {
			const Database::DbCopyRule & r = rules.at(t);
			switch (r.type) {
			case Database::CP_RULE_PROJECT_SRC:
				qdest.prepareWhere(r.field + " = ", r.destValue);
				wherefields.append(r.field);
				break;
			case Database::CP_RULE_PROJECT_DEST:
				{
					QVariant val;
					qsrc.getValue(r.field,val);
					qdest.prepareWhere(r.field + " = ", val);
					wherefields.append(r.field);
				}
				break;
			default:
				break;
			}

		}

		// Quelldatenbankeintrag auslesen
		int line = 1;
		while (strlen(entrys[line].com) > 0 && ok) {
			QString field = entrys[line].com;

			if (!field.startsWith("_") && field != "id" && !wherefields.contains(field)) {
				QVariant val;
				ok &= qsrc.getValue(field,val);
				// qDebug() << field << val;
				ok &= qdest.prepareValue(field,val);
			}
			line++;
		}

		// Eintrag in Zieldatenbank schreiben
		if (ok && qdest.execPrepared()) {
			entries_written++;
			qDebug() << table << "entries written:" << entries_written;
		} else {
			ok = false;
		}

		// Child Einträge kopieren
		if (ok) {
			for (int t=0; t<child_rules.size(); t++) {
				ok = copy_db_entry_childs(target_db, child_rules.at(t), qsrc.lastID(), qdest.lastID());
			}

		}
	}

	return ok;
}



void Database::displayDatabaseNotAvailable(const QString & where)
{
	QString msg = QObject::tr("Connection to database not available.\n\nAction aborted.\n\nPlease check the configuration of your database settings");
	POPUPERRORMSG( where,msg );
}

bool Database::copy_db_entry_childs(Database *target_db, const Database::DbCopyRule *rule, int org_id, int target_id)
{
	if (debug > 0) {
		qDebug() << "Child_Rule  Parent table:" << rule->field
				 << "Child table:" << rule->childTable
				 << "Child field:" << rule->childForeignField
				 << "Parent Id:" << org_id;

	}

	bool found = false;
	int idx = 0;
	while (idx < Database::table_list->size() && !found) {
		if (strcmp(Database::table_list->at(idx)->attr, rule->childTable.toLatin1().data()) == 0) {
			found = true;
		} else {
			idx++;
		}
	}
	if (!found) {
		DEBUGERROR("Table %s not found",rule->childTable.toLatin1().data());
		return false;
	}
	DBfield * entrys = Database::table_list->at(idx);

	bool ok = true;
	int childs_read = 0;
	int childs_written = 0;
	QVariant target_foreign_id = target_id;

	// Records löschen, die die Target_Id im foreign field bereits eingetragen haben
	DBquery d(target_db, DBquery::DELETE, rule->childTable);
	d.prepareWhere(rule->childForeignField + " = ", target_id);
	ok = d.execPrepared();
	if (ok) {
		if (d.lastDeleteCount()) {
			if (debug > 0) qDebug() << "   Target Table:" << rule->childTable << " childs deleted:" << d.lastDeleteCount();
		}
	}

	// Query absetzen, das Records mit der foreignId Id aus der angebenen Tabelle sucht
	DBquery q(this, DBquery::SELECT, rule->childTable);
	q.prepareWhere(rule->childForeignField + " = ", org_id);

	ok = q.execPrepared();
	while (ok && q.nextRecord()) {
		childs_read++;
		if (debug > 0) qDebug() << "   Org Table:" << rule->childTable << " childs read:" << childs_read;

		// Query für Zieldatenbank
		DBquery qd(target_db, DBquery::INSERT, rule->childTable);

		// Quelldatenbankeintrag auslesen und Daten in Zieldatenbankquery schreiben
		int line = 1;
		while (strlen(entrys[line].com) > 0 && ok) {
			QString field = entrys[line].com;

			if (!field.startsWith("_") && field != "id" /*&& !wherefields.contains(field)*/) {
				QVariant val;
				if (field == rule->childForeignField) {
					ok = qd.prepareValue(field, target_foreign_id);
				} else {
					ok &= q.getValue(field,val);
					ok &= qd.prepareValue(field,val);
				}
			}

			line++;
		}

		// Eintrag in Zieldatenbank schreiben
		if (ok && qd.execPrepared()) {
			childs_written++;
			if (debug > 0) qDebug() << "   Target Table:" << rule->childTable << " childs written:" << childs_written;
		} else {
			ok = false;
		}


	}

	return ok;
}

bool Database::isValid()
{
	bool valid = true;
	for (int t=0; t<MAX_DATABASE_CONNECTIONS; t++) {
		valid &= s_valid_f[t];
	}
	return valid;
}

int Database::getConnectionIndex(QThread * for_thread)
{
	bool found = false;
	int loop = 0;
	int con = 0;

	while (!found && ++loop <50000000) {

		get_connect_mutex.lock();

		// Zunächst suchen, ob vom anfordernden Thread schon DBqueries existieren
		con = 0;
		while (con < MAX_DATABASE_CONNECTIONS && !found) {
			if (for_thread == used_by_thread[con]) {
				found = true;
			} else {
				con++;
			}
		}

		if (found) {
			// Zähler erhöhen
			used_cnt[con]++;
			if (debug > 5) DEVELTEXT("Database::getConnectionIndex: DBquery use count for thread '%s' is %d",for_thread->objectName().toLatin1().data(),used_cnt[con]);
		} else {
			// Thread ist nicht registriert
			// Also eine freie Verbindung suchen
			con = 0;
			while (con < MAX_DATABASE_CONNECTIONS && !found) {
				if ( 0 == used_by_thread[con]) {
					found = true;
					used_by_thread[con] = for_thread;
					used_cnt[con] = 1;
					if (debug > 4) DEVELTEXT("Database::getConnectionIndex: Registered DBquery for thread '%s'",for_thread->objectName().toLatin1().data());
				} else {
					con++;
				}
			}
		}

		get_connect_mutex.unlock();

		if (!found) {
			if (1 == loop) {
				DEVELTEXT("Database::getConnectionIndex: No free connection found -> wait");
			}
			con = -1;
		}
	}
	if (!found) {
		DEBUGERROR("Database::getConnectionIndex: No free connection found!");
	}
	if(loop > 3)
	{
		DEVELTEXT("Database::getConnectionIndex: %d iterations", loop);
	}
	return con;
}

bool Database::freeConnectionIndex(QThread * thread)
{
	// Nach Thread Registrierung suchen
	int con = 0;
	bool found = false;

	get_connect_mutex.lock();

	while (!found && con < MAX_DATABASE_CONNECTIONS) {
		if (thread == used_by_thread[con]) {
			found = true;
		} else {
			con++;
		}
	}

	if (found) {
		// Thread Eintrag gefunden
		// herunterzählen und sehen, ob das der letzte war
		used_cnt[con]--;
		if (used_cnt[con] == 0) {
			used_by_thread[con] = 0;
			if (debug > 5) DEVELTEXT("Database::freeConnectionIndex: Freed last DBquery for thread '%s'",thread->objectName().toLatin1().data());

		}
		else if (used_cnt[con] < 0) {
			DEBUGERROR("Database::freeConnectionIndex: Thread use count < 0 for thread '%s' -> Clear",thread->objectName().toLatin1().data());
			used_cnt[con] = 0;
		}
		else {
			if (debug > 5) DEVELTEXT("Database::freeConnectionIndex: DBquery use count for thread '%s' is %d",thread->objectName().toLatin1().data(),used_cnt[con]);
		}
	} else {
		// Thread Eintrag wurde nicht gefunden
		// Das darf nicht passieren
		DEBUGERROR("Database::freeConnectionIndex: Thread '%s' not registered",thread->objectName().toLatin1().data());
	}


	get_connect_mutex.unlock();

	return found;
}

const QString & Database::connectionName(uint idx)
{
	if (idx < MAX_DATABASE_CONNECTIONS) {
		return connection_names[idx];
	} else {
		return connection_names[0];
	}
}
