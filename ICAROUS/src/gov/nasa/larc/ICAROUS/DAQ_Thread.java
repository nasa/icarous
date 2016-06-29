package gov.nasa.larc.ICAROUS;

public class DAQ_Thread implements Runnable{

    public Thread t;
    public String threadName;
    public AircraftData SharedData;
    public ICAROUS_Interface AP;
    
    public DAQ_Thread(String name,AircraftData Input, ICAROUS_Interface apInterface){
	threadName   = name;
	SharedData   = Input;
	AP           = apInterface;	
    }

    public void run(){
	while(true){
	    synchronized(SharedData){
		AP.AP_Read();
	    }
	}
    }

    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

}
