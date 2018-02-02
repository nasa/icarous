<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:transform version="2.0"
               xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
               xmlns:sum="plexil-summarizer">

<!--
* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

<xsl:output method="text" omit-xml-declaration="yes"/>

<!-- Generates a consise textual summary of a Core PLEXIL XML file -->
<!-- Invoked by the script plexil/bin/summarize-plexil -->

<xsl:variable name="tab" select= "'  '"/>

<xsl:template match="PlexilPlan">
  <xsl:text>Summary of PLEXIL Plan 

</xsl:text>
  <!-- currently, there can only been one node here, the root node -->
  <xsl:for-each select="Node">
    <xsl:call-template name= "node"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name= "node">
  <xsl:param name= "indent" select= "''"/>
  <xsl:value-of select= "concat($indent, sum:nicer-name(@NodeType), 'Node ')"/>
  <xsl:value-of select= "NodeId"/>
  <xsl:text>
</xsl:text>
  <xsl:if test= "@NodeType = 'NodeList'">
    <xsl:for-each select="NodeBody/NodeList/Node">
      <xsl:call-template name= "node">
        <xsl:with-param name= "indent" select= "concat($indent, $tab)"/>
      </xsl:call-template>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:function name= "sum:nicer-name">
  <xsl:param name= "name"/>
  <xsl:choose>
    <xsl:when test= "$name = 'NodeList'">
      <xsl:sequence select= "'List'"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:sequence select= "$name"/>      
    </xsl:otherwise>
  </xsl:choose>
</xsl:function>

</xsl:transform>
