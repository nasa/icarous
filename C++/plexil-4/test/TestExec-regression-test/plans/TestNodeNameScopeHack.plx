<?xml version="1.0" encoding="UTF-8"?>
<!-- test plan -->
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
 <Node NodeType="NodeList">
  <NodeId>TestNodeNameScopeHack</NodeId>
  <EndCondition>
   <EQInternal>
    <NodeStateVariable>
     <NodeId>B</NodeId>
    </NodeStateVariable>
    <NodeStateValue>FINISHED</NodeStateValue>
   </EQInternal>
  </EndCondition>
  <NodeBody>
   <NodeList>
    <Node NodeType="Assignment">
     <NodeId>condition_eval</NodeId>
     <VariableDeclarations>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Boolean</Type>
      </DeclareVariable>
     </VariableDeclarations> 
     <NodeBody>
      <Assignment>
       <BooleanVariable>x</BooleanVariable>
       <BooleanRHS>
         <BooleanValue>1</BooleanValue> 
       </BooleanRHS>
      </Assignment>
     </NodeBody>
    </Node> 
    <Node NodeType="NodeList">
     <NodeId>B</NodeId>
     <StartCondition>
      <EQInternal>
       <NodeStateVariable>
        <NodeId>condition_eval</NodeId> 
       </NodeStateVariable>
       <NodeStateValue>FINISHED</NodeStateValue>
      </EQInternal>
     </StartCondition>
     <EndCondition>
       <EQInternal>
        <NodeStateVariable>
         <NodeId>D</NodeId>
        </NodeStateVariable>
        <NodeStateValue>FINISHED</NodeStateValue>
       </EQInternal>
     </EndCondition>
     <NodeBody>
      <NodeList>
       <Node NodeType="NodeList"> 
        <NodeId>D</NodeId>
        <NodeBody>
         <NodeList>
          <Node NodeType="Assignment">
           <NodeId>condition_eval</NodeId>
           <VariableDeclarations>        
            <DeclareVariable>
               <Name>y</Name>
               <Type>Boolean</Type>
            </DeclareVariable>
           </VariableDeclarations>
           <NodeBody>
            <Assignment>
             <BooleanVariable>y</BooleanVariable>
              <BooleanRHS><BooleanValue>1</BooleanValue></BooleanRHS>
            </Assignment>
           </NodeBody>
          </Node> 
         </NodeList>
        </NodeBody>
       </Node> 
      </NodeList>
     </NodeBody>
    </Node>  
   </NodeList>
  </NodeBody>
 </Node> 
</PlexilPlan>

