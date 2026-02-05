# Source files
set(CLREFLECT_SRCS
    
    "${CMAKE_CURRENT_LIST_DIR}/ClangDriver.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/CLMirrorASTParser.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/CLMirrorASTVisitor.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/CLMirrorActionFactory.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/CLMirrorDiagnosticConsumer.cpp"
	
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeGen.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTDeclsUtils.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeBuffer.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodePrinter.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/ASTCodeManager.cpp"
	
	"${CMAKE_CURRENT_LIST_DIR}/main.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/Logger.cpp"
	
    # Add more source files as needed
)

target_sources(${CXX_EXE_NAME} PRIVATE ${CLREFLECT_SRCS})