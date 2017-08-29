<?xml version="1.0" encoding="UTF-8"?>
<!-- Test cases for detection of bad Integer constant formats -->
<PlexilPlan>
  <Node NodeType="Empty">
    <NodeId>invalid-integer-format-4</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>trailing-junk</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0+</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
  </Node>
</PlexilPlan>

