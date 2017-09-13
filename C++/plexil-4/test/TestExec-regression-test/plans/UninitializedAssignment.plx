<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <Node NodeType="Assignment">
    <NodeId>UninitializedAssignment</NodeId>
      <VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      </VariableDeclarations>
      <PostCondition>
        <EQNumeric>
          <IntegerVariable>foo</IntegerVariable>
          <IntegerValue>3</IntegerValue>
        </EQNumeric>
      </PostCondition>
      <NodeBody>
        <Assignment>
          <IntegerVariable>foo</IntegerVariable>
          <NumericRHS>
            <IntegerValue>3</IntegerValue>
          </NumericRHS>
        </Assignment>
      </NodeBody>
  </Node>
</PlexilPlan>
