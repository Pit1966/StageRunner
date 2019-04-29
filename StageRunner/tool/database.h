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


#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include <QList>
#include <QVariant>
#include "dbquery.h"

#define MAX_DATABASE_CONNECTIONS 4
#define MAX_ENUM_IN_PROJECT_VARS 5

class QThread;

class DBfield {
public:
	const char * com;
	const char * para;
	const char * attr;
};

class Database {
public:

	enum DB_CP_RULE_TYPE {
		CP_RULE_NONE,
		CP_RULE_PROJECT_SRC,
		CP_RULE_PROJECT_DEST,
		CP_RULE_CHILD
	};

	struct DbCopyRule {
		DB_CP_RULE_TYPE type;
		QString field;
		QString childTable;
		QString childForeignField;
		QVariant srcValue;
		QVariant destValue;
	};

	QString hostName;
	QString databaseName;
	QString userName;
	QString password;
	int error;

private:
	static Database * globalInstance;					///< Pointer auf das globale Database Objekt
	static int connection_name_idx_cnt;
	static QList<DBfield*>*table_list;					///< Liste mit allen Tabellen der Optidatenbank
	static QList<DBfield*>*dyn_table_list;				///< Liste mit allen Tabellen aus dynamischen Varsets (müssen am Ende freigegeben werden)
	static int instance_cnt;								///< Soviele Datenbankobjekte gibt es

	bool s_valid_f[MAX_DATABASE_CONNECTIONS];
	QThread * used_by_thread[MAX_DATABASE_CONNECTIONS];
	int used_cnt[MAX_DATABASE_CONNECTIONS];
	QString connection_names[MAX_DATABASE_CONNECTIONS];	///< Die Namen für eine erfolgreich aufgebaute Database Verbindung

	QMutex get_connect_mutex;

	bool is_remote_db;
	int max_connect_tries;								///< Das ist die maximale Anzahl-Versuche für den Connect

public:
	Database();
	~Database();

	static bool hasMySqlDriver();
	static Database * getGlobalInstance();

	bool set(QString p_hostName, QString p_databaseName, QString p_userName, QString p_password, bool remote = false);
	bool connectToDatabase();
	void closeConnection();
	bool createDatabase(const QString &root_passwd);
	bool checkExists();
	bool updateAllTables();
	bool checkAllTables();
	bool repairAllTables();
	bool createNewTable(DBfield *entrys);
	bool updateTable(DBfield *entrys);
	bool checkTable(DBfield *entrys);
	bool repairTable(DBfield *entrys);
	void debugPrintTableDefinition(DBfield *entrys);
	bool copyProjectToOtherDb(Database *target_db, quint64 src_projid, quint64 dest_projid = 0);
	bool copyTableToRemoteDb(Database *target_db, DBfield *entrys, QList<DbCopyRule>rules);
	void setMaxConnectionTries(int val) {max_connect_tries = val;}


	bool tableExists(QString table);
	bool isValid();
	/**
	 * @brief Wurde Datenbank auf externem Rechner geöffnet
	 * @return true, falls Datenbank nicht auf localhost liegt
	 */
	inline bool isRemoteDb() {return is_remote_db;}
	int getConnectionIndex(QThread * for_thread);
	bool freeConnectionIndex(QThread * thread);
	const QString & connectionName(uint idx);

	static void displayDatabaseNotAvailable(const QString & where);

private:
	bool copy_db_entry_childs(Database *target_db, const DbCopyRule * rule, int org_id, int target_id);
};

#endif
