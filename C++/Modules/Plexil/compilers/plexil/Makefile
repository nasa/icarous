# Copyright (c) 2006-2016, Universities Space Research Association (USRA).
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#    * Neither the name of the Universities Space Research Association nor the
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

PLEXIL_HOME			?= ../..
include $(PLEXIL_HOME)/makeinclude/standard-defs.make

# Program defaults

# Locations
PARSER_SRC_DIR		:= antlr
PARSER_JAVA_DIR		:= generated
PARSER_JAVA_SRC_DIR	:= $(PARSER_JAVA_DIR)/plexil
JAVA_SRCPATH		:= java
JAVA_SRC_DIR		:= $(JAVA_SRCPATH)/plexil
CLASSES_DIR			:= classes
THIRD_PARTY_JAR_DIR	:= $(PLEXIL_HOME)/jars

# Files
PARSER_SRCS		:= $(PARSER_SRC_DIR)/Plexil.g $(PARSER_SRC_DIR)/PlexilTreeTransforms.g
PARSER_JAVA		:= $(PARSER_JAVA_SRC_DIR)/PlexilLexer.java \
                   $(PARSER_JAVA_SRC_DIR)/PlexilParser.java \
                   $(PARSER_JAVA_SRC_DIR)/PlexilTreeTransforms.java
PARSER_CLASSES	:= $(PARSER_JAVA:$(PARSER_JAVA_SRC_DIR)/%.java=$(CLASSES_DIR)/plexil/%.class)
JAVA_SRCS		:= $(wildcard $(JAVA_SRC_DIR)/*.java)
JAVA_CLASSES	:= $(JAVA_SRCS:$(JAVA_SRC_DIR)/%.java=$(CLASSES_DIR)/plexil/%.class)
PLEXILC_JAR		:= $(PLEXIL_HOME)/jars/PlexilCompiler.jar

# Intermediate files that can be deleted after the primary target is built.
DIRT			+= $(PARSER_JAVA) $(PARSER_CLASSES) $(JAVA_CLASSES)

# Invoking ANTLR 3
ANTLR3_TOOL_JAR	:= $(THIRD_PARTY_JAR_DIR)/antlr-3.5.2-complete-no-st3.jar
ANTLR_OPTS		:= -fo $(PARSER_JAVA_SRC_DIR)
ANTLR			:= $(JAVA) -jar $(ANTLR3_TOOL_JAR)

# Other necessities
ANTLR3_RUNTIME_JAR	:= $(ANTLR3_TOOL_JAR)
NANOXML_JAR			:= $(THIRD_PARTY_JAR_DIR)/nanoxml.jar
SAXON_JAR			:= $(THIRD_PARTY_JAR_DIR)/saxon.jar

# Classpath for Java compiler
JAVAC_CLASSPATH	:= $(ANTLR3_RUNTIME_JAR):$(NANOXML_JAR):$(SAXON_JAR):$(CLASSES_DIR)

# Java compiler options
# Support ancient Java versions
JAVAC_OPTIONS	:= -source 1.6 -target 1.6

plexil-default: $(PLEXILC_JAR)

$(PLEXILC_JAR): $(JAVA_CLASSES) $(PARSER_CLASSES) Makefile
	$(RM) $@
	$(JAR) cf $@ -C $(CLASSES_DIR) .

# Hack: After initial build, comment out the following for incremental.
$(JAVA_CLASSES): $(JAVA_SRCS) | $(PARSER_JAVA) $(CLASSES_DIR)

$(PARSER_CLASSES): $(PARSER_JAVA) | $(CLASSES_DIR)

$(CLASSES_DIR):
	$(MKDIR) -p $(CLASSES_DIR)

$(PARSER_JAVA_SRC_DIR)/PlexilLexer.java $(PARSER_JAVA_SRC_DIR)/PlexilParser.java: $(PARSER_SRC_DIR)/Plexil.g
	$(ANTLR) $(ANTLR_OPTS) $<

$(PARSER_JAVA_SRC_DIR)/PlexilTreeTransforms.java: $(PARSER_SRC_DIR)/PlexilTreeTransforms.g $(PARSER_SRC_DIR)/Plexil.g
	$(ANTLR) $(ANTLR_OPTS) $<

$(JAVA_CLASSES): $(CLASSES_DIR)/plexil/%.class : $(JAVA_SRC_DIR)/%.java | $(PARSER_JAVA) $(JAVA_SRCS)
	$(JAVAC) -classpath $(JAVAC_CLASSPATH) \
             -sourcepath "$(PARSER_JAVA_DIR):$(JAVA_SRCPATH)" \
             -d $(CLASSES_DIR) $(JAVAC_OPTIONS) $<

$(PARSER_CLASSES): $(CLASSES_DIR)/plexil/%.class : $(PARSER_JAVA_SRC_DIR)/%.java | $(JAVA_SRCS)
	$(JAVAC) -classpath $(JAVAC_CLASSPATH) \
             -sourcepath "$(PARSER_JAVA_DIR):$(JAVA_SRCPATH)" \
             -d $(CLASSES_DIR) $(JAVAC_OPTIONS) $<

clean: localclean

localclean::
	$(RM) $(PLEXILC_JAR) $(PARSER_JAVA_DIR) $(CLASSES_DIR)

include $(PLEXIL_HOME)/makeinclude/standard-targets.make

# Override the Makedepend target
Makedepend:
