<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <Node NodeType="NodeList">
    <NodeId>arrayTooBig</NodeId>
    <VariableDeclarations>

      <!-- an array of integers -->

      <DeclareArray>
        <Name>myIntegerArray</Name>
        <Type>Integer</Type>
        <MaxSize>3</MaxSize>
        <InitialValue>
          <IntegerValue>1</IntegerValue>
          <IntegerValue>2</IntegerValue>
          <IntegerValue>3</IntegerValue>
          <IntegerValue>4</IntegerValue>
        </InitialValue>
      </DeclareArray>

    </VariableDeclarations>
      
    <!-- a start condition exersizing arrays -->

    <StartCondition>
      <AND>

        <!-- unknown integer -->

        <NOT>
          <IsKnown>
            <ArrayElement>
            <Name>myIntegerArray</Name>
            <Index>
              <IntegerValue>2</IntegerValue>
            </Index>
            </ArrayElement>
          </IsKnown>
        </NOT>
      </AND>
    </StartCondition>
    <NodeBody>
      <NodeList>

        <!-- lookup a state -->

        <Node NodeType="Assignment">
          <NodeId>testIntArrayAssignment</NodeId>

          <VariableDeclarations>
          <!-- another array of integers -->

            <DeclareArray>
              <Name>myOtherIntegerArray</Name>
              <Type>Integer</Type>
              <MaxSize>3</MaxSize>
              <InitialValue>
                <IntegerValue>4</IntegerValue>
                <IntegerValue>5</IntegerValue>
                <IntegerValue>6</IntegerValue>
              </InitialValue>
            </DeclareArray>
          </VariableDeclarations>

          <StartCondition>

            <!-- known integer -->

            <EQNumeric>
              <ArrayElement>
                <Name>myIntegerArray</Name>
                <Index>
                  <IntegerValue>0</IntegerValue>
                </Index>
              </ArrayElement>
              <IntegerValue>1</IntegerValue>
            </EQNumeric>
          </StartCondition>

          <!-- test assigning the integer array -->

          <NodeBody>
            <Assignment>
              <ArrayVariable>myIntegerArray</ArrayVariable>
              <ArrayRHS>
                <ArrayVariable>myOtherIntegerArray</ArrayVariable>
              </ArrayRHS>
            </Assignment>
          </NodeBody>

        </Node>

      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
