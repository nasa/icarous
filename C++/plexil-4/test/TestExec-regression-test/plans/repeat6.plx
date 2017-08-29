<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan>

<!-- working node types -->

<!--   <Node NodeType="Command"> -->

<!-- broken node types -->

  <Node NodeType="Empty">
<!--   <Node NodeType="Assignment"> -->

    <NodeId>repeat6</NodeId>

    <!-- variables -->

<!--     <VariableDeclarations> -->
<!--        <DeclareVariable> -->
<!--          <Name>continue</Name> -->
<!--          <Type>Boolean</Type> -->
<!--          <InitialValue> -->
<!--            <BooleanValue>true</BooleanValue> -->
<!--          </InitialValue> -->
<!--        </DeclareVariable> -->
<!--     </VariableDeclarations> -->

    <!-- keep looping until continue == false -->

    <RepeatCondition>
        <LookupOnChange>
          <Name>
            <StringValue>continue</StringValue>
          </Name>
        </LookupOnChange>
    </RepeatCondition>

   <!-- execute a command -->

<!--     <NodeBody> -->
<!--       <Command> -->
<!--         <Name> -->
<!--           <StringValue>c1</StringValue> -->
<!--         </Name> -->
<!--       </Command> -->
<!--     </NodeBody> -->

    <!-- assign array new value -->

<!--           <NodeBody> -->
<!--             <Assignment> -->
<!--               <BooleanVariable>continue</BooleanVariable> -->
<!--               <BooleanRHS> -->
<!--                 <LookupNow> -->
<!--                   <Name> -->
<!--                     <StringValue>continue</StringValue> -->
<!--                   </Name> -->
<!--                 </LookupNow> -->
<!--               </BooleanRHS> -->
<!--             </Assignment> -->
<!--           </NodeBody> -->

<!--           <NodeBody> -->
<!--             <Assignment> -->
<!--               <ArrayVariable>a4</ArrayVariable> -->
<!--               <BooleanRHS> -->
<!--                 <LookupNow> -->
<!--                   <Name> -->
<!--                     <StringValue>a5</StringValue> -->
<!--                   </Name> -->
<!--                 </LookupNow> -->
<!--               </BooleanRHS> -->
<!--             </Assignment> -->
<!--           </NodeBody> -->
  </Node>
</PlexilPlan>
