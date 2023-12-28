#include "mainwindow.h"
#include <QSqlRecord>
#include <QMessageBox>
#include <QFileDialog>
#include "SqlHelper.h"

MainWindow::MainWindow(QWidget *parent, QString dbPath)
    : QMainWindow(parent), m_dbPath(dbPath)
{
    ui.setupUi(this);
	m_sqlHelper = SqlHelper::Instance();
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
		QList<TableInfo> tableInfoList;
		while (pragmaQuery.next()) {
			QSqlRecord record = pragmaQuery.record();
			int cid = record.value(0).toInt();
			QString name = record.value(1).toString();
			QString type = record.value(2).toString();
			int notNull = record.value(3).toInt();
			QString defaultValue = record.value(4).toString();
			int pk = record.value(5).toInt();
			tableInfoList << TableInfo(cid, name, type, notNull, defaultValue, pk);
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
		tableInfoWidget->setColumnWidth(0, 35);
		tableInfoWidget->setColumnWidth(2, 69);
		tableInfoWidget->setColumnWidth(3, 70);
		//tableInfoWidget->setColumnWidth(4, 109);
		tableInfoWidget->setHorizontalHeaderLabels(QStringList() << "Cid" << "Name" << "Type" << "NotNull" << "DefaultValue" << "PK");
		tableInfoWidget->horizontalHeader()->setStretchLastSection(true);
		tableInfoWidget->verticalHeader()->hide();
	}
	tableInfoWidget->setRowCount(0);
	QList<TableInfo> tableInfoList = m_tableInfoMap.value(currentSelectTable);
	tableInfoWidget->setRowCount(tableInfoList.size());
	for (int i = 0; i < tableInfoList.size(); i++)
	{
		TableInfo tableInfo = tableInfoList.at(i);
		tableInfoWidget->setItem(i, 0, new QTableWidgetItem(QString::number(tableInfo.cid)));
		tableInfoWidget->setItem(i, 1, new QTableWidgetItem(tableInfo.name));
		tableInfoWidget->setItem(i, 2, new QTableWidgetItem(tableInfo.type));
		tableInfoWidget->setItem(i, 3, new QTableWidgetItem(QString::number(tableInfo.notNull)));
		tableInfoWidget->setItem(i, 4, new QTableWidgetItem(tableInfo.dfltValue));
		tableInfoWidget->setItem(i, 5, new QTableWidgetItem(QString::number(tableInfo.pk)));
	}
	tableInfoWidget->resizeColumnToContents(1);
	tableInfoWidget->resizeColumnToContents(4);
	//tableInfoWidget->resizeColumnsToContents();
	//qDebug() << tableInfoWidget->width();
	//qDebug() << tableInfoWidget->columnWidth(0) << tableInfoWidget->columnWidth(1) << tableInfoWidget->columnWidth(2)
	//	<< tableInfoWidget->columnWidth(3) << tableInfoWidget->columnWidth(4) << tableInfoWidget->columnWidth(5);
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
	// 选择生成位置
	QString generatePath = QFileDialog::getExistingDirectory(this, "选择生成位置", "./");
	
}
