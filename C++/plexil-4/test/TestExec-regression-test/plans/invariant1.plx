<?xml version="1.0" encoding="utf-8"?>
<PlexilPlan>
  <Node NodeType="NodeList">
    <NodeId>invariant1</NodeId>
    <InvariantCondition>
      <NOT>
          <EQInternal>
            <NodeOutcomeVariable>
              <NodeRef dir="child">foo</NodeRef>
            </NodeOutcomeVariable>
            <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
          </EQInternal>
      </NOT>
    </InvariantCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty">
          <NodeId>foo</NodeId>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
