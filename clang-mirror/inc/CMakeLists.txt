# Header files
set(CLREFLECT_INCS
    "${CMAKE_CURRENT_LIST_DIR}/ASTParser.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTParserUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/ClangDriver.h"
	"${CMAKE_CURRENT_LIST_DIR}/CLMirrorActionFactory.h"
	"${CMAKE_CURRENT_LIST_DIR}/CLMirrorDiagnosticConsumer.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTDeclsUtils.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTVisitor.h"
	"${CMAKE_CURRENT_LIST_DIR}/Logger.h"
	"${CMAKE_CURRENT_LIST_DIR}/Constants.h"
	"${CMAKE_CURRENT_LIST_DIR}/StringUtils.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeMeta.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeBuffer.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodePrinter.h"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeManager.h"
)

# Add headers to the library target
target_sources(${CXX_EXE_NAME} PRIVATE ${CLREFLECT_INCS})
