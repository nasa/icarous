<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="SynchronousCommand" FileName="foo.ple" LineNo="104" ColNo="1">
    <NodeId>A</NodeId>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="aux">
          <NodeId>ep2cp_SynchronousCommandAux</NodeId>
          <NodeBody>
            <NodeList>
              <Node NodeType="Command" epx="aux">
                <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                <EndCondition>
                  <OR>
                    <EQInternal>
                      <NodeCommandHandleVariable>
                        <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                      </NodeCommandHandleVariable>
                      <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                    </EQInternal>
                    <EQInternal>
                      <NodeCommandHandleVariable>
                        <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                      </NodeCommandHandleVariable>
                      <NodeCommandHandleValue>COMMAND_FAILED</NodeCommandHandleValue>
                    </EQInternal>
                    <EQInternal>
                      <NodeCommandHandleVariable>
                        <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                      </NodeCommandHandleVariable>
                      <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
                    </EQInternal>
                  </OR>
                </EndCondition>
                <NodeBody>
                  <Command>
                    <Name>
                      <StringValue>foo</StringValue>
                    </Name>
                    <Arguments>
                      <StringValue>this</StringValue>
                    </Arguments>
                  </Command>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
