# Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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

# The original version of this Makefile was contributed by Jeff Kowing
# (jeffrey.d.kowing@nasa.gov)
#
# This makefile generates RELAX NG Compact schemas from XML Schemas.
# A *.rnc RELAX-NG compact schema file will be generated for each
# *.xsd file found in this Makefile's directory.
#
# To generate RELAX-NG Compact schema, type:
# $ make
#
# To remove all generated RELAX-NG Compact schema, type:
# $ make clean
#
# For information about RELAX NG, including information about the
# conversion tools used by this makefile, see http://relaxng.org/.
#
# This makefile requires the following tools:
#
# rngconv - Sun RELAX NX Converter.  This software converts, among
# other things, an XML Schema to a RELAX NG schema.  After
# registering, you can get it at
# http://wwws.sun.com/software/xml/developers/relaxngconverter/.
#
# trang - Trang converts, among other things, a RELAX NG schema to
# RELAX NG Compact form.  Get it at
# http://www.thaiopensource.com/download/trang-20030619.zip.
#
# rncfix - My simple bash script for fixing up the generated
# PlexilPlan RELAX NG Compact schema.
#
# To summarize what this makefile does: 
#
# Given a PlexilPlan XML Schema called PLEXIL_schema.xsd, the steps to
# convert are as follows:
#
# $ java -jar rngconv.jar PLEXIL_schema.xsd > PLEXIL_schema.rng
#
# PLEXIL_schema.rng is the RELAX NG form.
#
# $ java -jar trang.jar PLEXIL_schema.rng PLEXIL_schema.rnc
# 
# PLEXIL_schema.rnc is the RELAX NG compact form.  But a few fixes
# have to be made, so the final step is:
#
# $ mv PLEXIL_schema.rnc PLEXIL_schema.rnc.in
# $ rncfix PLEXIL_schema.rnc.in PLEXIL_schema.rnc 
#
#------------------------------------------------------------------------------

include $(PLEXIL_HOME)/makeinclude/standard-defs.make

# My bone head script to fix up the generated RELAX-NG Compact schema.
RNCFIX := ./rncfix

SCHEMA_XSD = core-plexil.xsd safe-plexil.xsd extended-plexil.xsd plexil-script.xsd
SCHEMA_RNG =    $(SCHEMA_XSD:.xsd=.rng)
SCHEMA_RNC =    $(SCHEMA_XSD:.xsd=.rnc)
SCHEMA_RNC_IN = $(SCHEMA_XSD:.xsd=.rnc.in)

plexil-default: rng rnc

rnc: $(SCHEMA_RNC)

rng: $(SCHEMA_RNG)

dust:
	@ -rm -f $(SCHEMA_RNG)
	@ -rm -f $(SCHEMA_RNC_IN)

clean: dust
	@ -rm -f $(SCHEMA_RNC)
	@ -rm -f plexil-formatted.xsd

# The rule creates a RELAX-NG schema from a XML Schema.
%.rng: %.xsd plexil-base.xsd
	$(JAVA) -jar $(PLEXIL_HOME)/jars/rngconv.jar $< > $@

# The rule creates a RELAX-NG compact schema from a RELAX-NG schema.
%.rnc.in: %.rng
	$(JAVA) -jar $(PLEXIL_HOME)/jars/trang.jar $< $@.rnc
	mv $@.rnc $@

# The rule makes some fixes to the generated RELAX-NG compact schema.
%.rnc: %.rnc.in
	$(RNCFIX) -f $< $@

# Uncomment these if you want to see these intermediate files.
#.PRECIOUS: %.rnc.in 
#.PRECIOUS: %.rng
