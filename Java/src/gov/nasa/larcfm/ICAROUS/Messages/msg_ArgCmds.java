package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_ArgCmds extends IcarousMessages {
	
	public class command_name {
		public static final int _ARM_ = 0;
		public static final int _TAKEOFF_ = 1;
		public static final int _SETMODE_ = 2;
		public static final int _LAND_ = 3;
		public static final int _GOTOWP = 4;
		public static final int _SETPOS_ = 5;
		public static final int _SETVEL_ = 6;
		public static final int _SETYAW_ = 7;
		public static final int _SETSPEED_ = 8;
		public static final int _STATUS_ = 9;
	}
	
	command_name name;
	float param1,param2;
	float param3,param4;
	float param5,param6;
	float param7,param8;
	byte text[] = new byte[50];
}
