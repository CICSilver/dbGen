#include "mainwindow.h"
#include "SqlHelper.h"
#include <QtWidgets/QApplication>
#include <QFileDialog>
#include <QSqlRecord>

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
    /*MainWindow w;
    w.show();*/
	QString fileName = QFileDialog::getOpenFileName(
		nullptr, // parent widget
		"选择数据库文件", // dialog title
		"./", // initial directory
		"Database files(*.db)" // filter
	);
	MainWindow w(nullptr, fileName);
	w.show();
    return a.exec();
}
