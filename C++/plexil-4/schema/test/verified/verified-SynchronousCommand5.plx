<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="Concurrence">
    <NodeId>ep2cp_Concurrence_d1e3</NodeId>
    <VariableDeclarations>
      <!-- This is the array that's used for the command: -->
      <DeclareArray>
        <Name>x</Name>
        <Type>Integer</Type>
        <MaxSize>4</MaxSize>
      </DeclareArray>
      <DeclareArray>
        <Name>y</Name>
        <Type>Integer</Type>
        <MaxSize>6</MaxSize>
      </DeclareArray>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="SynchronousCommand" FileName="foo.ple" LineNo="104" ColNo="1">
          <NodeId>A</NodeId>
          <RepeatCondition>
            <BooleanValue>true</BooleanValue>
          </RepeatCondition>
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
                          <ArrayVariable>ep2cp_return</ArrayVariable>
                          <Name>
                            <StringValue>foo</StringValue>
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
