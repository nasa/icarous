package gov.nasa.larcfm.ICAROUS.Interface;

import com.MAVLink.MAVLinkPacket;

import gov.nasa.larcfm.ICAROUS.Icarous;

public class ArduPilot extends MAVLinkInterface {
	
	
	public ArduPilot(Icarous IC){
		super(IC);
	}
	
	@Override
	public void GetData(){
		byte[] buffer = null;
		MAVLinkPacket RcvdPacket = null;
		buffer = PT.Read();
		if(buffer != null){
			for(int i=0;i<buffer.length;++i){
				RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer[i]);
				if(RcvdPacket != null){
					ProcessMessage(RcvdPacket);
					
					pipe.
				}			
			}
		}
	}

}
