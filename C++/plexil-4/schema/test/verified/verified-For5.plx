<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="For">
    <NodeId>Root</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>total</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
      <DeclareVariable>
        <Name>i</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="aux">
          <NodeId>ep2cp_ForLoop</NodeId>
          <SkipCondition>
            <NOT>
              <LT>
                <IntegerVariable>i</IntegerVariable>
                <IntegerValue>3</IntegerValue>
              </LT>
            </NOT>
          </SkipCondition>
          <RepeatCondition>
            <BooleanValue>true</BooleanValue>
          </RepeatCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="NodeList" epx="While">
                <NodeId>ep2cp_While_d1e71</NodeId>
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
                          <IntegerVariable>total</IntegerVariable>
                          <MUL>
                            <IntegerVariable>i</IntegerVariable>
                            <IntegerValue>5</IntegerValue>
                          </MUL>
                        </LT>
                      </PostCondition>
                    </Node>
                    <Node NodeType="Assignment" epx="Action">
                      <NodeId>Inner</NodeId>
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
                      <NodeBody>
                        <Assignment>
                          <IntegerVariable>total</IntegerVariable>
                          <NumericRHS>
                            <ADD>
                              <IntegerVariable>total</IntegerVariable>
                              <IntegerValue>1</IntegerValue>
                            </ADD>
                          </NumericRHS>
                        </Assignment>
                      </NodeBody>
                    </Node>
                  </NodeList>
                </NodeBody>
              </Node>
              <Node NodeType="Assignment" epx="LoopVariableUpdate">
                <NodeId>ep2cp_ForLoopUpdater</NodeId>
                <StartCondition>
                  <EQInternal>
                    <NodeStateVariable>
                      <NodeRef dir="sibling">ep2cp_While_d1e71</NodeRef>
                    </NodeStateVariable>
                    <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
                </StartCondition>
                <NodeBody>
                  <Assignment>
                    <IntegerVariable>i</IntegerVariable>
                    <NumericRHS>
                      <ADD>
                        <IntegerVariable>i</IntegerVariable>
                        <IntegerValue>1</IntegerValue>
                      </ADD>
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
