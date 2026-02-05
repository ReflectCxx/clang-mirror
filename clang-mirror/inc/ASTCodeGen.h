
#include <fstream>

namespace clmr {
	class ASTCodeBuffer;
}

namespace clmr
{
	struct ASTCodeGen
	{
		static void emitCxxMirrorHeader(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitRegisteredIDsHeader(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitRegistrationInitsHeader(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitRegistrationInitsSource(std::ofstream& pOut, ASTCodeBuffer*);
	};
}