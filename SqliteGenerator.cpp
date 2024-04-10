#include "SqliteGenerator.h"
#include <QMessageBox>
#include <QMap>
#include <QDebug>

void qgen::SqliteGenerator::GenerateGetterSetters()
{
    for (auto it = m_memberMap.constBegin(); it != m_memberMap.constEnd(); ++it)
    {
        // upper first letter
        // m_.... -> ....
        QStringList list = it.key().split('_');
        QString funcName;
        for(int i = 1;i<list.size();++i)
        {
            QString tmp = list.at(i);
            tmp = tmp.at(0).toUpper() + tmp.mid(1).toLower();
            funcName += tmp;
        }
        //funcName = funcName.at(0).toUpper() + funcName.mid(1).toLower();
        QString memberName = it.key();
        QString memberType = it.value();
        // Getter
        m_code.append(GetTab() + QString("%1 Get%2() const { return %3; }\n")
            .arg(memberType)
            .arg(funcName)
            .arg(memberName));
        // Setter
        m_code.append(GetTab() + QString("void Set%1(const %2& _%1) { %3 = _%1; }\n")
        .arg(funcName)
        .arg(memberType)
        .arg(memberName));
    }
}

void qgen::SqliteGenerator::GenerateConstructor()
{
    m_code.append(GetTab() + m_className + "(QObject* parent = NULL) : QObject(parent)\n");
    m_code.append(GetTab() + "{}\n");

    // Constructor with query
    m_code.append(GetTab() + m_className + "(const QSqlQuery& query)\n");
    m_code.append(GetTab() + "{\n");
    TabIncrease();
    for (auto const& fieldInfo : m_tableInfo)
    {
        QString transType = fieldInfo.type == "TEXT" ? "String" : "Int";
        m_code.append(GetTab() + QString("m_%1 = query.value(\"%1\").to%2();\n")
            .arg(fieldInfo.name)
            .arg(transType));
    }
    TabDecrease();
    m_code.append(GetTab() + "}\n");
    //TabIncrease();
}

void qgen::SqliteGenerator::GenerateDestructor()
{
	m_code.append(GetTab() + "~" + m_className + "(){}\n");
}

void qgen::SqliteGenerator::GenerateCopyConstructor()
{
    m_code.append(GetTab() + m_className + "(const " + m_className + "& other)\n");
	TabIncrease();
    m_code.append(GetTab() + ":\n");
    for (auto it = m_memberMap.constBegin(); it != m_memberMap.end(); ++it)
    {
		m_code.append(GetTab() + QString("%1(other.%2),\n").arg(it.key()).arg(it.key()));
	}
    m_code.replace(m_code.lastIndexOf(','), 1, QString());
	TabDecrease();
	m_code.append(GetTab() + "{}\n");
}

void qgen::SqliteGenerator::GenerateProperties()
{
    for(auto it = m_memberMap.constBegin(); it != m_memberMap.end(); ++it)
    {
        m_code.append(GetTab() + it.value() + " " + it.key() + ";\n");
    }
}

void qgen::SqliteGenerator::GenerateMacros()
{
    if (m_tableInfo.isEmpty())
        return;
    m_code.append(GetTab() + "Q_OBJECT\n");
    for (auto const& fieldInfo : m_tableInfo)
    {
        // Q_PROPERTY(memberType fieldName MEMBER memberName READ GetFuncName WRITE SetFuncName)
        QString property;
        QString fieldName = fieldInfo.name;
        QString memberType = GetMemberType(fieldInfo.type);
        QString memberName = "m_" + fieldName;
        QString getFuncName = QString("Get%1").arg(fieldName.at(0).toUpper() + fieldName.mid(1).toLower());
        QString setFuncName = getFuncName;
        setFuncName.replace(0, 1, 'S');
        if (memberType.isEmpty())
        {
            QMessageBox::warning(nullptr, "错误", QString("未知的字段类型(%1, %2)").arg(memberName).arg(memberType));
        }
        m_memberMap.insert(memberName, memberType);
        property = GetTab() + "Q_PROPERTY(" 
            + memberType + " "+ fieldName 
            + " MEMBER " + memberName
            + " READ " + getFuncName
            + " WRITE " + setFuncName + ")\n";
        m_code.append(property);
    }
}

