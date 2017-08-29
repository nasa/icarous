<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="While">
    <NodeId>ep2cp_While_d1e3</NodeId>
    <RepeatCondition>
      <EQInternal>
        <NodeOutcomeVariable>
          <NodeRef dir="child">ep2cp_WhileTest</NodeRef>
        </NodeOutcomeVariable>
        <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
      </EQInternal>
    </RepeatCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty" epx="Condition">
          <NodeId>ep2cp_WhileTest</NodeId>
          <PostCondition>
            <LT>
              <IntegerValue>0</IntegerValue>
              <IntegerValue>1</IntegerValue>
            </LT>
          </PostCondition>
        </Node>
        <Node NodeType="Empty" epx="Action">
          <NodeId>One</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="sibling">ep2cp_WhileTest</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <AND>
              <EQInternal>
                <NodeStateVariable>
                  <NodeRef dir="sibling">ep2cp_WhileTest</NodeRef>
                </NodeStateVariable>
                <NodeStateValue>FINISHED</NodeStateValue>
              </EQInternal>
              <EQInternal>
                <NodeFailureVariable>
                  <NodeRef dir="sibling">ep2cp_WhileTest</NodeRef>
                </NodeFailureVariable>
                <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
              </EQInternal>
            </AND>
          </SkipCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
