<PLEXILScript>
  <InitialState>
    <State name="At" type="bool">
      <Param>Rock</Param>
      <Value>0</Value>
    </State>
  </InitialState>
  <Script>
    <SendPlan file="simple_drive.xml"/>
    <State name="At" type="bool">
      <Param>Rock</Param>
      <Value>1</Value>
    </State>
    <CommandAck name="drive" type="bool">
      <Param type="real">1.0</Param>
      <Result>1</Result>
    </CommandAck>
    <CommandAck name="takeSample" type="bool">
      <Result>1</Result>
    </CommandAck>
  </Script>
  
</PLEXILScript>