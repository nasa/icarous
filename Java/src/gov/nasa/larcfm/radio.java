import gov.nasa.larcfm.ICAROUS.*;

public class radio{
    public static void main(String args[]){

	Interface SOCKET  = new Interface(Interface.SOCKET,"localhost",
				      Integer.parseInt(args[0]),
				      Integer.parseInt(args[1]),null);

	Interface SERIAL  = new Interface(Interface.SERIAL,args[2],Integer.parseInt(args[3]),null);

	while(true){
	    Interface.PassThrough(SOCKET,SERIAL);
	}
    }    
}
