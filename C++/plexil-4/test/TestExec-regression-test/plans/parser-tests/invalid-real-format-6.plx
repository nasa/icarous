<?xml version="1.0" encoding="UTF-8"?>
<!-- Test cases for detection of bad Real constant formats -->
<PlexilPlan>
  <Node NodeType="Empty">
    <NodeId>invalid-real-format-5</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>empty-exponent</Name>
        <Type>Real</Type>
        <InitialValue>
          <RealValue>1E</RealValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
  </Node>
</PlexilPlan>

