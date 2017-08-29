<?xml version="1.0" encoding="UTF-8"?>
<!-- test plan -->
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <GlobalDeclarations>
    <CommandDeclaration>
      <Name>blah</Name>
    </CommandDeclaration>
    <StateDeclaration>
      <Name>Event_5520</Name>
      <Return>
        <Type>Boolean</Type>
      </Return>
    </StateDeclaration>
  </GlobalDeclarations>
 <Node NodeType="NodeList">
   <NodeId>TestEndCondition</NodeId>
   <StartCondition>  
    <EQBoolean>
     <LookupOnChange>
      <Name><StringValue>Event_5520</StringValue></Name>
     </LookupOnChange>
     <BooleanValue>1</BooleanValue>
    </EQBoolean>
   </StartCondition>
   <EndCondition>
     <BooleanValue>1</BooleanValue>
   </EndCondition>
  <NodeBody>
   <NodeList>
    <Node NodeType="Command">
     <NodeId>Step_1_2_3</NodeId>
     <NodeBody>
       <Command>
        <Name><StringValue>blah</StringValue></Name>
       </Command>
     </NodeBody>
    </Node> <!-- end of TestEndCondition.Step 1_2_3 -->
   </NodeList>
  </NodeBody>
 </Node> <!-- End of node TestEndCondition -->
</PlexilPlan>

