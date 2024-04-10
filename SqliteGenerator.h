#pragma once
#include "AbstractGenerator.h"
namespace qgen
{
constexpr const char Tab = '\t';
	class SqliteGenerator : public AbstractGenerator
	{
	public:
		static SqliteGenerator* Instance()
		{
			static SqliteGenerator instance;
			return &instance;
		}
	protected:
		void GenerateGetterSetters();
		void GenerateConstructor();
		void GenerateDestructor();
		void GenerateCopyConstructor();
		void GenerateProperties();
		void GenerateMacros();
		QString GetMemberType(QString type);

	private:
		SqliteGenerator() {};
		~SqliteGenerator() {};

		// ͨ�� AbstractGenerator �̳�
		void GenerateOperatorEqual() override;
		void GenerateCreateFunc() override;
		void GenerateReadFunc() override;
		void GenerateUpdateFunc() override;
		void GenerateDeleteFunc() override;

		// ͨ�� AbstractGenerator �̳�
		void GenerateStaticConstructor() override;

		// ͨ�� AbstractGenerator �̳�
		void GeneratePrivateMembers() override;
	};
}

