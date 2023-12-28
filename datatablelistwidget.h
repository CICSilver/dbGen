#pragma once

#include <QListWidget>
#include "ui_datatablelistwidget.h"

class DataTableListWidget : public QListWidget
{
	Q_OBJECT

public:
	DataTableListWidget(QWidget *parent = nullptr);
	~DataTableListWidget();

	// 处理右键菜单事件
	 void mousePressEvent(QMouseEvent *event) override;

private:
	Ui::DataTableListWidgetClass ui;
};
