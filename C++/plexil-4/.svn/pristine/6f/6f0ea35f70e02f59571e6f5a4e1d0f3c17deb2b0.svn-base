from primFmttrs import *
from formatters import *
from _IPC import *
from IPC import *

IPC_initialize()

def test0 () :
    IPC_initialize()
    buf = createBuffer(createByteArray(10))
    ds = IPCdata()
    ds._f1 = 666.0
    it = DOUBLE_Trans()
    it.Encode(ds, 1, buf)
    printBuffer(buf)
    rewindBuffer(buf)
    it.Decode(ds, 2, buf)
    print ds._f2
    print it.ELength(ds, 1)
    print it.SimpleType()
    ar = [1, 2, 3]
    rewindBuffer(buf)
    it.EncodeElement(ar, 1, buf)
    printBuffer(buf)
    rewindBuffer(buf)
    it.DecodeElement(ar, 2, buf)
    print ar

def test1 () :
    IPC_initialize()
    buf = createBuffer(createByteArray(10))
    ds = IPCdata()
    ds._f1 = "h"
    st = CHAR_Trans()
    st.Encode(ds, 1, buf)
    printBuffer(buf)
    rewindBuffer(buf)
    st.Decode(ds, 2, buf)
    print ds._f2
    print st.ELength(ds, 1)
    print st.SimpleType()
#    ar = ['eat', 'more', 'chicken']
    ar = ['e', 'm', 'c']
    rewindBuffer(buf)
    st.EncodeElement(ar, 1, buf)
    printBuffer(buf)
    rewindBuffer(buf)
    st.DecodeElement(ar, 2, buf)
    print ar

# This tests all the primitives
def test2 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("int")
    vc1 = IPC_VARCONTENT_TYPE()
    marshall(fmt1, 1, vc1)
    printByteArray(vc1.content, vc1.length)
    print unmarshall(fmt1, vc1.content)
    print

    vc3 = IPC_VARCONTENT_TYPE()
    fmt3 = IPC_parseFormat("boolean")
    marshall(fmt3, False, vc3)
    printByteArray(vc3.content, vc3.length)
    print unmarshall(fmt3, vc3.content)
    print

    vc4 = IPC_VARCONTENT_TYPE()
    fmt4 = IPC_parseFormat("float")
    marshall(fmt4, 1.0, vc4)
    printByteArray(vc4.content, vc4.length)
    print unmarshall(fmt4, vc4.content)
    print

    vc2 = IPC_VARCONTENT_TYPE()
    fmt2 = IPC_parseFormat("double")
    marshall(fmt2, 1.0, vc2)
    printByteArray(vc2.content, vc2.length)
    print unmarshall(fmt2, vc2.content)
    print

    vc5 = IPC_VARCONTENT_TYPE()
    fmt5 = IPC_parseFormat("byte")
    marshall(fmt5, 0XFA, vc5)
    printByteArray(vc5.content, vc5.length)
    print unmarshall(fmt5, vc5.content)
    print

    vc5 = IPC_VARCONTENT_TYPE()
    fmt5 = IPC_parseFormat("ubyte")
    marshall(fmt5, 0XFA, vc5)
    printByteArray(vc5.content, vc5.length)
    print unmarshall(fmt5, vc5.content)
    print

    vc6 = IPC_VARCONTENT_TYPE()
    fmt6 = IPC_parseFormat("string")
    marshall(fmt6, "hello", vc6)
    printByteArray(vc6.content, vc6.length)
    print unmarshall(fmt6, vc6.content)
    print

    vc6 = IPC_VARCONTENT_TYPE()
    fmt6 = IPC_parseFormat("string")
    marshall(fmt6, "", vc6)
    printByteArray(vc6.content, vc6.length)
    print unmarshall(fmt6, vc6.content)
    print

    vc7 = IPC_VARCONTENT_TYPE()
    fmt7 = IPC_parseFormat("char")
    marshall(fmt7, 'c', vc7)
    printByteArray(vc7.content, vc7.length)
    print unmarshall(fmt7, vc7.content)
    print

    vc8 = IPC_VARCONTENT_TYPE()
    fmt8 = IPC_parseFormat("short")
    marshall(fmt8, 666, vc8)
    printByteArray(vc8.content, vc8.length)
    print unmarshall(fmt8, vc8.content)
    print

    vc9 = IPC_VARCONTENT_TYPE()
    fmt9 = IPC_parseFormat("long")
    marshall(fmt9, 0X7FFFFFFF, vc9)
