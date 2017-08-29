<?xml version="1.0" encoding="UTF-8"?>
<!-- Test cases for detection of bad Boolean constant formats -->
<PlexilPlan>
  <Node NodeType="Empty">
    <NodeId>invalid-boolean-format-3</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>trailing-junk</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>true+</BooleanValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
  </Node>
</PlexilPlan>

