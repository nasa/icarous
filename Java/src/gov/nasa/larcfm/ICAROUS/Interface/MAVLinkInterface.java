package gov.nasa.larcfm.ICAROUS.Interface;

import java.util.ArrayList;

import com.MAVLink.MAVLinkPacket;
import com.MAVLink.Parser;
import com.MAVLink.Messages.MAVLinkMessage;

import gov.nasa.larcfm.ICAROUS.Icarous;
import gov.nasa.larcfm.ICAROUS.Port;
import gov.nasa.larcfm.ICAROUS.Messages.*;

public class MAVLinkInterface implements Interface {

	protected Port PT;
	protected Icarous IC;
	protected Parser MsgParser      = new Parser();
	public MAVLinkInterface pipe;
	
	MAVLinkInterface(Icarous ic){
		IC = ic;
	}
	
	public void SetPipe(MAVLinkInterface intf){
		pipe = intf;
	}
	
	@Override
	public void GetData() {					
		ArrayList<MAVLinkPacket> RcvdData = null;
		MAVLinkPacket RcvdPacket = null;
		RcvdData = ReadMavlinkData();		
		for(int i=0;i<RcvdData.size();++i){
			RcvdPacket = RcvdData.get(i);				
			ProcessMessage(RcvdPacket);						
		}
	}
	
	public ArrayList<MAVLinkPacket> ReadMavlinkData(){
		ArrayList<MAVLinkPacket> listRcvdData = new ArrayList<MAVLinkPacket>();
		byte[] buffer = null;
		MAVLinkPacket RcvdPacket = null;
		buffer = PT.Read();
		if(buffer != null){
			for(int i=0;i<buffer.length;++i){
				RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer[i]);
				if(RcvdPacket != null){
					listRcvdData.add(RcvdPacket);
				}			
			}
		}		
		return listRcvdData;
	}
	
	public void SendMavlinkData(MAVLinkMessage msg2send){
		MAVLinkPacket raw_packet = msg2send.pack();
		raw_packet.sysid  = msg2send.sysid;
		raw_packet.compid = msg2send.compid;	    
		byte[] buffer     = raw_packet.encodePacket();	    	    
		PT.Write(buffer);
	}

	@Override
	public void SendData(IcarousMessages msg) {
		// TODO Auto-generated method stub

	}
	
	public void ProcessMessage(MAVLinkPacket RcvdPacket){
		
	}

}
