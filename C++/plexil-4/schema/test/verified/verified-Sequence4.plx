<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="Sequence">
    <NodeId>ep2cp_Sequence_d1e3</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <InvariantCondition>
      <NOT>
        <OR>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">One</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">One</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">ep2cp_Sequence_d1e29</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">ep2cp_Sequence_d1e29</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">Three</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">Three</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
        </OR>
      </NOT>
    </InvariantCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty">
          <NodeId>One</NodeId>
        </Node>
        <Node NodeType="NodeList" epx="Sequence">
          <NodeId>ep2cp_Sequence_d1e29</NodeId>
          <StartCondition>
            <AND>
              <EQInternal>
                <NodeStateVariable>
                  <NodeRef dir="sibling">One</NodeRef>
                </NodeStateVariable>
                <NodeStateValue>FINISHED</NodeStateValue>
              </EQInternal>
              <EQNumeric>
                <IntegerVariable>foo</IntegerVariable>
                <IntegerValue>0</IntegerValue>
              </EQNumeric>
            </AND>
          </StartCondition>
          <InvariantCondition>
            <NOT>
              <AND>
                <EQInternal>
                  <NodeOutcomeVariable>
                    <NodeRef dir="child">Two</NodeRef>
                  </NodeOutcomeVariable>
                  <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                </EQInternal>
                <EQInternal>
                  <NodeStateVariable>
                    <NodeRef dir="child">Two</NodeRef>
                  </NodeStateVariable>
                  <NodeStateValue>FINISHED</NodeStateValue>
                </EQInternal>
              </AND>
            </NOT>
          </InvariantCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="Empty">
                <NodeId>Two</NodeId>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
        <Node NodeType="Empty">
          <NodeId>Three</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">ep2cp_Sequence_d1e29</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
