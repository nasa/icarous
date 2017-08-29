<?xml version="1.0" encoding="UTF-8"?>
<!-- Test cases for detection of bad Real constant formats -->
<PlexilPlan>
  <Node NodeType="Empty">
    <NodeId>invalid-real-format-1</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>trailing-plus</Name>
        <Type>Real</Type>
        <InitialValue>
          <RealValue>0+</RealValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
  </Node>
</PlexilPlan>

