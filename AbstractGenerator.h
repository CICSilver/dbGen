#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include "tableDefine.h"

enum genType
{
	Define = 0,	// Define文件, 包含数据库命名空间定义，需要优先生成，其他文件需要包含此文件
	Model = 1,	// Model文件， 包含类定义，成员变量，构造函数，析构函数，拷贝构造函数，赋值操作符重载，get/set方法
	Dao = 2,	// Dao文件, 包含数据库操作方法
};

class AbstractGenerator
{
public:

	// 生成数据库表定义
	QString GenerateDefine(QString tableName, tbg::TableInfo tableInfo)
	{
		m_code.clear();
		QString macroName = "_DEFINE_H";
		// 宏定义
		m_code.append("#ifndef " + tableName.toUpper() + macroName + "\n");
		m_code.append("#define " + tableName.toUpper() + macroName + "\n");
		// 头文件
		m_code.append("#include <QString>\n");

		// 命名空间
		m_code.append("namespace dbtable\n{\n");
		TabIncrease();
		m_code.append(GetTab() + "namespace " + tableName.toLower() + "\n");
		m_code.append(GetTab() + "{\n");
		TabIncrease();
		m_code.append(GetTab() + "constexpr auto tableName = \"" + tableName + "\";\n");
		for (auto const& fieldInfo : tableInfo)
		{
			m_code.append(GetTab() + "constexpr auto " + fieldInfo.name + " = \"" + fieldInfo.name + "\";\n");
		}
		TabDecrease();
		m_code.append(GetTab() + "}\n");
		TabDecrease();
		m_code.append("}\n");
		m_code.append("#endif \n");
		return m_code;
	}

	QString GenerateModel(QString tableName, tbg::TableInfo tableInfo)
	{
		return Generate(genType::Model, tableName, tableInfo);
	}

	QString GenerateDao(QString tableName, tbg::TableInfo tableInfo)
	{
		return Generate(genType::Dao, tableName, tableInfo);
	}


protected:
	AbstractGenerator()
	:
		m_tabCnt(0),
		m_isDefineGenerated(false)
	{}
	~AbstractGenerator() {}
	//virtual void GenerateClassDefinition() = 0;
	virtual void GenerateGetterSetters() = 0;
	virtual void GenerateConstructor() = 0;
	virtual void GenerateDestructor() = 0;
	virtual void GenerateCopyConstructor() = 0;
	virtual void GenerateOperatorEqual() = 0;
	virtual void GenerateProperties() = 0;
	virtual void GenerateMacros() = 0;

	// DAO
	virtual void GenerateStaticConstructor() = 0;
	// not used
	virtual void GenerateCreateFunc() = 0;
	// read model from database (only by primary key for now)
	virtual void GenerateReadFunc() = 0;
	// update model in database (only by primary key for now)
	virtual void GenerateUpdateFunc() = 0;
	// delete model from database (only by primary key for now)
	virtual void GenerateDeleteFunc() = 0;
	// generate private constructor/destructor for singleton, and private members
	virtual void GeneratePrivateMembers() = 0;

	// 按字段类型获取对应成员变量类型
	virtual QString GetMemberType(QString type) = 0;
	void TabIncrease() { ++m_tabCnt; }
	void TabDecrease() { if (m_tabCnt > 0) { --m_tabCnt; } }
	QString GetTab() { return QString(m_tabCnt, '\t'); }

private:
	virtual QString Generate(genType type, QString name, tbg::TableInfo tableInfo)
	{
		m_code.clear();
		m_tabCnt = 0;
		m_tableName = name;
		m_tableInfo = tableInfo;
		QString nameSpace = type == genType::Model ? "model" : "dao";
		QString suffix = type == genType::Model ? "Model" : "Dao";
		// model 需要继承自QObject
		QString inherit = type == genType::Model ? " : QObject" : "";
		m_className = m_tableName + suffix;
		QString macroName = QString("%1_%2_H").arg(m_tableName.toUpper()).arg(suffix).toUpper();
		m_code.append("#ifndef " + macroName + "\n");
		m_code.append("#define " + macroName + "\n");

		// 生成包含头文件
		if(type == Model)
		{
			m_code.append("#include <QObject>\n");
			m_code.append("#include <QSqlQuery>\n");
			m_code.append("#include <QVariant>\n");
			m_code.append(QString("#include \"../define/%1define.h\"\n").arg(m_tableName.toLower()));
		}
		if(type == Dao)
		{
			// DAO文件需要包含model文件, SqlHelper文件
			m_code.append("#include \"../model/" + m_tableName.toLower() + "model" + ".h\"\n");
			m_code.append("#include \"../SqlHelper.h\"\n");
			m_code.append(QString("using model::%1Model;\n").arg(m_tableName));
		}
		// 声明命名空间
		if (m_tableName.isEmpty() || m_tableInfo.isEmpty())
			return QString();
		m_code.append("namespace " + nameSpace + " { \n");
		TabIncrease();
		m_code.append(GetTab() + "class " + m_className + inherit + " \n");
		m_code.append(GetTab() + "{ \n");
		TabIncrease();

		if(type == Model)
		{
			// QT下启用，注册类属性
			GenerateMacros();
			TabDecrease();
			m_code.append(GetTab() + "public: \n");
			TabIncrease();
			GenerateConstructor();
			GenerateDestructor();
			GenerateCopyConstructor();
			GenerateOperatorEqual();
			GenerateGetterSetters();

			TabDecrease();
			m_code.append(GetTab() + "private: \n");
			TabIncrease();
			GenerateProperties();
		}
		else if(type == Dao)
		{
			m_code.append(GetTab() + "public: \n");
			TabIncrease();
			GenerateStaticConstructor();	// √
			GenerateReadFunc();
			GenerateDeleteFunc();
			GenerateUpdateFunc();

			GeneratePrivateMembers();
		}

		TabDecrease();
		m_code.append(GetTab() + "}; \n");
		m_code.append(QString("}; // %1 \n").arg(nameSpace));
		m_code.append("#endif");
		return m_code;
	};

protected:
	// utilities
	QString UpperFirstWord(QString str)
	{
		str[0] = str[0].toUpper();
		return str;
	}

protected:
	QString m_code;
	QString m_tableName;
	QString m_namespace;
	QString m_className;
	QMap<QString, QString> m_memberMap;	// <memberName, memberType>
	tbg::TableInfo m_tableInfo;
	int m_tabCnt;
	bool m_isDefineGenerated;
private:
	AbstractGenerator(const AbstractGenerator&) = delete;
	AbstractGenerator& operator=(const AbstractGenerator&) = delete;
};
