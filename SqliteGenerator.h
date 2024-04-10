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

		// 通过 AbstractGenerator 继承
		void GenerateOperatorEqual() override;
		void GenerateCreateFunc() override;
		void GenerateReadFunc() override;
		void GenerateUpdateFunc() override;
		void GenerateDeleteFunc() override;

		// 通过 AbstractGenerator 继承
		void GenerateStaticConstructor() override;

		// 通过 AbstractGenerator 继承
		void GeneratePrivateMembers() override;
	};
}

