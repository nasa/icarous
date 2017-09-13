<?xml version="1.0" encoding="UTF-8"?>
<!-- test plan -->
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <GlobalDeclarations>
    <StateDeclaration>
      <Name>Event_5520</Name>
      <Return>
        <Type>Boolean</Type>
      </Return>
    </StateDeclaration>
    <StateDeclaration>
      <Name>three_min_timer</Name>
      <Return>
        <Type>Boolean</Type>
      </Return>
    </StateDeclaration>
  </GlobalDeclarations>
 <Node NodeType="NodeList">
   <NodeId>Procedure_5_350</NodeId>
   <StartCondition>  <!-- Have received C&W message 5520 -->
    <EQBoolean>
     <LookupOnChange>
      <Name><StringValue>Event_5520</StringValue></Name>
     </LookupOnChange>
     <BooleanValue>1</BooleanValue>
    </EQBoolean>
   </StartCondition>
  <NodeBody>
   <NodeList>
    <!-- ==================================================================================== -->
    <!-- Step 1, 2, 3: Check to see of the advisory has autocleared then execute branch       -->
    <!-- ==================================================================================== -->
    <Node NodeType="NodeList">
     <NodeId>Step_1_2_3</NodeId>
     <VariableDeclarations>
       <DeclareVariable>
             <Name>advisory_persistent</Name>
             <Type>Boolean</Type>
       </DeclareVariable>
<!--       <DeclareBoolean> -->
<!--        <BooleanVariable>advisory_persistent</BooleanVariable> -->
<!--       </DeclareBoolean> -->
     </VariableDeclarations>
     <NodeBody>
      <!-- Step_1 = Three_Min_Timer + Determine_Persistence  -->
      <NodeList>
       <!-- =============================== -->
       <!-- Wait for theree minutes         -->
       <!-- =============================== -->
       <Node NodeType="Assignment">
        <NodeId>Three_Min_Timer</NodeId>
        <EndCondition> <!-- Currently UE script does not support LookupWithFrequency or Timepoints, hence using a simplified solution -->
         <EQBoolean>
          <LookupOnChange>
           <Name><StringValue>three_min_timer</StringValue></Name>
          </LookupOnChange>
          <BooleanValue>1</BooleanValue>
         </EQBoolean> 
        </EndCondition>
        <VariableDeclarations> <!-- The assignment is a hack since the UE does not have empty node body implemented yet -->
          <DeclareVariable>
            <Name>hack</Name>
            <Type>Boolean</Type>
          </DeclareVariable>
<!--          <DeclareBoolean> -->
<!--           <BooleanVariable>hack</BooleanVariable> -->
<!--          </DeclareBoolean> -->
        </VariableDeclarations>
        <NodeBody>
         <Assignment>
          <BooleanVariable>hack</BooleanVariable>
          <BooleanRHS><BooleanValue>0</BooleanValue></BooleanRHS>
         </Assignment>
        </NodeBody>
       </Node> <!-- End of node Step_1_2_3.Three_Min_Timer -->
       <!-- =============================== -->
       <!-- Determine Persistence           -->
       <!-- =============================== -->
       <Node NodeType="Assignment">
        <NodeId>Determine_Persistence</NodeId>
        <Interface>
         <InOut>
           <DeclareVariable>
             <Name>advisory_persistent</Name>
             <Type>Boolean</Type>
           </DeclareVariable>
<!--              <BooleanVariable>advisory_persistent</BooleanVariable> -->
         </InOut>
        </Interface> 
        <NodeBody>
         <Assignment>
          <BooleanVariable>advisory_persistent</BooleanVariable>
          <BooleanRHS>
           <LookupNow>
            <Name><StringValue>Event_5520</StringValue></Name>
           </LookupNow>
          </BooleanRHS>
         </Assignment>
        </NodeBody>
       </Node> <!-- End of node Step_1_2_3.Determine_Persistence -->
       <Node NodeType="NodeList"> 
        <NodeId>Step_1_Branch</NodeId>
        <VariableDeclarations>
          <DeclareVariable>
            <Name>which_1</Name>
            <Type>Boolean</Type>
          </DeclareVariable>
<!--          <DeclareBoolean> -->
<!--           <BooleanVariable>which_1</BooleanVariable> -->
<!--          </DeclareBoolean> -->
        </VariableDeclarations>
        <Interface>
         <In>
           <DeclareVariable>
             <Name>advisory_persistent</Name>
             <Type>Boolean</Type>
           </DeclareVariable>
<!--            <BooleanVariable>advisory_persistent</BooleanVariable> -->
         </In>
        </Interface> 
        <NodeBody>
         <NodeList>
          <!-- Evaluate condition to select branch from Step_1 to Step_2 or Step_3-->
          <Node NodeType="Assignment">
           <NodeId>condition_eval</NodeId>
           <Interface>
             <In>
               <DeclareVariable>
                 <Name>advisory_persistent</Name>
                 <Type>Boolean</Type>
               </DeclareVariable>
<!--                <BooleanVariable>advisory_persistent</BooleanVariable> -->
             </In>
             <InOut> 
               <DeclareVariable>
                 <Name>which_1</Name>
                 <Type>Boolean</Type>
               </DeclareVariable>
<!--               <BooleanVariable>which_1</BooleanVariable> -->
             </InOut>  
           </Interface>  
           <NodeBody>
            <Assignment>
             <BooleanVariable>which_1</BooleanVariable>
             <BooleanRHS>
              <BooleanVariable>advisory_persistent</BooleanVariable>
             </BooleanRHS>
            </Assignment>
           </NodeBody>
          </Node> 
          </NodeList>
        </NodeBody>
       </Node> <!-- End of Step_1_2_3.Step_1_Branch -->
      </NodeList>
     </NodeBody>
    </Node> <!-- end of Procedure_5_350.Step 1_2_3 -->
   </NodeList>
  </NodeBody>
 </Node> <!-- End of node Procedure_5_350 -->
</PlexilPlan>

