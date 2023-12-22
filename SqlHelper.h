#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>
#include <QMetaProperty>
#include <QDir>
#include <QDebug>

#define default_dbName "noteTool.db"
#define default_connName "note_database_conn"
#define default_username "xyr"
#define default_passwd "123456"

class SqlHelper
{
public:
	static SqlHelper* Instance()
	{
		static SqlHelper instance;
		return &instance;
	}
	void RunSqlScript(QString scriptPath = "init.sql", QString connName = default_connName);

	/// <summary>
	/// 初始化数据库的表结构
	/// </summary>
	void Init();
	void SetDBPath(QString dbPath) { m_dbPath = dbPath; }
	template<typename T>
	void Insert(QString tableName, const T& model, QString connName = default_connName, bool isAutoIncrement = true);

	template <typename T>
	void Update(QString tableName, QString _where, T& model, QString connName = default_connName);

	void Delete(QString tableName, QString _where, QString connName = default_connName)
	{
		QString sql = "DELETE FROM " + tableName + " WHERE " + _where;
		Execute(sql, connName);
	}

	QSqlQuery Where(QString tableName, QString _where, QString connName = default_connName)
	{
		QString sql = "SELECT * FROM " + tableName + " WHERE " + _where;
		return Execute(sql, connName);
	}

	void CloseDB(QString connName = default_connName)
	{
		QSqlDatabase db = QSqlDatabase::database(connName);
		db.close();
	}
	void ConnectToDB();
	QSqlQuery Execute(QString sql, QString connName = default_connName);
private:
	SqlHelper()
	{
		ConnectToDB();
	}
	~SqlHelper() {}
	QList<QString> m_connName;
	QString m_dbPath;
};

template<typename T>
inline void SqlHelper::Insert(QString tableName, const T& model, QString connName, bool isAutoIncrement)
{
	const QMetaObject* metaobject = model.metaObject();
	QString sql = "INSERT INTO " + tableName + " (";
	QString values = "VALUES (";
	int offset = QObject::staticMetaObject.propertyCount();
	int count = metaobject->propertyCount();
	for (int i = offset; i < count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char* name = metaproperty.name();
		if (isAutoIncrement && QString(name).toLower() == "id")
		{
			// 跳过自增id
			continue;
		}
		sql += name;
		if (i != count - 1)
		{
			sql += ",";
		}

		QVariant value = model.property(name);
		values += "'" + value.toString() + "'";
		if (i != count - 1)
		{
			values += ",";
		}
	}
	sql += ") " + values + ")";


	Execute(sql);
}

template<typename T>
inline void SqlHelper::Update(QString tableName, QString _where, T& model, QString connName)
{
	const QMetaObject* metaobject = model.metaObject();
	QString sql = "UPDATE " + tableName;
	QString set = " SET ";
	QString where = " WHERE " + _where;
	int offset = QObject::staticMetaObject.propertyCount();
	int count = metaobject->propertyCount();
	for (int i = offset; i < count; ++i)
	{
		QMetaProperty property = metaobject->property(i);
		const char* name = property.name();

	}
}
