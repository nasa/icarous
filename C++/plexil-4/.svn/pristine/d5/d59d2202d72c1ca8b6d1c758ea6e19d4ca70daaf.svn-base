<?xml version="1.0" encoding="UTF-8"?>
<!-- Tests Unknown result, when there's no Else clause -->
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="If">
    <NodeId>ep2cp_If_d1e4</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>test</Name>
        <Type>Boolean</Type>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty" epx="Condition">
          <NodeId>ep2cp_IfTest</NodeId>
          <PostCondition>
            <BooleanVariable>test</BooleanVariable>
          </PostCondition>
        </Node>
        <Node NodeType="Empty" epx="Then">
          <NodeId>One</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="sibling">ep2cp_IfTest</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <EQInternal>
              <NodeFailureVariable>
                <NodeRef dir="sibling">ep2cp_IfTest</NodeRef>
              </NodeFailureVariable>
              <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
            </EQInternal>
          </SkipCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