QString qgen::SqliteGenerator::GetMemberType(QString type)
{
    if(type == "TEXT")
		return "QString";
	else if(type == "INTEGER" || type == "INT")
		return "int";
	else if(type == "REAL")
        return "double";
    else if(type == "BLOB")
		return "QByteArray";
	else
		return "";
}

void qgen::SqliteGenerator::GenerateOperatorEqual()
{
    QString funcName = m_className + "& " + "operator=";
    QString arguments = "const " + m_className + "& other";
    m_code.append(GetTab() + QString("%1(%2)\n").arg(funcName).arg(arguments));
    m_code.append(GetTab() + "{\n");
    TabIncrease();

    m_code.append(GetTab() + "if (this == &other)\n");
    m_code.append(GetTab() + "{\n");
    TabIncrease();
    m_code.append(GetTab() + "return *this;\n");
    TabDecrease();
    m_code.append(GetTab() + "}\n");

    for(QMap<QString, QString>::const_iterator it = m_memberMap.constBegin(); it != m_memberMap.end(); ++it)
    {
		m_code.append(GetTab() + "this->" + it.key() + " = other." + it.key() + ";\n");
	}
    m_code.append(GetTab() + "return *this;\n");
    TabDecrease();
    m_code.append(GetTab() + "}\n");
}

void qgen::SqliteGenerator::GenerateCreateFunc()
{
}

QDebug operator<<(QDebug dbg, const tbg::FieldInfo info)
{
	dbg.nospace() << QString("(cid = %1, name = %2, type = %3, notNull = %4, dfltValue = %5, pk = %6)")
		.arg(info.cid)
		.arg(info.name)
		.arg(info.type)
		.arg(info.notNull)
		.arg(info.dfltValue)
		.arg(info.pk);
	return dbg.nospace();
}

void qgen::SqliteGenerator::GenerateReadFunc()
{
    //qDebug() << m_tableInfo.GetPrimaryKey();
    QString primaryKeyName = m_tableInfo.GetPrimaryKey().name;
    QString modelName = QString("%1Model").arg(m_tableName);
    // GetAllModel function
    m_code.append(GetTab() + QString("QList<%1Model> GetAll%1()\n").arg(m_tableName));
    m_code.append(GetTab() + "{\n");
    TabIncrease();
    m_code.append(GetTab() + "QList<" + modelName + "> modelList;\n");
    m_code.append(GetTab()
        + QString("QSqlQuery query = helper->Where(dbtable::%1::tableName, \"1 = 1\");\n")
        .arg(m_tableName.toLower()));
    m_code.append(GetTab() + QString("while (query.next())\n"));
    m_code.append(GetTab() + "{\n");
    TabIncrease();
    m_code.append(GetTab() + modelName + " model;\n");

    TabDecrease();
    m_code.append(GetTab() + "}\n");    // end while
    TabDecrease();
    m_code.append(GetTab() + "}\n");	// end GetAllModel function

    // Get model by primary key function
    m_code.append(GetTab() + QString());


}

void qgen::SqliteGenerator::GenerateUpdateFunc()
{
}

void qgen::SqliteGenerator::GenerateDeleteFunc()
{
}

void qgen::SqliteGenerator::GenerateStaticConstructor()
{
    m_code.append(GetTab() + QString("static %1* Instance()\n").arg(m_className));
    m_code.append(GetTab() + "{\n");
    TabIncrease();
    m_code.append(GetTab() + "static " + m_className + " instance;\n");
    m_code.append(GetTab() + "return &instance;\n");
    TabDecrease();
    m_code.append(GetTab() + "}\n");
}

void qgen::SqliteGenerator::GeneratePrivateMembers()
{
	TabDecrease();
    m_code.append(GetTab() + "private:\n");
	TabIncrease();
	// constructor
    m_code.append(GetTab() + QString("%1()\n").arg(m_className));
    m_code.append(GetTab() + "{\n");
    TabIncrease();
    m_code.append(GetTab() + "helper = SqlHelper::Instance();\n");
    TabDecrease();
    m_code.append(GetTab() + "}\n"); // constructor end
    // destructor
    m_code.append(GetTab() + QString("~%1() {}\n").arg(m_className)); // destructor end
    // member
    m_code.append(GetTab() + QString("SqlHelper* helper;\n"));  // member end
	TabDecrease();
}

