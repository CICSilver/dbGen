#pragma once

#include <QtWidgets/QMainWindow>
#include <QStringList>
#include "datatablelistwidget.h"
#include <QMap>
#include "ui_mainwindow.h"
#include "tableDefine.h"
class AbstractGenerator;
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

protected:
    void GenerateFile(const QString& type, const QString& fileName, const QString& pathPrefix, std::function<QString()> generateContent);
    void CopyHelperFile(const char* srcPath, const char* outPath);

protected slots:
    void onTableNameListWidgetItemDoubleClicked(QListWidgetItem* item);
    void onTableNameListWidgetItemClicked(QListWidgetItem* item);
    void onGenerateBtnClicked();

private:
    Ui::MainWindowClass ui;
    QMap<QString, tbg::TableInfo> m_tableInfoMap;
    QStringList m_tableNameList;
    SqlHelper* m_sqlHelper;
    QString m_dbPath;
    AbstractGenerator* m_generator;
};
