<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations LineNo="1" ColNo="0">
      <CommandDeclaration LineNo="1" ColNo="0">
         <Name>Drive</Name>
         <Parameter>
            <Type>Integer</Type>
         </Parameter>
         <Parameter>
            <Type>Boolean</Type>
         </Parameter>
         <Parameter>
            <Type>Real</Type>
         </Parameter>
         <Parameter>
            <Type>String</Type>
         </Parameter>
      </CommandDeclaration>
      <CommandDeclaration LineNo="2" ColNo="8">
         <Name>TakePicture</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>Boolean</Type>
         </Return>
         <Parameter>
            <Type>Integer</Type>
         </Parameter>
         <Parameter>
            <Type>Integer</Type>
         </Parameter>
         <Parameter>
            <Type>Real</Type>
         </Parameter>
      </CommandDeclaration>
      <CommandDeclaration LineNo="3" ColNo="7">
         <Name>update</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>String</Type>
         </Return>
         <Parameter>
            <Name>s</Name>
            <Type>String</Type>
         </Parameter>
      </CommandDeclaration>
      <StateDeclaration LineNo="4" ColNo="8">
         <Name>X</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>Boolean</Type>
         </Return>
      </StateDeclaration>
      <LibraryNodeDeclaration LineNo="5" ColNo="0">
         <Name>LibTest</Name>
         <Interface LineNo="5" ColNo="22">
            <In>
               <DeclareVariable LineNo="5" ColNo="22">
                  <Name>i</Name>
                  <Type>Integer</Type>
               </DeclareVariable>
            </In>
            <InOut>
               <DeclareVariable LineNo="5" ColNo="36">
                  <Name>s</Name>
                  <Type>String</Type>
               </DeclareVariable>
            </InOut>
         </Interface>
      </LibraryNodeDeclaration>
   </GlobalDeclarations>
   <Node NodeType="NodeList" epx="Concurrence" LineNo="8" ColNo="0">
      <NodeId>Test</NodeId>
      <VariableDeclarations>
         <DeclareVariable LineNo="10" ColNo="2">
            <Name>i</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>5</IntegerValue>
            </InitialValue>
         </DeclareVariable>
         <DeclareVariable LineNo="11" ColNo="2">
            <Name>b</Name>
            <Type>Boolean</Type>
            <InitialValue>
               <BooleanValue>true</BooleanValue>
            </InitialValue>
         </DeclareVariable>
         <DeclareArray LineNo="12" ColNo="2">
            <Name>j</Name>
            <Type>Real</Type>
            <MaxSize>4</MaxSize>
            <InitialValue>
               <RealValue>5.0</RealValue>
            </InitialValue>
         </DeclareArray>
         <DeclareVariable LineNo="13" ColNo="2">
            <Name>ess</Name>
            <Type>String</Type>
            <InitialValue>
               <StringValue>4</StringValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <StartCondition>
         <EQInternal>
            <NodeStateVariable>
               <NodeId>ANode</NodeId>
            </NodeStateVariable>
            <NodeStateValue>FINISHED</NodeStateValue>
         </EQInternal>
      </StartCondition>
      <RepeatCondition>
         <NENumeric>
            <IntegerVariable>i</IntegerVariable>
            <IntegerValue>5</IntegerValue>
         </NENumeric>
      </RepeatCondition>
      <PreCondition>
         <EQString>
            <StringValue>hi</StringValue>
            <Concat LineNo="19" ColNo="18">
               <StringValue>h</StringValue>
               <StringValue>i</StringValue>
            </Concat>
         </EQString>
      </PreCondition>
      <PostCondition>
         <NEBoolean>
            <BooleanValue>true</BooleanValue>
            <EQBoolean>
               <BooleanValue>false</BooleanValue>
               <BooleanValue>true</BooleanValue>
            </EQBoolean>
         </NEBoolean>
      </PostCondition>
      <InvariantCondition>
         <OR>
            <AND>
               <NOT>
                  <BooleanVariable>b</BooleanVariable>
               </NOT>
               <IsKnown>
                  <IntegerVariable>i</IntegerVariable>
               </IsKnown>
            </AND>
            <BooleanValue>false</BooleanValue>
         </OR>
      </InvariantCondition>
      <EndCondition>
         <LE>
            <IntegerVariable>i</IntegerVariable>
            <IntegerValue>7</IntegerValue>
         </LE>
      </EndCondition>
      <SkipCondition>
         <OR>
            <OR>
               <EQInternal>
                  <NodeStateValue>WAITING</NodeStateValue>
                  <NodeStateValue>FAILING</NodeStateValue>
               </EQInternal>
               <EQInternal>
                  <NodeStateValue>ITERATION_ENDED</NodeStateValue>
                  <NodeStateValue>INACTIVE</NodeStateValue>
               </EQInternal>
            </OR>
            <EQInternal>
               <NodeStateValue>EXECUTING</NodeStateValue>
               <NodeStateValue>FINISHING</NodeStateValue>
            </EQInternal>
         </OR>
      </SkipCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="Assignment" LineNo="27" ColNo="4">
               <NodeId>ANode</NodeId>
               <StartCondition>
                  <GT>
                     <ArrayElement>
                        <Name>j</Name>
                        <Index>
                           <IntegerValue>1</IntegerValue>
                        </Index>
                     </ArrayElement>
                     <IntegerValue>3</IntegerValue>
                  </GT>
               </StartCondition>
               <NodeBody>
                  <Assignment>
                     <ArrayElement>
                        <Name>j</Name>
                        <Index>
                           <IntegerValue>2</IntegerValue>
                        </Index>
                     </ArrayElement>
                     <NumericRHS>
                        <SUB LineNo="27" ColNo="25">
                           <ADD LineNo="27" ColNo="14">
                              <IntegerValue>3</IntegerValue>
                              <MUL LineNo="27" ColNo="19">
                                 <IntegerValue>2</IntegerValue>
                                 <IntegerValue>1</IntegerValue>
                              </MUL>
                           </ADD>
                           <DIV LineNo="27" ColNo="30">
                              <IntegerValue>5</IntegerValue>
                              <SQRT LineNo="27" ColNo="32">
                                 <ABS LineNo="27" ColNo="38">
                                    <IntegerValue>7</IntegerValue>
                                 </ABS>
                              </SQRT>
                           </DIV>
                        </SUB>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="32" ColNo="4">
               <NodeId>CNode</NodeId>
               <StartCondition>
                  <LookupOnChange>
                     <Name>
                        <StringValue>X</StringValue>
                     </Name>
                  </LookupOnChange>
               </StartCondition>
               <NodeBody>
                  <Command>
                     <Name>
                        <StringValue>Drive</StringValue>
                     </Name>
                     <Arguments LineNo="32" ColNo="10">
                        <IntegerValue>1</IntegerValue>
                        <BooleanValue>false</BooleanValue>
                        <RealValue>1.53</RealValue>
                        <StringValue>31</StringValue>
                     </Arguments>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="LibraryNodeCall">
               <NodeId>LNode</NodeId>
               <NodeBody>
                  <LibraryNodeCall>
                     <NodeId>LibTest</NodeId>
                     <Alias>
                        <NodeParameter>i</NodeParameter>
                        <IntegerVariable>i</IntegerVariable>
                     </Alias>
                     <Alias>
                        <NodeParameter>s</NodeParameter>
                        <StringVariable>ess</StringVariable>
                     </Alias>
                  </LibraryNodeCall>
               </NodeBody>
            </Node>
            <Node NodeType="Empty" LineNo="0" ColNo="0">
               <NodeId>BLOCK__2</NodeId>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>