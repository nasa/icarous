/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package com.MAVLink.myadditions;

import java.util.ArrayList;

import com.MAVLink.MAVLinkPacket;
import com.MAVLink.Messages.MAVLinkMessage;
import com.MAVLink.Messages.MAVLinkPayload;
import gov.nasa.larcfm.Util.Triple;

public class msg_polygon extends MAVLinkMessage {
  
  public static final int MAVLINK_MSG_ID_POLY = 152; //change as desired
  public int MAVLINK_MSG_LENGTH;
  private static final long serialVersionUID = MAVLINK_MSG_ID_POLY;
  private static final int MAX_TRIPLES = 9; 
  
  
  
  /**
  * This field is a list of triples of variable size, triples represent lat, lon, alt
  */
  public ArrayList<Triple<Double,Double,Double>> trips;  //(8*3)*length bytes
    
  /**
  * This field represents height
  */
  public double h;  //8 bytes
  
  /**
   * This field represent if this is a "IN" polygon or an "OUT" polygon: 1 for IN, 0 for OUT 
   * (wasn't sure of best way to do this)
   */
  public byte flag;  //1 byte
  
  /**
   * Generic timestamp field (full 8 byte long - can decide on any timestamp convention)
   */
   public long time_ms; //8 bytes

   /**
    * ID to identify polygon so that multiple packets can be connected - using unsigned int for packing, so only 4 bytes
    */
   public long poly_id; //4 bytes
   
   /**
    * Sequence number within this polygon
    */
   public short sequence; //2 bytes
   
   /**
    * Total number of packets in this polygon
    */
   public short poly_size;  //2 bytes
   
  /**
  * Generates the payload for a mavlink message for a message of this type
  * @return
  */
  public MAVLinkPacket pack(){
      //Message length = trips bytes [(8*3)*trips.size()]+ h bytes [8] + flag bytes [1] + time_ms bytes [8]
      //  + poly_id bytes [4] + sequence bytes [2] + poly_size bytes [2]
      //MAX payload size = 255 bytes, so max num triples = 9
      MAVLINK_MSG_LENGTH = (8*3)*trips.size() + 8 + 1 + 8 + 4 + 2 +2;
      MAVLinkPacket packet = new MAVLinkPacket(MAVLINK_MSG_LENGTH);
      packet.sysid = 255;   //change system id here
      packet.compid = 190;  //change component id here
      packet.msgid = MAVLINK_MSG_ID_POLY;
      int lim = (trips.size() > MAX_TRIPLES) ? MAX_TRIPLES : trips.size();
      Triple<Double, Double, Double> curr;
      for(int i = 0; i < lim; i++){
        curr = trips.get(i);
        packet.payload.putDouble(curr.getFirst());
        packet.payload.putDouble(curr.getSecond());
        packet.payload.putDouble(curr.getThird());
      }
      packet.payload.putDouble(h);
      packet.payload.putByte(flag);
      packet.payload.putLong(time_ms);
      packet.payload.putUnsignedInt(poly_id);
      packet.payload.putShort(sequence);
      packet.payload.putShort(poly_size);
      return packet;
  }

  /**
  * Decode a test message into this class fields
  *
  * @param payload The message to decode
  */
  public void unpack(MAVLinkPayload payload) {
      payload.resetIndex();
      trips = new ArrayList<Triple<Double, Double, Double>>();
      //infer number of triples from payload size : subtract 25 for all fields other than triples, then divide by 24 (each triple = 24 bytes)
      int numTrips = (payload.size() - 25)/24;
      for(int i = 0; i < numTrips; i++){
        trips.add(new Triple<Double, Double, Double>(payload.getDouble(), payload.getDouble(), payload.getDouble()));
      }
      this.h = payload.getDouble();
      this.flag = payload.getByte();
      this.time_ms = payload.getLong();
      this.poly_id = payload.getUnsignedInt();
      this.sequence = payload.getShort();
      this.poly_size = payload.getShort();
      
  }

  /**
  * Default constructor
  */
  public msg_polygon(){
      msgid = MAVLINK_MSG_ID_POLY;
  }

  /**
  * Constructor for a new message, initializes the message with the payload
  * from a mavlink packet
  *
  */
  public msg_polygon(MAVLinkPacket mavLinkPacket){
      this.sysid = mavLinkPacket.sysid;
      this.compid = mavLinkPacket.compid;
      this.msgid = MAVLINK_MSG_ID_POLY;
      unpack(mavLinkPacket.payload);        
  }

      
  /**
  * Returns a string with the MSG name and data
  */
  public String toString(){
    StringBuilder b = new StringBuilder("MAVLINK_MSG_ID_POLY -"+" # Triples: "+trips.size() + "\nTriples: \n\t");
    //we are printing out all triples stored in the list here - but remember, only max of 10 will be sent
    for(int i = 0; i < trips.size(); i++){
      b.append(trips.get(i));
      b.append("\n\t");
    }
    b.append("Height: "+this.h+"\n\t");
    b.append("Flag: "+this.flag+"\n\t");
    b.append("Timestamp: "+this.time_ms+"\n\t");
    b.append("Polygon ID: " + this.poly_id+"\n\t");
    b.append("Sequence Position: Packet " + this.sequence +"/" + this.poly_size+"\n");
    return b.toString();
  }
}
