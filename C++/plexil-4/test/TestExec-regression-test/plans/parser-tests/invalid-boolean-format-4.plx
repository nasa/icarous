<?xml version="1.0" encoding="UTF-8"?>
<!-- Test cases for detection of bad Boolean constant formats -->
<PlexilPlan>
  <Node NodeType="Empty">
    <NodeId>invalid-boolean-format-4</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>trailing-junk</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>false+</BooleanValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
  </Node>
</PlexilPlan>

