package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_ArgCmds extends IcarousMessages {
	
	public static final int ICAROUS_MSG_ID_ARG_CMDS = 1;
	
	public class command_name {
		public static final int _ARM_ = 0;
		public static final int _TAKEOFF_ = 1;
		public static final int _SETMODE_ = 2;
		public static final int _LAND_ = 3;
		public static final int _GOTOWP_ = 4;
		public static final int _SETPOS_ = 5;
		public static final int _SETVEL_ = 6;
		public static final int _SETYAW_ = 7;
		public static final int _SETSPEED_ = 8;
		public static final int _STATUS_ = 9;
	}
		
	public int name;
	public float param1,param2;
	public float param3,param4;
	public float param5,param6;
	public float param7,param8;
	public byte text[] = new byte[50];
	
	
	public msg_ArgCmds() {
		msgid = ICAROUS_MSG_ID_ARG_CMDS;		
	}
}
