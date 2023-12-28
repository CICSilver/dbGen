#include "datatablelistwidget.h"
#include <QMenu>
#include <QMouseEvent>
DataTableListWidget::DataTableListWidget(QWidget *parent)
	: QListWidget(parent)
{
	ui.setupUi(this);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

DataTableListWidget::~DataTableListWidget()
{}

void DataTableListWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		QMenu* menu = new QMenu(this);
		QAction* action = new QAction("Add", this);
		menu->addAction(action);
		menu->exec(QCursor::pos());
	}
	QListWidget::mousePressEvent(event);
}