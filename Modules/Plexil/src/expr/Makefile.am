# Copyright (c) 2006-2017, Universities Space Research Association (USRA).
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Universities Space Research Association nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libPlexilExpr.la

libPlexilExpr_la_CPPFLAGS = -I@top_srcdir@/value -I@top_srcdir@/utils

include_HEADERS = Alias.hh allocateCache.hh ArrayReference.hh ArrayVariable.hh \
 Assignable.hh Constant.hh Expression.hh ExpressionConstants.hh \
 ExpressionListener.hh Function.hh GetValueImpl.hh NodeConnector.hh \
 NodeConstantExpressions.hh NotifierImpl.hh Operator.hh \
 SimpleBooleanVariable.hh UserVariable.hh

libPlexilExpr_la_SOURCES = Alias.cc allocateCache.cc \
 ArithmeticOperators.cc ArrayReference.cc ArrayVariable.cc ArrayOperators.cc \
 Assignable.cc BooleanOperators.cc Comparisons.cc Constant.cc Expression.cc \
 ExpressionConstants.cc Function.cc GetValueImpl.cc \
 NodeConstantExpressions.cc NotifierImpl.cc Operator.cc OperatorImpl.cc \
 SimpleBooleanVariable.cc StringOperators.cc UserVariable.cc

if MODULE_TESTS_OPT
  bin_PROGRAMS = test/expr-module-tests
  noinst_HEADERS = test/TrivialListener.hh
  test_expr_module_tests_SOURCES = test/aliasTest.cc test/allocatorTest.cc \
 test/arithmeticTest.cc test/arrayConstantTest.cc test/arrayOperatorsTest.cc \
 test/arrayReferenceTest.cc test/arrayVariableTest.cc \
 test/booleanOperatorsTest.cc test/comparisonsTest.cc test/constantsTest.cc \
 test/functionsTest.cc test/listenerTest.cc test/simpleBooleanVariableTest.cc \
 test/stringTest.cc test/TrivialListener.cc test/variablesTest.cc \
 test/expr-test-module.cc
  test_expr_module_tests_CPPFLAGS = -I../value -I../utils
  test_expr_module_tests_LDADD = libPlexilExpr.la \
 ../value/libPlexilValue.la ../utils/libPlexilUtils.la
endif
