<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="Sequence">
    <NodeId>ep2cp_Sequence_d2e1</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>distance</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>direction</Name>
        <Type>Real</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>ep2cp_hdl</Name>
        <Type>String</Type>
      </DeclareVariable>
    </VariableDeclarations>
    <InvariantCondition>
      <NOT>
        <OR>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">ep2cp_CmdWait</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">ep2cp_CmdWait</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">ep2cp_CmdGetParam_distance</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">ep2cp_CmdGetParam_distance</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">ep2cp_CmdGetParam_direction</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">ep2cp_CmdGetParam_direction</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">ep2cp_CmdAction_moveRover</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">ep2cp_CmdAction_moveRover</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
          <AND>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">ep2cp_CmdReturn</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="child">ep2cp_CmdReturn</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </AND>
        </OR>
      </NOT>
    </InvariantCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Command">
          <NodeId>ep2cp_CmdWait</NodeId>
          <EndCondition>
            <IsKnown>
              <StringVariable>ep2cp_hdl</StringVariable>
            </IsKnown>
          </EndCondition>
          <NodeBody>
            <Command>
              <StringVariable>ep2cp_hdl</StringVariable>
              <Name>
                <StringValue>ReceiveCommand</StringValue>
              </Name>
              <Arguments>
                <StringValue>moveRover</StringValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>ep2cp_CmdGetParam_distance</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">ep2cp_CmdWait</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
          <EndCondition>
            <IsKnown>
              <IntegerVariable>distance</IntegerVariable>
            </IsKnown>
          </EndCondition>
          <NodeBody>
            <Command>
              <IntegerVariable>distance</IntegerVariable>
              <Name>
                <StringValue>GetParameter</StringValue>
              </Name>
              <Arguments>
                <StringVariable>ep2cp_hdl</StringVariable>
                <IntegerValue>0</IntegerValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>ep2cp_CmdGetParam_direction</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">ep2cp_CmdGetParam_distance</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
          <EndCondition>
            <IsKnown>
              <RealVariable>direction</RealVariable>
            </IsKnown>
          </EndCondition>
          <NodeBody>
            <Command>
              <RealVariable>direction</RealVariable>
              <Name>
                <StringValue>GetParameter</StringValue>
              </Name>
              <Arguments>
                <StringVariable>ep2cp_hdl</StringVariable>
                <IntegerValue>1</IntegerValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="NodeList">
          <NodeId>ep2cp_CmdAction_moveRover</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">ep2cp_CmdGetParam_direction</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="Empty">
                <NodeId>foo</NodeId>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>ep2cp_CmdReturn</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeStateVariable>
                <NodeRef dir="sibling">ep2cp_CmdAction_moveRover</NodeRef>
              </NodeStateVariable>
              <NodeStateValue>FINISHED</NodeStateValue>
            </EQInternal>
          </StartCondition>
          <NodeBody>
            <Command>
              <Name>
                <StringValue>SendReturnValue</StringValue>
              </Name>
              <Arguments>
                <StringVariable>ep2cp_hdl</StringVariable>
                <BooleanValue>true</BooleanValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
