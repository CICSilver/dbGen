#pragma once
#include <QString>
namespace tbg
{
	// 数据库指定表的一个元素
	struct FieldInfo
	{
		FieldInfo(int _cid = 0
			, QString _name = ""
			, QString _type = ""
			, int _notNull = 0
			, QString _dfltValue = ""
			, int _pk = 0)
			: cid(_cid), name(_name), type(_type), notNull(_notNull), dfltValue(_dfltValue), pk(_pk)
		{}
		int cid;
		QString name;
		QString type;
		int notNull;
		QString dfltValue;
		int pk;
	};

	class TableInfo : public QList<FieldInfo>
	{
	public:
		TableInfo() : QList<FieldInfo>() 
		{
			m_primaryKey_index = -1;
		};
		FieldInfo GetPrimaryKey() const
		{
			return m_primaryKey_index < 0? FieldInfo() : this->at(m_primaryKey_index);
		}

		void SetPrimaryKey(const int index)
		{
			m_primaryKey_index = index;
		}
	private:
		int m_primaryKey_index;
	};
	//using TableInfo = QList<FieldInfo>;
}
