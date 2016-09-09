import java.io.*;
import java.nio.file.*;
import java.net.*;
import com.MAVLink.*;
import com.MAVLink.icarous.*;

public class BB_SAFEGUARDREADER{

    public static void main(String args[]){

	Path GPIO_export        = Paths.get("/sys/class/gpio/export");
	Path GPIO_unexport      = Paths.get("sys/class/gpio/unexport");

	String port             = args[0];
	
	Path GPIO_direction     = Paths.get("/sys/class/gpio/gpio"+port+"/direction");
	Path GPIO_value         = Paths.get("/sys/class/gpio/gpio"+port+"/value");
	
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
	
	int udpSendPort         = Integer.parseInt(args[1]);
	    
	try{
	    // Activate port
	    Files.write(GPIO_export,port.getBytes(),StandardOpenOption.WRITE);
	}
	catch(IOException e){	    
	    
	}

	// Set port direction
	try{
	    Files.write(GPIO_direction,"in".getBytes(),StandardOpenOption.WRITE);
	}
	catch(IOException e){

	}
	
	
	byte[] input, buffer;
	try{
	    // Read input from port
	    input  = Files.readAllBytes(GPIO_value) ;
	    // Constuct MAVLink message
	    msg_safeguard msgSafeguard = new msg_safeguard();
	    msgSafeguard.value         = input[0];
	    MAVLinkPacket raw_packet = msgSafeguard.pack();
	    buffer            = raw_packet.encodePacket();

	    // Send MAVLink message via socket to ICAROUS
	    DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
	    sock.send(output);
	}
	catch(IOException e){
	    
	}

	

	

    }
}
