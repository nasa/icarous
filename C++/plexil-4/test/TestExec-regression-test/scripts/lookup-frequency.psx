<PLEXILScript>
  <InitialState>
    <State name="temperature" type="real">
      <Value>0</Value>
    </State>
    <State name="time" type="real">
      <Value>0</Value>
    </State>
  </InitialState>
  <!-- At this point, the node should be executing and waiting for the acknowledgement -->
  <Script>
    <!-- This should cause no update: it's more frequent than the high frequency -->
    <Simultaneous>
      <State name="time" type="real">
	<Value>0.5</Value>
      </State>
      <State name="temperature" type="real">
	<Value>1</Value>
      </State>
    </Simultaneous>
    
    <!-- this should cause an update -->
    <Simultaneous>
      <State name="time" type="real">
	<Value>3</Value>
      </State>
      <State name="temperature" type="real">
	<Value>10</Value>
      </State>
    </Simultaneous>
    
    <!-- this should cause a failure.  there wasn't an update in time -->
    <State name="time" type="real">
      <Value>10</Value>
    </State>

    <CommandAbort name="sit_and_wait" type="bool">
      <Result>1</Result>
    </CommandAbort>
  </Script>
</PLEXILScript>