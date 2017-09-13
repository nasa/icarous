<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator" FileName="Plan.ple">
  <Node NodeType="NodeList" FileName="Plan.ple" LineNo="2" ColNo="1">
    <NodeId>Interface</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
      <DeclareVariable>
        <Name>y</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
      <DeclareVariable>
        <Name>z</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="Sequence" FileName="Plan.ple" LineNo="7" ColNo="22">
          <NodeId>One</NodeId>
          <InvariantCondition>
            <NOT>
              <OR>
                <AND>
                  <EQInternal>
                    <NodeOutcomeVariable>
                      <NodeRef dir="child">DoFirst</NodeRef>
                    </NodeOutcomeVariable>
                    <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                    <NodeStateVariable>
                      <NodeRef dir="child">DoFirst</NodeRef>
                    </NodeStateVariable>
                    <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
                </AND>
                <AND>
                  <EQInternal>
                    <NodeOutcomeVariable>
                      <NodeRef dir="child">DoSecond</NodeRef>
                    </NodeOutcomeVariable>
                    <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                    <NodeStateVariable>
                      <NodeRef dir="child">DoSecond</NodeRef>
                    </NodeStateVariable>
                    <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
                </AND>
              </OR>
            </NOT>
          </InvariantCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="Assignment" FileName="Plan.ple" LineNo="8" ColNo="34">
                <NodeId>DoFirst</NodeId>
                <NodeBody>
                  <Assignment>
                    <IntegerVariable>x</IntegerVariable>
                    <NumericRHS>
                      <ADD>
                        <IntegerVariable>x</IntegerVariable>
                        <IntegerValue>1</IntegerValue>
                      </ADD>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
              <Node NodeType="Assignment" FileName="Plan.ple" LineNo="9" ColNo="35">
                <NodeId>DoSecond</NodeId>
                <StartCondition>
                  <EQInternal>
                    <NodeStateVariable>
                      <NodeRef dir="sibling">DoFirst</NodeRef>
                    </NodeStateVariable>
                    <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
                </StartCondition>
                <NodeBody>
                  <Assignment>
                    <IntegerVariable>y</IntegerVariable>
                    <NumericRHS>
                      <ADD>
                        <IntegerVariable>y</IntegerVariable>
                        <IntegerValue>1</IntegerValue>
                      </ADD>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
        <Node NodeType="NodeList" epx="Concurrence" FileName="Plan.ple" LineNo="11" ColNo="24">
          <NodeId>Two</NodeId>
          <NodeBody>
            <NodeList>
              <Node NodeType="Assignment" FileName="Plan.ple" LineNo="12" ColNo="38">
                <NodeId>DoFirstDiff</NodeId>
                <NodeBody>
                  <Assignment>
                    <IntegerVariable>z</IntegerVariable>
                    <NumericRHS>
                      <ADD>
                        <IntegerVariable>z</IntegerVariable>
                        <IntegerValue>1</IntegerValue>
                      </ADD>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
              <Node NodeType="Assignment" FileName="Plan.ple" LineNo="13" ColNo="35">
                <NodeId>DoSecond</NodeId>
                <NodeBody>
                  <Assignment>
                    <IntegerVariable>y</IntegerVariable>
                    <NumericRHS>
                      <ADD>
                        <IntegerVariable>y</IntegerVariable>
                        <IntegerValue>1</IntegerValue>
                      </ADD>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
