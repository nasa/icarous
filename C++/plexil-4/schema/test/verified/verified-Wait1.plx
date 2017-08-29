<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="Empty" epx="Wait" FileName="foo.ple" LineNo="104" ColNo="1">
    <NodeId>Wait1</NodeId>
    <EndCondition>
      <GE>
        <LookupOnChange>
          <Name>
            <StringValue>time</StringValue>
          </Name>
          <Tolerance>
            <RealValue>0.2</RealValue>
          </Tolerance>
        </LookupOnChange>
        <ADD>
          <RealValue>23.9</RealValue>
          <NodeTimepointValue>
            <NodeId>Wait1</NodeId>
            <NodeStateValue>EXECUTING</NodeStateValue>
            <Timepoint>START</Timepoint>
          </NodeTimepointValue>
        </ADD>
      </GE>
    </EndCondition>
  </Node>
</PlexilPlan>
