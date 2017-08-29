<?xml version="1.0" encoding="UTF-8"?>
<!-- Test cases for detection of bad Integer constant formats -->
<PlexilPlan>
  <Node NodeType="Empty">
    <NodeId>invalid-integer-formats</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>leading-decimal-point</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>.0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
      <DeclareVariable>
        <Name>trailing-decimal-point</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0.</IntegerValue>
        </InitialValue>
      </DeclareVariable>
      <DeclareVariable>
        <Name>trailing-plus</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0+</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
  </Node>
</PlexilPlan>

