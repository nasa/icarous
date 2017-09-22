package gov.nasa.larcfm.ICAROUS.Messages;

public class msg_Visbands extends IcarousMessages {
	
	public static final int ICAROUS_MSG_ID_VISBANDS = 8;	
	public int numBands;
	public int type1,type2,type3,type4,type5;
	public float min1,min2,min3,min4,min5;
	public float max1,max2,max3,max4,max5;
	
	public msg_Visbands(){
		msgid = ICAROUS_MSG_ID_VISBANDS;
	}
}
