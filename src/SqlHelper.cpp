#include "SqlHelper.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

void SqlHelper::ConnectToDB()
{
	QDir dir;
	if (m_dbPath.isEmpty())
		return;
	if (!dir.exists(m_dbPath))
	{
		dir.mkpath(m_dbPath);
	}
	QString db_conn_name = m_dbPath.split('/').last() + "_conn_" + QString::number(m_connName.size());
	m_connName.push_back(db_conn_name);
	m_curConnName = db_conn_name;
	//添加数据库连接
	if (!QSqlDatabase::contains(db_conn_name))
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", db_conn_name);
		db.setDatabaseName(m_dbPath);
		db.setUserName("");
		db.setPassword("");
		m_connName.push_back(db_conn_name);
	}
}

QSqlQuery SqlHelper::Execute(QString sql)
{
	QSqlDatabase db = QSqlDatabase::database(m_curConnName);
	QSqlQuery query(db);
	if (!db.isValid() || !db.open())
	{
		qDebug() << "Insert failed. beacuse: " << db.lastError();
	}

	if (!query.exec(sql))
	{
		QString errMsg = query.lastError().text();
		if (errMsg.contains("UNIQUE"))
		{
			// TODO: 处理重复插入错误
			//DepulicateError();
		}
	}
	return query;
}