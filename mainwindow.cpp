#include "mainwindow.h"
#include <QSqlRecord>
#include <QMessageBox>
#include <QFileDialog>
#include "SqlHelper.h"
#include "SqliteGenerator.h"
MainWindow::MainWindow(QWidget *parent, QString dbPath)
    : QMainWindow(parent), m_dbPath(dbPath)
{
    ui.setupUi(this);
	m_sqlHelper = SqlHelper::Instance();
	m_generator = qgen::SqliteGenerator::Instance();
	ui.qtCheckBox->setVisible(false);
	m_sqlHelper->SetDBPath(dbPath);
	m_sqlHelper->ConnectToDB();
	// 获取表数据
	GetAllTableInfo();
	// 初始化表名列表
	InitTableNameList();
	// 显示选中表信息
	ShowTableInfoWithSelectedTable();

	connect(ui.tableNameListWidget, &DataTableListWidget::itemDoubleClicked,
		this, &MainWindow::onTableNameListWidgetItemDoubleClicked);
	connect(ui.tableNameListWidget, &DataTableListWidget::itemClicked,
		this, &MainWindow::onTableNameListWidgetItemClicked);
	connect(ui.generateBtn, &QPushButton::clicked,
		this, &MainWindow::onGenerateBtnClicked);
}

MainWindow::~MainWindow()
{}

void MainWindow::GetAllTableInfo()
{
	auto tableQuery = m_sqlHelper->Where("sqlite_master", "type='table'");
	while (tableQuery.next())
	{
		QString tableName = tableQuery.value("name").toString();
		if (tableQuery.value("name").toString() != "sqlite_sequence")
		{
			m_tableNameList << tableName;
		}
	}
	if (m_tableNameList.isEmpty())
	{
		QMessageBox::information(this, "提示", "读取表数据失败！");
	}
	for (auto tableName : m_tableNameList)
	{
		QSqlQuery pragmaQuery = m_sqlHelper->Execute(QString("PRAGMA table_info(%1)").arg(tableName));
		tbg::TableInfo tableInfoList;
		while (pragmaQuery.next()) {
			QSqlRecord record = pragmaQuery.record();
			int cid = record.value(0).toInt();
			QString name = record.value(1).toString();
			QString type = record.value(2).toString();
			int notNull = record.value(3).toInt();
			QString defaultValue = record.value(4).toString();
			int pk = record.value(5).toInt();
			tableInfoList << tbg::FieldInfo(cid, name, type, notNull, defaultValue, pk);
			if(pk == 1)
			{
				tableInfoList.SetPrimaryKey(tableInfoList.size() - 1);
			}
		}
		m_tableInfoMap.insert(tableName, tableInfoList);

	}
}

void MainWindow::InitTableNameList()
{
	for (auto const& tableName : m_tableNameList)
	{
		ui.tableNameListWidget->addItem(tableName);
	}
	ui.tableNameListWidget->setCurrentRow(0);
}

void MainWindow::ShowTableInfoWithSelectedTable()
{
	//init tableWidget
	QString currentSelectTable = ui.tableNameListWidget->currentItem()->text();
	QTableWidget* tableInfoWidget = ui.tableInfoWidget;
	if (tableInfoWidget->columnCount() == 0)
	{
		tableInfoWidget->setColumnCount(6);
		tableInfoWidget->setColumnWidth(0, 55);
		tableInfoWidget->setColumnWidth(2, 89);
		tableInfoWidget->setColumnWidth(3, 90);
		//tableInfoWidget->setColumnWidth(4, 109);
		tableInfoWidget->setHorizontalHeaderLabels(QStringList() << "Cid" << "Name" << "Type" << "NotNull" << "DefaultValue" << "PK");
		// 输出表格每列宽度
		//qDebug() << tableInfoWidget->columnWidth(0) << tableInfoWidget->columnWidth(1) << tableInfoWidget->columnWidth(2) << tableInfoWidget->columnWidth(3) << tableInfoWidget->columnWidth(4) << tableInfoWidget->columnWidth(5);
		//qDebug() << this->width();
		tableInfoWidget->horizontalHeader()->setStretchLastSection(true);
		tableInfoWidget->verticalHeader()->hide();
	}
	tableInfoWidget->setRowCount(0);
	tbg::TableInfo tableInfoList = m_tableInfoMap.value(currentSelectTable);
	tableInfoWidget->setRowCount(tableInfoList.size());
	for (int i = 0; i < tableInfoList.size(); i++)
	{
		auto fieldInfo = tableInfoList.at(i);
		tableInfoWidget->setItem(i, 0, new QTableWidgetItem(QString::number(fieldInfo.cid)));
		tableInfoWidget->setItem(i, 1, new QTableWidgetItem(fieldInfo.name));
		tableInfoWidget->setItem(i, 2, new QTableWidgetItem(fieldInfo.type));
		tableInfoWidget->setItem(i, 3, new QTableWidgetItem(QString::number(fieldInfo.notNull)));
		tableInfoWidget->setItem(i, 4, new QTableWidgetItem(fieldInfo.dfltValue));
		tableInfoWidget->setItem(i, 5, new QTableWidgetItem(QString::number(fieldInfo.pk)));
	}
	tableInfoWidget->resizeColumnToContents(1);
	tableInfoWidget->resizeColumnToContents(4);
}

