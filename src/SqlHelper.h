#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>
#include <QMetaProperty>
#include <QDir>
#include <QDebug>

class SqlHelper
{
public:
	static SqlHelper* Instance()
	{
		static SqlHelper instance;
		return &instance;
	}

	void SwitchConn(quint8 conn_id)
	{
		if (conn_id < m_connName.size())
		{
			m_curConnName = m_connName.at(conn_id);
		}
	}
	void SetDBPath(QString dbPath) { m_dbPath = dbPath; }
	template<typename T>
	void Insert(QString tableName, const T& model, bool isAutoIncrement = true);

	template <typename T>
	void Update(QString tableName, QString _where, T& model);

	void Delete(QString tableName, QString _where)
	{
		QString sql = "DELETE FROM " + tableName + " WHERE " + _where;
		Execute(sql);
	}

	QSqlQuery Where(QString tableName, QString _where)
	{
		QString sql = "SELECT * FROM " + tableName + " WHERE " + _where;
		return Execute(sql);
	}

	void CloseDB()
	{
		QSqlDatabase db = QSqlDatabase::database(m_curConnName);
		db.close();
	}
	void ConnectToDB();
	QSqlQuery Execute(QString sql);
private:
	SqlHelper()
	{
		ConnectToDB();
	}
	~SqlHelper() {}
	QList<QString> m_connName;
	QString m_dbPath;
	QString m_curConnName;
};

template<typename T>
inline void SqlHelper::Insert(QString tableName, const T& model, bool isAutoIncrement)
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
inline void SqlHelper::Update(QString tableName, QString _where, T& model)
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