#    marshall(fmt9, -1, vc9)
    printByteArray(vc9.content, vc9.length)
    print unmarshall(fmt9, vc9.content)

class struct1(IPCdata) :
    _fields = ('i', ('a1', 'struct2'))

class struct2(IPCdata) :
    _fields = ('str', 'd')

# test structures
def test3 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("{int, {string, double}}")
    vc1 = IPC_VARCONTENT_TYPE()
    ds = IPCdata()
    ds._f0 = 666
    ds._f1 = IPCdata();
    ds._f1._f0 = "hello"
    ds._f1._f1 = 3.14159
    marshall(fmt1, ds, vc1)
    printByteArray(vc1.content, vc1.length)
    print unmarshall(fmt1, vc1.content)
    print

    ds1 = struct1()
    ds2 = struct2()
    ds1.i = 1234
    ds1.a1 = ds2
    ds2.str = "eat more spam"
    ds2.d = 9.87654321
    marshall(fmt1, ds1, vc1)
    printByteArray(vc1.content, vc1.length)
    ds1 = struct1()
    (obj, ret) = unmarshall(fmt1, vc1.content, ds1)
    print (obj, ret, ds1 == obj, ds2 == ds1.a1)
    print

    ds1 = struct1()
    ds1.a1 = ds2
    (obj, ret) = unmarshall(fmt1, vc1.content, ds1)
    print (obj, ret, ds1 == obj, ds2 == ds1.a1)
    IPC_printData(fmt1, sys.stdout, obj)
    print

    print unmarshall(fmt1, vc1.content, oclass=struct1)
    # Should raise an error
    unmarshall(fmt1, vc1.content, ds1, struct2)

class istruct(IPCdata) :
  _fields = ('i')

# test fixed arrays
def test4 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("[int :5]")
    vc1 = IPC_VARCONTENT_TYPE()
    ds1 = range(10,15)
    marshall(fmt1, ds1, vc1)
    printByteArray(vc1.content, vc1.length)
    print unmarshall(fmt1, vc1.content)
    print

    fmt2 = IPC_parseFormat("[{string, double} :5]")
    vc2 = IPC_VARCONTENT_TYPE()
    ds2 = [None]*5
    foo = ('eat', 'more', 'spam', 'for', 'life')
    for i in range(0,5) :
        ds2[i] = struct2()
        ds2[i].str = foo[i]
        ds2[i].d = float(pow(i,3))
    marshall(fmt2, ds2, vc2)
    printByteArray(vc2.content, vc2.length)
    print unmarshall(fmt2, vc2.content, None, struct2)
    print

    fmt3 = IPC_parseFormat("[int : 3, 4]")
    vc3 = IPC_VARCONTENT_TYPE()
    ds3 = [None]*3
    for i in range(0, 3) : ds3[i] = range(pow(i+1,2), pow(i+1,2)+4)
    marshall(fmt3, ds3, vc3)
    printByteArray(vc3.content, vc3.length)
    print unmarshall(fmt3, vc3.content)

    fmt4 = IPC_parseFormat("[{double} : 3, 4]")
    vc4 = IPC_VARCONTENT_TYPE()
    ds4 = [None]*3
    for i in range(0, 3) :
      ds4[i] = [None]*4
      for j in range(0, 4) : ds4[i][j] = istruct(); ds4[i][j].i = (i+1.0)*(j+1)
    marshall(fmt4, ds4, vc4)
    printByteArray(vc4.content, vc4.length)
    print unmarshall(fmt4, vc4.content, oclass=istruct)

class struct4(IPCdata) :
    _fields = ('num', 'ar')

class struct5(IPCdata) :
    _fields = (('ar', 'struct2'), 'num')

class struct6(IPCdata) :
    _fields = ('dim1', 'dim2', 'ar')

