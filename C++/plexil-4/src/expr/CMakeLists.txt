cmake_minimum_required(VERSION 3.7)
project(PlexilExpr)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O2 -fPIC")

add_definitions(-DHAVE_CONFIG_H -DPIC)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../lib)

set(SOURCE_FILES
        Alias.cc
        Alias.hh
        allocateCache.cc
        allocateCache.hh
        ArithmeticOperators.cc
        ArithmeticOperators.hh
        ArrayOperators.cc
        ArrayOperators.hh
        ArrayReference.cc
        ArrayReference.hh
        ArrayVariable.cc
        ArrayVariable.hh
        Assignable.cc
        Assignable.hh
        BooleanOperators.cc
        BooleanOperators.hh
        Comparisons.cc
        Comparisons.hh
        Constant.cc
        Constant.hh
        Expression.cc
        Expression.hh
        ExpressionConstants.cc
        ExpressionConstants.hh
        ExpressionListener.hh
        Function.cc
        Function.hh
        GetValueImpl.cc
        GetValueImpl.hh
        NodeConnector.hh
        NodeConstantExpressions.cc
        NodeConstantExpressions.hh
        NotifierImpl.cc
        NotifierImpl.hh
        Operator.cc
        Operator.hh
        OperatorImpl.cc
        OperatorImpl.hh
        SimpleBooleanVariable.cc
        SimpleBooleanVariable.hh
        StringOperators.cc
        StringOperators.hh
        UserVariable.cc
        UserVariable.hh)

include_directories(
        ./
        ../
        ../utils
        ../value
        )

add_library(PlexilExpr SHARED ${SOURCE_FILES})

target_link_libraries(PlexilExpr wrap nsl m rt pthread dl)
