<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Concurrence">
      <NodeId>Root</NodeId>
      <NodeBody>
         <NodeList>
            <Node NodeType="Empty">
               <NodeId>A</NodeId>
               <StartCondition>
                  <GE>
                     <NodeTimepointValue>
                        <NodeId>B</NodeId>
                        <NodeStateValue>EXECUTING</NodeStateValue>
                        <Timepoint>START</Timepoint>
                     </NodeTimepointValue>
                     <IntegerValue>0</IntegerValue>
                  </GE>
               </StartCondition>
            </Node>
            <Node NodeType="Empty">
               <NodeId>B</NodeId>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>