# test variable arrays
def test5 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("{int, <int :1>}")
    vc1 = IPC_VARCONTENT_TYPE()
    ds1 = struct4()
    ds1.num = 5
    ds1.ar = range(10,15)
    marshall(fmt1, ds1, vc1)
    printByteArray(vc1.content, vc1.length)
    print unmarshall(fmt1, vc1.content, struct4())
    print

    fmt2 = IPC_parseFormat("{<{string, double} :2>, int}")
    vc2 = IPC_VARCONTENT_TYPE()
    ds2 = struct5()
    ds2.ar = [None]*5
    ds2.num = 5
    foo = ('eat', 'more', 'spam', 'for', 'life')
    for i in range(0,5) :
        ds2.ar[i] = struct2()
        ds2.ar[i].str = foo[i]
        ds2.ar[i].d = float(pow(i,3))
    marshall(fmt2, ds2, vc2)
    printByteArray(vc2.content, vc2.length)
    print unmarshall(fmt2, vc2.content, struct5())
    print

    fmt3 = IPC_parseFormat("{int, int, <int : 1, 2>}")
    vc3 = IPC_VARCONTENT_TYPE()
    ds3 = struct6()
    ds3.dim1 = 3
    ds3.dim2 = 4
    ds3.ar = [None]*ds3.dim1
    for i in range(0, 3) : ds3.ar[i] = range(pow(i+1,2), pow(i+1,2)+ds3.dim2)
    marshall(fmt3, ds3, vc3)
    printByteArray(vc3.content, vc3.length)
    print unmarshall(fmt3, vc3.content, oclass=struct6)

class struct7(IPCdata) :
    _fields = ('i1', 'status')

WaitVal    = 0
SendVal    = 1
ReceiveVal = 2
ListenVal  = 3

# test enums
def test6 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("{int, {enum WaitVal, SendVal, ReceiveVal, ListenVal}}")
    vc1 = IPC_VARCONTENT_TYPE()
    ds1 = struct7()
    ds1.status = ReceiveVal
    ds1.i1 = 42
    marshall(fmt1, ds1, vc1)
    printByteArray(vc1.content, vc1.length)
    print unmarshall(fmt1, vc1.content, struct7())
    print

    fmt2 = IPC_parseFormat("{int, {enum : 4}}")
    vc2 = IPC_VARCONTENT_TYPE()
    ds2 = struct7()
    ds2.status = ReceiveVal
    ds2.i1 = 42
    marshall(fmt2, ds2, vc2)
    printByteArray(vc2.content, vc2.length)
    print unmarshall(fmt2, vc2.content, struct7())

# test pointers
def test7 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("{int, {string, double}}")
    vc1 = IPC_VARCONTENT_TYPE()
    ds = IPCdata()
    ds._f0 = 666
    ds._f1 = IPCdata();
    ds._f1._f0 = "hello"
    ds._f1._f1 = 3.14159
    marshall(fmt1, ds, vc1)
    printByteArray(vc1.content, vc1.length)
    print unmarshall(fmt1, vc1.content)
    print

    fmt2 = IPC_parseFormat("{*int, *{string, double}}")
    vc2 = IPC_VARCONTENT_TYPE()
    ds = IPCdata()
    ds._f0 = 666
    ds._f1 = IPCdata();
    ds._f1._f0 = "hello"
    ds._f1._f1 = 3.14159
    marshall(fmt2, ds, vc2)
    printByteArray(vc2.content, vc2.length)
    print unmarshall(fmt2, vc2.content, oclass=struct1)
    print

    fmt3 = fmt2
    vc3 = IPC_VARCONTENT_TYPE()
    ds = IPCdata()
    ds._f0 = 666
    ds._f1 = None
    marshall(fmt3, ds, vc3)
    printByteArray(vc3.content, vc3.length)
    print unmarshall(fmt3, vc3.content, oclass=struct1)

# test named formatters
# Need to be connected to IPC central to test this...
def test8 () :
    IPC_initialize()
    fmt1 = IPC_parseFormat("")

def msgHandler1 (msgInstance, data, clientData) :
  print 'msgHandler1', IPC_msgInstanceName(msgInstance), data, clientData

