#pragma once
#include <QString>
struct TableInfo
{
	TableInfo(int _cid, QString _name, QString _type, int _notNull, QString _dfltValue, int _pk)
		: cid(_cid), name(_name), type(_type), notNull(_notNull), dfltValue(_dfltValue), pk(_pk)
	{}
	int cid;
	QString name;
	QString type;
	int notNull;
	QString dfltValue;
	int pk;
};
