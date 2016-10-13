import java.io.*;
import java.nio.file.*;
import java.net.*;
import com.MAVLink.*;
import com.MAVLink.icarous.*;

public class BB_SAFEGUARDREADER{

    public static void main(String args[]){

	Path GPIO_export        = Paths.get("/sys/class/gpio/export");
	Path GPIO_unexport      = Paths.get("sys/class/gpio/unexport");

	String port1            = args[0];
	String port2            = args[1];
	
	Path GPIO_direction1    = Paths.get("/sys/class/gpio/gpio"+port1+"/direction");
	Path GPIO_value1        = Paths.get("/sys/class/gpio/gpio"+port1+"/value");

	Path GPIO_direction2    = Paths.get("/sys/class/gpio/gpio"+port2+"/direction");
	Path GPIO_value2        = Paths.get("/sys/class/gpio/gpio"+port2+"/value");
	
	InetAddress host        = null;

	try{
	    host           = InetAddress.getByName("localhost");
	}catch(UnknownHostException e){
	    System.out.println(e);
	}

	DatagramSocket sock = null;
	try{
	    sock     = new DatagramSocket();
	}catch(SocketException e){
	    System.out.println(e);
	}
	
	int udpSendPort         = Integer.parseInt(args[2]);
	    
	try{
	    // Activate port
	    Files.write(GPIO_export,port1.getBytes(),StandardOpenOption.WRITE);
	    Files.write(GPIO_export,port2.getBytes(),StandardOpenOption.WRITE);
	}
	catch(IOException e){	    
	    
	}

	// Set port direction
	try{
	    Files.write(GPIO_direction1,"in".getBytes(),StandardOpenOption.WRITE);
	    Files.write(GPIO_direction2,"in".getBytes(),StandardOpenOption.WRITE);
	}
	catch(IOException e){

	}

	double timeStart = (double) System.nanoTime()/1E9;
	
	while(true){

	    double timeNow = (double) System.nanoTime()/1E9;
	    byte[] input1,input2, buffer;
	    try{
		// Read input from port
		input1  = Files.readAllBytes(GPIO_value1) ;
		input2  = Files.readAllBytes(GPIO_value2) ;
				
		// Constuct MAVLink message
		msg_safeguard msgSafeguard = new msg_safeguard();

		if((int) input1[0] == 48 ){

		    if((int) input2[0] == 48 ){
			msgSafeguard.value         = 0;
		    }
		    else{
			msgSafeguard.value         = 2;
		    }
		    
		}
		else{

		    if((int) input2[0] == 48 ){
			msgSafeguard.value         = 1;
		    }
		    else{
			msgSafeguard.value         = 3;
		    }
		}
								
		MAVLinkPacket raw_packet = msgSafeguard.pack();
		buffer            = raw_packet.encodePacket();
		
		// Send MAVLink message via socket to ICAROUS
		if(timeNow - timeStart > 1){
		    DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
		    sock.send(output);
		    timeStart = timeNow;
		}
		
	    }
	    catch(IOException e){
	    
	    }
	}
	

	

    }
}