def msgHandler2 (msgInstance, data, clientData) :
  print 'msgHandler2', IPC_msgInstanceName(msgInstance), data, clientData, \
        IPC_dataLength(msgInstance)

def test9 () :
  IPC_connect("test")
  IPC_defineMsg("f", IPC_VARIABLE_LENGTH, "int")
  IPC_subscribeData("f", msgHandler1, 1)
  IPC_publishData("f", 42)
  IPC_listenClear(1000)
  print
  IPC_subscribeData("f", msgHandler2, 3)
  IPC_publishData("f", 666)
  IPC_listenClear(1000)
  print
  IPC_subscribeData("f", msgHandler1, 2)
  print "Num handlers:", IPC_numHandlers("f")
  IPC_publishData("f", 1234)
  IPC_listenClear(1000)
  print
  IPC_unsubscribe("f", msgHandler2)
  print "Num handlers:", IPC_numHandlers("f")
  vc = IPC_VARCONTENT_TYPE()
  IPC_marshall(IPC_msgFormatter("f"), 4321, vc)
  import _IPC
  _IPC.IPC_publishVC("f", vc)
  IPC_listenClear(1000)
  IPC_disconnect()

exit = False

def stdinHnd (fd, clientData) :
  global exit
  msg = sys.stdin.readline()
  print "stdinHnd:", msg,
  if (msg[0] in ('?', 'h')) :
    print "h: help"
    print "q: quit"
    print "u: stop listening"
  if (msg[0] == 'q') :
    print "quit"
    exit = True
  elif (msg[0] == 'u') :
    print "Silent"
    IPC_unsubscribeFD(fd, stdinHnd)

def test10 () :
  global exit
  IPC_connect("test")
  IPC_subscribeFD(sys.stdin.fileno(), stdinHnd)
  exit = False
  while (not exit) : IPC_listen(1000)
  IPC_disconnect()

def test11 () :
  IPC_connect("test")
  IPC_perror("Test")
  IPC_setCapacity(3)
  IPC_defineMsg("f", IPC_VARIABLE_LENGTH, "int")
  IPC_setMsgQueueLength("f", 1)
  IPC_setMsgPriority("f", 2)
  IPC_setVerbosity(IPC_Print_Errors)
  IPC_disconnect()
  IPC_defineMsg("h", IPC_VARIABLE_LENGTH, "int")
  IPC_disconnect()

def test12 () :
  IPC_connect("test")
  IPC_defineMsg("f", IPC_VARIABLE_LENGTH, "int")
  IPC_defineMsg("h", IPC_VARIABLE_LENGTH, "int")
  print 'Here1'
  IPC_subscribeData("f", msgHandler1, 1)
  print 'Here2'
  IPC_subscribeData("f", msgHandler1, 2)
  print 'Here3'
  IPC_subscribeData("g", msgHandler1, 1)
  print 'Here4'
  IPC_disconnect()

def msgHandler3 (msgInstance, byteArray, clientData) :
  (data, retVal) = IPC_unmarshall(IPC_msgFormatter("g"), byteArray)

  print 'msgHandler3', IPC_msgInstanceName(msgInstance), data, clientData, \
        IPC_dataLength(msgInstance)
  IPC_freeByteArray(byteArray)

def test13 () :
  IPC_connect("test")
  IPC_defineMsg("f", 4, None)
  IPC_subscribe("f", msgHandler3, None)
  IPC_defineMsg("g", IPC_VARIABLE_LENGTH, "int")
  vc = IPC_VARCONTENT_TYPE()
  IPC_marshall(IPC_parseFormat("int"), 1234, vc)
  IPC_publish("f", vc.length, vc.content)
  IPC_listenClear(1000)
  IPC_publishVC("f", vc)
  IPC_listenClear(1000)
  IPC_publishFixed("f", vc.content)
  IPC_listenClear(1000)
  IPC_disconnect()

connected = False
disconnected = False

def connectHandler1 (moduleName, clientData) :
  global connected
  connected = True
  print "connectHandler1", moduleName, clientData

def connectHandler2 (moduleName, clientData) :
  print "connectHandler2", moduleName, clientData

def disconnectHandler1 (moduleName, clientData) :
  global disconnected
  disconnected = True
  print "disconnectHandler1", moduleName, clientData

