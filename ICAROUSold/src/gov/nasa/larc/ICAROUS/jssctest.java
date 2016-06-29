/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

import java.util.ArrayList;
import java.util.Arrays;

import gov.nasa.larcfm.Util.f;
import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortList;
import jssc.SerialPortTimeoutException;

public class jssctest {

	static final String packetTail = "*FF\r\n";

	//assumes command starts with a $
	static String packet(String cmd) {
		int crc = calcChecksum8(cmd.substring(1).getBytes());
		return cmd+"*"+Integer.toHexString(crc)+"\r\n";
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String[] portNames = SerialPortList.getPortNames();
		for (int i = 0; i < portNames.length; i++) {
			f.pln(i+" :: "+portNames[i]);
		}
		if (portNames.length > 1) {
			try {
				SerialPort sp = new SerialPort(portNames[1]);
				sp.openPort();
				f.pln("sp="+sp.isOpened());
				sp.setParams(115200, 8, 1, 0);
				String cmd = "$VNRRG,63";				
				String pkt = packet(cmd);
f.pln("pkt="+pkt);				
				byte[] buffer = pkt.getBytes();
				sp.writeBytes(buffer);
				int sz = sp.getInputBufferBytesCount();
				f.pln("sz = "+sz);			
				//			byte[] bs = sp.readBytes(100, 1000);
				String val = new String();
				for (int j = 0; j < 10; j++) {
					String ret = sp.readString();
					if (ret != null) {
						val = val+ret;
						int idx1 = val.indexOf('$');
						int idx2 = val.indexOf('*');
						if (idx2 > idx1) {
//							int checksum = Long.parseLong(s, radix)valueOf("0x"+Integer.pval.substring(idx2+1,idx2+3));
//							String csstr = val.substring(idx1+1,idx2);
							val = val.substring(idx1,idx2+3);
//							f.pln("csstr="+csstr);
//							if (checksum == calcChecksum8(csstr.getBytes())) {
//								f.pln("CHECKSUM OK");
//							} else {
//								f.pln("CHECKSUM ERR");
//							}
							break;
						}
					}
					Thread.sleep(100);
				}
				f.pln(val);
				sp.closePort();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	
	public static int calcChecksum8(byte[] data) {
		int crc = 0;
		for (int i = 0; i < data.length; i++) {
			crc ^= data[i];
		}
		return crc;
	}

	public static int calcChecksum16(byte[] data) {
		int crc = 0;
		for (int i = 0; i < data.length; i++) {
			crc = (crc >> 8) | (crc << 8);
			crc ^= data[i];
			crc ^= (crc & 0xff) >> 4;
			crc ^= crc << 12;
			crc ^= (crc & 0x00ff) << 5;
		}
		return crc;
	}

}
