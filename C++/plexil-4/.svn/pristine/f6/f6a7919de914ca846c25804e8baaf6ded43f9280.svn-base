<?xml version="1.0" encoding="UTF-8"?>
<!-- test plan -->
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	    xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <GlobalDeclarations>
    <StateDeclaration>
      <Name>time</Name>
      <Return>
        <Type>Real</Type>
      </Return>
    </StateDeclaration>
  </GlobalDeclarations>
  <Node NodeType="Assignment">
    <NodeId>TestTimepoint</NodeId>
    <EndCondition> 
      <GT>
	<LookupOnChange>
	  <Name><StringValue>time</StringValue></Name>
	  <Tolerance><RealValue>180</RealValue></Tolerance>
	</LookupOnChange>
	<ADD>
	  <NodeTimepointValue>
	    <NodeRef dir="self">Three_Min_Timer</NodeRef>
	    <NodeStateValue>EXECUTING</NodeStateValue>
	    <Timepoint>START</Timepoint>
	  </NodeTimepointValue>
	  <IntegerValue>180</IntegerValue>
	</ADD>
      </GT>
    </EndCondition> 
    <VariableDeclarations> 
      <DeclareVariable>
        <Name>hack</Name>
        <Type>Boolean</Type>
      </DeclareVariable>
</VariableDeclarations> 
    <NodeBody>
      <Assignment>
	<BooleanVariable>hack</BooleanVariable>
	<BooleanRHS><BooleanValue>1</BooleanValue></BooleanRHS>
      </Assignment>
    </NodeBody>
  </Node> 
</PlexilPlan>