def disconnectHandler2 (moduleName, clientData) :
  print "disconnectHandler2", moduleName, clientData

def test14 () :
  global connected, disconnected
  IPC_connect("test")
  IPC_subscribeConnect(connectHandler1)
  IPC_subscribeDisconnect(disconnectHandler1)  
  connected = False; disconnected = False
  while (not connected or not disconnected) : IPC_listen(1000)
  print "HERE1"
  IPC_subscribeConnect(connectHandler1, 1)
  IPC_subscribeConnect(connectHandler2)
  IPC_subscribeDisconnect(disconnectHandler1, 1)
  IPC_subscribeDisconnect(disconnectHandler2)  
  connected = False; disconnected = False
  while (not connected or not disconnected) : IPC_listen(1000)
  print "HERE2"
  IPC_unsubscribeConnect(connectHandler2)
  IPC_unsubscribeDisconnect(disconnectHandler2)  
  connected = False; disconnected = False
  while (not connected or not disconnected) : IPC_listen(1000)
  print "HERE3"
  IPC_unsubscribeConnect(connectHandler1)
  IPC_unsubscribeDisconnect(disconnectHandler1)  
  connected = False; disconnected = False
  while (not connected or not disconnected) : IPC_listen(1000)
  print "HERE4"
  IPC_disconnect()

changed = False

def changeHandler1 (msgName, numHandlers, clientData) :
  global changed
  changed = True
  print "changeHandler1", msgName, numHandlers, clientData

def changeHandler2 (msgName, numHandlers, clientData) :
  print "changeHandler2", msgName, numHandlers, clientData

def test15 () :
  global changed
  IPC_connect("test")
  IPC_subscribeHandlerChange("message1", changeHandler1)
  changed = False
  while (not changed) : IPC_listen(1000)
  print "HERE1"
  IPC_subscribeHandlerChange("message1", changeHandler1, 1)
  IPC_subscribeHandlerChange("message1", changeHandler2)
  changed = False
  while (not changed) : IPC_listen(1000)
  print "HERE2"
  IPC_unsubscribeHandlerChange("message1", changeHandler2)
  changed = False
  while (not changed) : IPC_listen(1000)
  print "HERE3"
  IPC_unsubscribeHandlerChange("message1", changeHandler1)
  changed = False
  while (not changed) : IPC_listen(1000)
  print "HERE4"
  IPC_disconnect()

def test16 () :
  IPC_connect("test")
  IPC_defineMsg("h", IPC_VARIABLE_LENGTH, "{int, fooType}")
  IPC_defineFormat("fooType", "{string, double}")
  IPC_subscribeData("h", msgHandler2, dataClass=struct1)
  ds = IPCdata()
  ds._f0 = 666
  ds._f1 = IPCdata();
  ds._f1._f0 = "hello"
  ds._f1._f1 = 3.14159
  IPC_setVerbosity(IPC_Print_Errors)
  print "Do formats match? {int, {string, double}} %d" % \
        IPC_checkMsgFormats("h", "{int, {string, double}}")
  print "Do formats match? {int, fooType} %d" % \
        IPC_checkMsgFormats("h", "{int, fooType}")
  print "Do formats match? {int, fooType} %d" % \
        IPC_checkMsgFormats("h", "{int, double}")
  IPC_publishData("h", ds)
  IPC_listenClear(1000)
  print "printData: ",
  sys.stdout.flush()
  IPC_printData(IPC_msgFormatter("h"), sys.stdout, ds)
  IPC_disconnect()

def msgHandler4 (msgInstance, data, clientData) :
  print 'msgHandler4', IPC_msgInstanceName(msgInstance), data, clientData
  vc = IPC_VARCONTENT_TYPE()
  IPC_marshall(IPC_msgFormatter("g"), 9876, vc)
  IPC_respondVC(msgInstance, "g", vc)

def msgHandler5 (msgInstance, data, clientData) :
  print 'msgHandler5', IPC_msgInstanceName(msgInstance), data, clientData
  if (data.i != 0) :
    IPC_respondData(msgInstance, "g", 9876)
  else :
    IPC_respondData(msgInstance, "h", None)

