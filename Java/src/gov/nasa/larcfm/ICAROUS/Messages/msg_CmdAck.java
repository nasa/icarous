package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_CmdAck extends IcarousMessages {
	public static final int ICAROUS_MSG_ID_CMD_ACK = 3;	
	public int name;
	public int result;
	
	public msg_CmdAck(){
		msgid = ICAROUS_MSG_ID_CMD_ACK;
	}
	
}