void MainWindow::GenerateFile(const QString& type, const QString& fileName, const QString& pathPrefix, std::function<QString()> generateContent)
{
	QDir dir;

	QString path = pathPrefix + "/" + type;
	if (!dir.exists(path))
		dir.mkpath(path);
	QFile file(QString("%1/%2%3.h").arg(path).arg(fileName.toLower()).arg(type));
	qDebug() << file.fileName();
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, "错误", "文件打开失败!");
		return;
	}
	file.write(generateContent().toStdString().c_str());
	file.close();
}

void MainWindow::onTableNameListWidgetItemDoubleClicked(QListWidgetItem* item)
{
	qDebug() << item->text();
	ShowTableInfoWithSelectedTable();
}

void MainWindow::onTableNameListWidgetItemClicked(QListWidgetItem* item)
{

}

void MainWindow::onGenerateBtnClicked()
{
	auto items = ui.tableNameListWidget->selectedItems();
	QString pathPrefix = "./DB_GeneratedFiles";
	QDir dir;

	// copy sqlhelper file
	QFile srcHeaderFile("./src/sqlhelper.h");
	QFile srcCppFile("./src/sqlhelper.cpp");
	srcHeaderFile.copy("./DB_GeneratedFiles/sqlhelper.h");
	srcCppFile.copy("./DB_GeneratedFiles/sqlhelper.cpp");
	//if(!srcHeaderFile.copy("./DB_GeneratedFiles/sqlHelper.h") ||
	//	!srcCppFile.copy("./DB_GeneratedFiles/sqlHelper.cpp"))
	//{
	//	QMessageBox::information(this, "错误", "数据库操作文件创建失败,可能已有重复文件");
	//}
	auto CheckAndCreateDir = [&dir](QString path)
	{
		if (!dir.exists(path))
			dir.mkpath(path);
	};
	for(auto const& item:items)
	{
		// generate database define file
		//QString path = pathPrefix + "/define";
		//CheckAndCreateDir(path);
		//QFile defineFile(QString("%1/%2define.h").arg(path).arg(item->text().toLower()));
		//qDebug() << defineFile.fileName();
		//if (!defineFile.open(QIODevice::WriteOnly))
		//{
		//	QMessageBox::critical(this, "错误", "define 文件打开失败!");
		//	return;
		//}
		//defineFile.write(m_generator->GenerateDefine(item->text(), m_tableInfoMap.value(item->text())).toStdString().c_str());
		//defineFile.close();
		QString fileName = item->text();
		GenerateFile(
			"define"
			, fileName
			, pathPrefix
			, [this, &item]{
				return m_generator->GenerateDefine(item->text(), m_tableInfoMap.value(item->text()));
			});

		// generate model file
		//path = pathPrefix + "/model";
		//CheckAndCreateDir(path);
		//QFile modelFile(QString("%1/%2model.h").arg(path).arg(item->text().toLower()));
		//qDebug() << modelFile.fileName();
		//if (!modelFile.open(QIODevice::WriteOnly))
		//{
		//	QMessageBox::critical(this, "错误", "model 文件打开失败!");
		//	return;
		//}
		//modelFile.write(m_generator->GenerateModel(item->text(), m_tableInfoMap.value(item->text())).toStdString().c_str());
		//modelFile.close();
		GenerateFile(
			"model"
			, fileName
			, pathPrefix
			, [this, &item]{
				return m_generator->GenerateModel(item->text(), m_tableInfoMap.value(item->text()));
			});

		// generate dao file
		//path = pathPrefix + "/dao";
		//CheckAndCreateDir(path);
		//QFile daoFile(QString("%1/%2dao.h").arg(path).arg(item->text().toLower()));
		//qDebug() << daoFile.fileName();
		//if (!daoFile.open(QIODevice::WriteOnly))
		//{
		//	QMessageBox::critical(this, "错误", "dao 文件打开失败!");
		//	return;
		//}
		//daoFile.write(m_generator->GenerateDao(item->text(), m_tableInfoMap.value(item->text())).toStdString().c_str());
		//daoFile.close();
		GenerateFile(
			"dao"
			, fileName
			, pathPrefix
			, [this, &item]{
				return m_generator->GenerateDao(item->text(), m_tableInfoMap.value(item->text()));
			});

	}
}
