<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="Try">
    <NodeId>ep2cp_Try_d1e3</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <EndCondition>
      <OR>
        <AND>
          <EQInternal>
            <NodeOutcomeVariable>
              <NodeRef dir="child">One</NodeRef>
            </NodeOutcomeVariable>
            <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
          </EQInternal>
          <EQInternal>
            <NodeStateVariable>
              <NodeRef dir="child">One</NodeRef>
            </NodeStateVariable>
            <NodeStateValue>FINISHED</NodeStateValue>
          </EQInternal>
        </AND>
        <EQInternal>
          <NodeStateVariable>
            <NodeRef dir="child">Two</NodeRef>
          </NodeStateVariable>
          <NodeStateValue>FINISHED</NodeStateValue>
        </EQInternal>
      </OR>
    </EndCondition>
    <PostCondition>
      <OR>
        <EQInternal>
          <NodeOutcomeVariable>
            <NodeRef dir="child">One</NodeRef>
          </NodeOutcomeVariable>
          <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
        </EQInternal>
        <EQInternal>
          <NodeOutcomeVariable>
            <NodeRef dir="child">Two</NodeRef>
          </NodeOutcomeVariable>
          <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
        </EQInternal>
      </OR>
    </PostCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Assignment">
          <NodeId>One</NodeId>
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
        <Node NodeType="Assignment">
          <NodeId>Two</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">One</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
          <PostCondition>
            <EQNumeric>
              <IntegerVariable>foo</IntegerVariable>
              <IntegerValue>6</IntegerValue>
            </EQNumeric>
          </PostCondition>
          <NodeBody>
            <Assignment>
              <IntegerVariable>foo</IntegerVariable>
              <NumericRHS>
                <IntegerValue>6</IntegerValue>
              </NumericRHS>
            </Assignment>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
