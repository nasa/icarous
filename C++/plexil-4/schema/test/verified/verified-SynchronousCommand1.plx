<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="SynchronousCommand" FileName="foo.ple" LineNo="104" ColNo="1">
    <NodeId>A</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Integer</Type>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="aux">
          <NodeId>ep2cp_SynchronousCommandAux</NodeId>
          <VariableDeclarations>
            <DeclareVariable>
              <Name>ep2cp_return</Name>
              <Type>Integer</Type>
            </DeclareVariable>
          </VariableDeclarations>
          <NodeBody>
            <NodeList>
              <Node NodeType="Command" epx="aux">
                <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                <EndCondition>
                  <IsKnown>
                    <IntegerVariable>ep2cp_return</IntegerVariable>
                  </IsKnown>
                </EndCondition>
                <NodeBody>
                  <Command>
                    <IntegerVariable>ep2cp_return</IntegerVariable>
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
                    <IntegerVariable>x</IntegerVariable>
                    <NumericRHS>
                      <IntegerVariable>ep2cp_return</IntegerVariable>
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
