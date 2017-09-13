<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="If">
    <NodeId>Root</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty" epx="Condition">
          <NodeId>ep2cp_IfTest</NodeId>
          <PostCondition>
            <EQNumeric>
              <IntegerVariable>foo</IntegerVariable>
              <IntegerValue>1</IntegerValue>
            </EQNumeric>
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
        <Node NodeType="Empty" epx="ElseIf">
          <NodeId>ep2cp_ElseIf-1</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeFailureVariable>
                <NodeRef dir="sibling">ep2cp_IfTest</NodeRef>
              </NodeFailureVariable>
              <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="sibling">ep2cp_IfTest</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
          </SkipCondition>
          <PostCondition>
            <EQNumeric>
              <IntegerVariable>foo</IntegerVariable>
              <IntegerValue>2</IntegerValue>
            </EQNumeric>
          </PostCondition>
        </Node>
        <Node NodeType="Empty" epx="Then">
          <NodeId>Two</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="sibling">ep2cp_ElseIf-1</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <NOT>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeRef dir="sibling">ep2cp_ElseIf-1</NodeRef>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
              </EQInternal>
            </NOT>
          </SkipCondition>
        </Node>
        <Node NodeType="Empty" epx="ElseIf">
          <NodeId>ep2cp_ElseIf-2</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeFailureVariable>
                <NodeRef dir="sibling">ep2cp_ElseIf-1</NodeRef>
              </NodeFailureVariable>
              <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <OR>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeRef dir="sibling">ep2cp_ElseIf-1</NodeRef>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SKIPPED</NodeOutcomeValue>
              </EQInternal>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeRef dir="sibling">ep2cp_ElseIf-1</NodeRef>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
              </EQInternal>
            </OR>
          </SkipCondition>
          <PostCondition>
            <EQNumeric>
              <IntegerVariable>foo</IntegerVariable>
              <IntegerValue>3</IntegerValue>
            </EQNumeric>
          </PostCondition>
        </Node>
        <Node NodeType="Empty" epx="Then">
          <NodeId>Three</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="sibling">ep2cp_ElseIf-2</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <NOT>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeRef dir="sibling">ep2cp_ElseIf-2</NodeRef>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
              </EQInternal>
            </NOT>
          </SkipCondition>
        </Node>
        <Node NodeType="Empty" epx="Else">
          <NodeId>Four</NodeId>
          <StartCondition>
            <EQInternal>
              <NodeFailureVariable>
                <NodeRef dir="sibling">ep2cp_ElseIf-2</NodeRef>
              </NodeFailureVariable>
              <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
            </EQInternal>
          </StartCondition>
          <SkipCondition>
            <OR>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeRef dir="sibling">ep2cp_ElseIf-2</NodeRef>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SKIPPED</NodeOutcomeValue>
              </EQInternal>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeRef dir="sibling">ep2cp_ElseIf-2</NodeRef>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
              </EQInternal>
            </OR>
          </SkipCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
