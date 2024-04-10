#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "SqlHelper.h"
#include <QtWidgets/QApplication>
#include <QFileDialog>
#include <QSqlRecord>
#include <QDebug>
#include "SqliteGenerator.h"
void GetAllTableInfo()
{
	auto instance = SqlHelper::Instance();
	auto tableQuery = instance->Where("sqlite_master", "type='table'");
	QStringList tableNameList;
	while (tableQuery.next())
	{
		QString tableName = tableQuery.value("name").toString();
		qDebug() << tableQuery.value("name").toString();
		if (tableQuery.value("name").toString() != "sqlite_sequence")
		{
			tableNameList << tableName;
		}
	}
	for (auto tableName : tableNameList)
	{
		qDebug() << tableName;
		QSqlQuery pragmaQuery = instance->Execute(QString("PRAGMA table_info(%1)").arg(tableName));
		while (pragmaQuery.next()) {
			QSqlRecord record = pragmaQuery.record();
			qDebug() << "Cid: " << record.value(0).toString()
				<< ", Name: " << record.value(1).toString()
				<< ", Type: " << record.value(2).toString()
				<< ", NotNull: " << record.value(3).toString()
				<< ", DefaultValue: " << record.value(4).toString()
				<< ", PK: " << record.value(5).toString();
		}

	}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QString fileName = QFileDialog::getOpenFileName(
		nullptr, // parent widget
		"选择数据库文件", // dialog title
		"./", // initial directory
		"Database files(*.db)" // filter
	);
	
	MainWindow w(nullptr, fileName);
	w.show();
	// ========== DBG =========
	//qgen::SqliteGenerator* instance = qgen::SqliteGenerator::Instance();
	//tbg::TableInfo tabInfo;
	//tbg::FieldInfo fieldInfo(0, "id", "INTEGER", 0, "", 1);
	//tbg::FieldInfo fieldInfo1(1, "date", "TEXT", 0, "", 1);
	//tabInfo.append(fieldInfo);
	//tabInfo.append(fieldInfo1);

	//QString code = instance->GenerateModel("Data", tabInfo);
	//QFile generateModelFile("./testGen/test.h");
	//generateModelFile.open(QIODevice::WriteOnly);
	//generateModelFile.write(code.toStdString().c_str());
	//generateModelFile.close();
	//QFile daoFile("./testGen/dao.h");
	//daoFile.open(QIODevice::WriteOnly);
	//daoFile.write(instance->GenerateDao("Data", tabInfo).toStdString().c_str());
	//daoFile.close();

    return a.exec();
}
