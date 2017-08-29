<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan>
    <GlobalDeclarations>
      <StateDeclaration>
        <Name>time</Name>
        <Return>
          <Type>Real</Type>
        </Return>
      </StateDeclaration>
    </GlobalDeclarations>
  <Node NodeType="NodeList">
    <NodeId>Parent</NodeId>
    <EndCondition>
      <EQInternal>
	<NodeStateVariable>
	  <NodeId>Child2</NodeId>
	</NodeStateVariable>
	<NodeStateValue>FINISHED</NodeStateValue>
      </EQInternal>
    </EndCondition>
    <NodeBody>
      <NodeList>
	<Node NodeType="Empty">
	  <NodeId>Child1</NodeId>
	</Node>
	<Node NodeType="Empty">
	  <NodeId>Child2</NodeId>
	  <StartCondition>
	    <EQInternal>
	      <NodeStateVariable>
		<NodeId>Child1</NodeId>
	      </NodeStateVariable>
	      <NodeStateValue>FINISHED</NodeStateValue>
	    </EQInternal>
	  </StartCondition>
	  <EndCondition>
	    <EQNumeric>
	      <ADD>
		<LookupOnChange>
		  <Name><StringValue>time</StringValue></Name>
		</LookupOnChange>
		<NodeTimepointValue>
                  <!-- KMD: this was Child1, which worked -->
		  <NodeId>Child2</NodeId>
		  <NodeStateValue>EXECUTING</NodeStateValue>
		  <Timepoint>START</Timepoint>
		</NodeTimepointValue>
	      </ADD>
	      <RealValue>0</RealValue>
	    </EQNumeric>
	  </EndCondition>
	</Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