def test17 () :
  IPC_connect("test")
  IPC_defineMsg("f", IPC_VARIABLE_LENGTH, "{int, {string, double}}")
  IPC_defineMsg("g", IPC_VARIABLE_LENGTH, "int")
  IPC_subscribeData("f", msgHandler4, None, struct1)
  IPC_subscribeData("g", msgHandler2, None)
  vc = IPC_VARCONTENT_TYPE()
  ds = IPCdata()
  ds._f0 = 666
  ds._f1 = IPCdata();
  ds._f1._f0 = "hello"
  ds._f1._f1 = 3.14159
  IPC_marshall(IPC_msgFormatter("f"), ds, vc)
  (byteArray, ret) = IPC_queryResponseVC("f", vc, IPC_WAIT_FOREVER)
  (obj, ret) = IPC_unmarshall(IPC_msgFormatter("g"), byteArray)
  IPC_freeByteArray(byteArray)
  print "Reply from query:", obj
  print
  IPC_publishData("g", 4321)
  IPC_listenClear(1000)
  print

  IPC_unsubscribe("f", msgHandler4)
  IPC_subscribeData("f", msgHandler5, None, struct1)
  print "IPC_queryResponseData:", IPC_queryResponseData("f", ds, 5000)
  IPC_defineMsg("h", IPC_VARIABLE_LENGTH, None)
  ds._f0 = 0
  print "IPC_queryResponseData:", IPC_queryResponseData("f", ds, 5000)
  IPC_disconnect()
  
def queryHandler1 (msgInstance, byteArray, clientData) :
  (obj, ret) = IPC_unmarshall(IPC_msgFormatter("g"), byteArray)
  IPC_freeByteArray(byteArray)
  print "queryHandler1: Reply from query:", obj, clientData

def queryHandler2 (msgInstance, data, clientData) :
  print "queryHandler2: Reply from query:", data, clientData

def test18 () :
  IPC_connect("test")
  IPC_defineMsg("f", IPC_VARIABLE_LENGTH, "{int, {string, double}}")
  IPC_defineMsg("g", IPC_VARIABLE_LENGTH, "int")
  IPC_subscribeData("f", msgHandler4, None, struct1)
  vc = IPC_VARCONTENT_TYPE()
  ds = IPCdata()
  ds._f0 = 666
  ds._f1 = IPCdata();
  ds._f1._f0 = "hello"
  ds._f1._f1 = 3.14159
  IPC_marshall(IPC_msgFormatter("f"), ds, vc)
  IPC_queryNotifyVC("f", vc, queryHandler1, 1234)
  IPC_listenClear(1000)
  print

  IPC_queryNotifyData("f", ds, queryHandler2, 1234)
  IPC_listenClear(1000)
  IPC_disconnect()

count = 0
done = False

def timerHnd1 (clientData, currentTime, scheduledTime) :
  global count, done
  count = count + 1
  done = (count == 3)
  print "timerHnd1:", clientData, currentTime, scheduledTime, \
        currentTime - scheduledTime, done

def timerHnd2 (clientData, currentTime, scheduledTime) :
  print "timerHnd2:", clientData, currentTime, scheduledTime, \
        currentTime - scheduledTime

def timerHnd3 (clientData, currentTime, scheduledTime) :
  print "timerHnd3:", clientData, currentTime, scheduledTime, \
        currentTime - scheduledTime

def timerHnd4 (clientData, currentTime, scheduledTime) :
  print "timerHnd4:", clientData, currentTime, scheduledTime, \
        currentTime - scheduledTime

def test19 () :
  global count, done
  done = False; count = 0
  IPC_connect("test")
  IPC_addTimer(1000, 3, timerHnd1, None)
  (ref, retval) = IPC_addTimerGetRef(500, 2, timerHnd2, done)
  IPC_addPeriodicTimer(1250, timerHnd3, IPCdata())
  while (not done) :
    IPC_listen(1000)
  IPC_removeTimer(timerHnd4)
  IPC_removeTimerByRef(ref)
  IPC_removeTimer(timerHnd3)
  IPC_addOneShotTimer(500, timerHnd4, 1)
  IPC_listen(1000)
  IPC_disconnect()
