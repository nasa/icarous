<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="Sequence">
    <NodeId>ep2cp_Sequence_d1e3</NodeId>
    <VariableDeclarations>
      <!-- This is the array used for command FOO: -->
      <DeclareArray>
        <Name>x</Name>
        <Type>Integer</Type>
        <MaxSize>4</MaxSize>
      </DeclareArray>
    </VariableDeclarations>
    <InvariantCondition>
      <NOT>
        <OR>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">A</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">A</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">B</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">B</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
        </OR>
      </NOT>
    </InvariantCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="SynchronousCommand" FileName="foo.ple" LineNo="104" ColNo="1">
          <NodeId>A</NodeId>
          <NodeBody>
            <NodeList>
              <Node NodeType="NodeList" epx="aux">
                <NodeId>ep2cp_SynchronousCommandAux</NodeId>
                <VariableDeclarations>
                  <DeclareArray>
                    <Name>ep2cp_return</Name>
                    <Type>Integer</Type>
                    <MaxSize>4</MaxSize>
                  </DeclareArray>
                </VariableDeclarations>
                <NodeBody>
                  <NodeList>
                    <Node NodeType="Command" epx="aux">
                      <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                      <EndCondition>
                        <IsKnown>
                          <ArrayElement>
                            <Name>ep2cp_return</Name>
                            <Index>
                              <IntegerValue>0</IntegerValue>
                            </Index>
                          </ArrayElement>
                        </IsKnown>
                      </EndCondition>
                      <NodeBody>
                        <Command>
                          <ResourceList>
                            <Resource>
                              <ResourceName>
                                <StringValue>arm</StringValue>
                              </ResourceName>
                              <ResourcePriority>
                                <IntegerValue>3</IntegerValue>
                              </ResourcePriority>
                            </Resource>
                          </ResourceList>
                          <ArrayVariable>ep2cp_return</ArrayVariable>
                          <Name>
                            <StringValue>FOO</StringValue>
                          </Name>
                        </Command>
                      </NodeBody>
                    </Node>
                    <Node NodeType="Assignment" epx="aux">
                      <NodeId>ep2cp_SynchronousCommandAssignment</NodeId>
                      <StartCondition>
                        <EQInternal>
                          <NodeStateVariable>
                            <NodeRef dir="sibling">ep2cp_SynchronousCommandCommand</NodeRef>
                          </NodeStateVariable>
                          <NodeStateValue>FINISHED</NodeStateValue>
                        </EQInternal>
                      </StartCondition>
                      <NodeBody>
                        <Assignment>
                          <ArrayVariable>x</ArrayVariable>
                          <ArrayRHS>
                            <ArrayVariable>ep2cp_return</ArrayVariable>
                          </ArrayRHS>
                        </Assignment>
                      </NodeBody>
                    </Node>
                  </NodeList>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
        <Node NodeType="NodeList" epx="SynchronousCommand">
          <NodeId>B</NodeId>
          <VariableDeclarations>
            <!-- This is the array used for command BAR: -->
            <DeclareArray>
              <Name>x</Name>
              <Type>String</Type>
              <MaxSize>20</MaxSize>
            </DeclareArray>
          </VariableDeclarations>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">A</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="NodeList" epx="aux">
                <NodeId>ep2cp_SynchronousCommandAux</NodeId>
                <VariableDeclarations>
                  <DeclareArray>
                    <Name>ep2cp_return</Name>
                    <Type>String</Type>
                    <MaxSize>20</MaxSize>
                  </DeclareArray>
                </VariableDeclarations>
                <NodeBody>
                  <NodeList>
                    <Node NodeType="Command" epx="aux">
                      <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                      <EndCondition>
                        <IsKnown>
                          <ArrayElement>
                            <Name>ep2cp_return</Name>
                            <Index>
                              <IntegerValue>0</IntegerValue>
                            </Index>
                          </ArrayElement>
                        </IsKnown>
                      </EndCondition>
                      <NodeBody>
                        <Command>
                          <ResourceList/>
                          <ArrayVariable>ep2cp_return</ArrayVariable>
                          <Name>
                            <StringValue>BAR</StringValue>
                          </Name>
                        </Command>
                      </NodeBody>
                    </Node>
                    <Node NodeType="Assignment" epx="aux">
                      <NodeId>ep2cp_SynchronousCommandAssignment</NodeId>
                      <StartCondition>
                        <EQInternal>
                          <NodeStateVariable>
                            <NodeRef dir="sibling">ep2cp_SynchronousCommandCommand</NodeRef>
                          </NodeStateVariable>
                          <NodeStateValue>FINISHED</NodeStateValue>
                        </EQInternal>
                      </StartCondition>
                      <NodeBody>
                        <Assignment>
                          <ArrayVariable>x</ArrayVariable>
                          <ArrayRHS>
                            <ArrayVariable>ep2cp_return</ArrayVariable>
                          </ArrayRHS>
                        </Assignment>
                      </NodeBody>
                    </Node>
                  </NodeList>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
