package gov.nasa.larcfm.ICAROUS.Interface;

import gov.nasa.larcfm.ICAROUS.Messages.IcarousMessages;

public interface Interface {
	public void GetData();
	public void SendData(IcarousMessages msg);
}
