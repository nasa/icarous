cmake_minimum_required(VERSION 3.7)
project(PlexilXmlParser)

set(LIBRARY_OUTPUT_PATH ${PLEXIL_ROOT}/lib)

set(SOURCE_FILES
        analyzePlan.cc
        ArithmeticFunctionFactory.cc
        ArithmeticFunctionFactory.hh
        ArrayLiteralFactory.cc
        ArrayLiteralFactory.hh
        ArrayVariableFactory.cc
        ArrayVariableFactory.hh
        ArrayVariableReferenceFactory.cc
        ArrayVariableReferenceFactory.hh
        commandXmlParser.cc
        commandXmlParser.hh
        ConcreteExpressionFactory.cc
        ConcreteExpressionFactory.hh
        ExpressionFactory.cc
        ExpressionFactory.hh
        Expressions.cc
        Expressions.hh
        FunctionFactory.cc
        FunctionFactory.hh
        InternalExpressionFactories.cc
        InternalExpressionFactories.hh
        LookupFactory.cc
        LookupFactory.hh
        parseAssignment.cc
        parseAssignment.hh
        parseGlobalDeclarations.cc
        parseGlobalDeclarations.hh
        parseLibraryCall.cc
        parseLibraryCall.hh
        parseNode.cc
        parseNode.hh
        parsePlan.cc
        parsePlan.hh
        parser-utils.cc
        parser-utils.hh
        planLibrary.cc
        planLibrary.hh
        PlexilSchema.cc
        PlexilSchema.hh
        SymbolTable.cc
        SymbolTable.hh
        updateXmlParser.cc
        updateXmlParser.hh
        UserVariableFactory.cc
        UserVariableFactory.hh)

include_directories(
        ./
        ../
        ../utils
        ../value
        ../third-party/pugixml/src
        ../exec
        ../expr
        ../intfc
)

link_directories(${CMAKE_SOURCE_DIR}/../../lib)

add_library(PlexilXmlParser SHARED ${SOURCE_FILES})

target_link_libraries(PlexilXmlParser
        pugixml
        PlexilExec
        PlexilIntfc
        PlexilExpr
        PlexilValue
        PlexilUtils
        wrap nsl m rt pthread dl)
