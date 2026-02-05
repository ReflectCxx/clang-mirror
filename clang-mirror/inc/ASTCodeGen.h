
#include <fstream>

namespace clmr {
	class ASTCodeBuffer;
}

namespace clmr
{
	struct ASTCodeGen
	{
		static void emitRegisteredIds(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitRegistrationFns(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitCxxMirrorHeader(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitCxxMirrorSource(std::ofstream& pOut, ASTCodeBuffer*);
		static void emitRegistrationCpp(std::ofstream& pOut, ASTCodeBuffer*);
	};
}