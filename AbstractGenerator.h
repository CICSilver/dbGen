#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include "tableDefine.h"

enum genType
{
	Define = 0,	// Define�ļ�, �������ݿ������ռ䶨�壬��Ҫ�������ɣ������ļ���Ҫ�������ļ�
	Model = 1,	// Model�ļ��� �����ඨ�壬��Ա���������캯���������������������캯������ֵ���������أ�get/set����
	Dao = 2,	// Dao�ļ�, �������ݿ��������
};

class AbstractGenerator
{
public:

	// �������ݿ����
	QString GenerateDefine(QString tableName, tbg::TableInfo tableInfo)
	{
		m_code.clear();
		QString macroName = "_DEFINE_H";
		// �궨��
		m_code.append("#ifndef " + tableName.toUpper() + macroName + "\n");
		m_code.append("#define " + tableName.toUpper() + macroName + "\n");
		// ͷ�ļ�
		m_code.append("#include <QString>\n");

		// �����ռ�
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

	// ���ֶ����ͻ�ȡ��Ӧ��Ա��������
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
		// model ��Ҫ�̳���QObject
		QString inherit = type == genType::Model ? " : QObject" : "";
		m_className = m_tableName + suffix;
		QString macroName = QString("%1_%2_H").arg(m_tableName.toUpper()).arg(suffix).toUpper();
		m_code.append("#ifndef " + macroName + "\n");
		m_code.append("#define " + macroName + "\n");

		// ���ɰ���ͷ�ļ�
		if(type == Model)
		{
			m_code.append("#include <QObject>\n");
			m_code.append("#include <QSqlQuery>\n");
			m_code.append("#include <QVariant>\n");
			m_code.append(QString("#include \"../define/%1define.h\"\n").arg(m_tableName.toLower()));
		}
		if(type == Dao)
		{
			// DAO�ļ���Ҫ����model�ļ�, SqlHelper�ļ�
			m_code.append("#include \"../model/" + m_tableName.toLower() + "model" + ".h\"\n");
			m_code.append("#include \"../SqlHelper.h\"\n");
			m_code.append(QString("using model::%1Model;\n").arg(m_tableName));
		}
		// ���������ռ�
		if (m_tableName.isEmpty() || m_tableInfo.isEmpty())
			return QString();
		m_code.append("namespace " + nameSpace + " { \n");
		TabIncrease();
		m_code.append(GetTab() + "class " + m_className + inherit + " \n");
		m_code.append(GetTab() + "{ \n");
		TabIncrease();

		if(type == Model)
		{
			// QT�����ã�ע��������
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
			GenerateStaticConstructor();	// ��
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
