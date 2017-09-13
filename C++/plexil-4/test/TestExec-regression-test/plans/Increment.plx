<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <Node NodeType="Assignment">
    <NodeId>Increment</NodeId>
	<Interface>
	  <In>
		<DeclareVariable>
		  <Name>x</Name>
		  <Type>Integer</Type>
		</DeclareVariable>
	  </In>
	  <InOut>
		<DeclareVariable>
		  <Name>result</Name>
		  <Type>Integer</Type>
		</DeclareVariable>
	  </InOut>
	</Interface>
    <NodeBody>
	  <Assignment>
		<IntegerVariable>result</IntegerVariable>
		<NumericRHS>
		  <ADD>
			<IntegerVariable>x</IntegerVariable>
			<IntegerValue>1</IntegerValue>
		  </ADD>
		</NumericRHS>
	  </Assignment>
    </NodeBody>
  </Node>
</PlexilPlan>
