cmake_minimum_required(VERSION 3.7)
project(xml_parser)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        test/arrayReferenceXmlParserTest.cc
        test/benchmark.cc
        test/commandXmlParserTest.cc
        test/constantXmlParserTest.cc
        test/FactoryTestNodeConnector.cc
        test/FactoryTestNodeConnector.hh
        test/functionXmlParserTest.cc
        test/lookupXmlParserTest.cc
        test/nodeXmlParserTest.cc
        test/parser-test-module.cc
        test/TrivialNodeConnector.cc
        test/TrivialNodeConnector.hh
        test/updateXmlParserTest.cc
        test/variableXmlParserTest.cc
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

add_executable(xml_parser ${SOURCE_FILES})