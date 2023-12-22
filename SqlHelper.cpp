#include "SqlHelper.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
void SqlHelper::RunSqlScript(QString scriptPath, QString connName)
{
	QFile scriptFile(scriptPath);
	if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Could not open SQL script file: " << scriptPath;
		return;
	}
	QTextStream in(&scriptFile);
	QString sql = in.readAll();
	scriptFile.close();
	QStringList sqlStatements = sql.split(";", QString::SkipEmptyParts);
	for (const QString& statement : sqlStatements)
	{
		if (!statement.trimmed().isEmpty())
		{
			Execute(statement, connName);
		}
	}
}

void SqlHelper::Init()
{
	QString sql = "select name from sqlite_master where type='table' and name='Word'";
	if (m_connName.isEmpty())
	{
		qDebug() << "无打开的数据库连接！";
	}
	QString connName = m_connName.at(0);
	QSqlDatabase db = QSqlDatabase::database(connName);
	auto query = Execute(sql, connName);
	if (!query.next())
	{
		// 未查询到表，需要初始化
		RunSqlScript("init.sql");
		qDebug() << "Init database";
		return;
	}
}

void SqlHelper::ConnectToDB()
{
	QDir dir;
	if (m_dbPath.isEmpty())
		return;
	if (!dir.exists(m_dbPath))
	{
		dir.mkpath(m_dbPath);
	}
	//添加数据库连接
	if (!QSqlDatabase::contains(default_connName))
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", default_connName);
		db.setDatabaseName(m_dbPath);
		db.setUserName(default_username);
		db.setPassword(default_passwd);
		m_connName.push_back(default_connName);
	}
}

QSqlQuery SqlHelper::Execute(QString sql, QString connName)
{
	QSqlDatabase db = QSqlDatabase::database(connName);
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