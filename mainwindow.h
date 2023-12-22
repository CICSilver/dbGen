#pragma once

#include <QtWidgets/QMainWindow>
#include <QStringList>
#include <QMap>
#include "ui_mainwindow.h"
#include "tableDefine.h"
class SqlHelper;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QString dbPath = "");
    ~MainWindow();
    void GetAllTableInfo();
    void InitTableNameList();
    void ShowTableInfoWithSelectedTable();

protected slots:
    void onTableNameListWidgetItemDoubleClicked(QListWidgetItem* item);
    void onTableNameListWidgetItemClicked(QListWidgetItem* item);

private:
    Ui::MainWindowClass ui;
    QMap<QString, QList<TableInfo>> m_tableInfoMap;
    QStringList m_tableNameList;
    SqlHelper* m_sqlHelper;
    QString m_dbPath;
};