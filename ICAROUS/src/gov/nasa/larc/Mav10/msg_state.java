/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package com.MAVLink.myadditions;

import com.MAVLink.MAVLinkPacket;
import com.MAVLink.Messages.MAVLinkMessage;
import com.MAVLink.Messages.MAVLinkPayload;

public class msg_state extends MAVLinkMessage {
  
  public static final int MAVLINK_MSG_ID_STATE = 153; //change as desired
  public static final int MAVLINK_MSG_LENGTH = 52;
  private static final long serialVersionUID = MAVLINK_MSG_ID_STATE;
  
  
  /**
  * This field contains lat info
  */
  public double lat;  //8 bytes
  
  /**
   * This field contains lon info
   */
  public double lon;  //8 bytes
  
  /**
   * This field contains alt info
   */
  public double alt;  //8 bytes
  
  /**
   * This field contains track info
   */
  public double track;  //8 bytes
  
  /**
   * This field contains groundspeed info
   */
  public double groundspeed;  //8 bytes
  
  /**
   * This field contains vertical speed info
   */
  public double verticalspeed;  //8 bytes
  
  /**
   * Generic timestamp field (milliseconds since system boot - using unsigned int for packing)
   */
   public long time_ms; //4 bytes

   
   
  /**
  * Generates the payload for a mavlink message for a message of this type
  * @return
  */
  public MAVLinkPacket pack(){
      MAVLinkPacket packet = new MAVLinkPacket(MAVLINK_MSG_LENGTH);
      packet.sysid = 255;   //change system id here
      packet.compid = 190;  //change component id here
      packet.msgid = MAVLINK_MSG_ID_STATE;
      packet.payload.putDouble(lat);
      packet.payload.putDouble(lon);
      packet.payload.putDouble(alt);
      packet.payload.putDouble(track);
      packet.payload.putDouble(groundspeed);
      packet.payload.putDouble(verticalspeed);
      packet.payload.putUnsignedInt(time_ms);
      return packet;
  }

  /**
  * Decode a test message into this class fields
  *
  * @param payload The message to decode
  */
  public void unpack(MAVLinkPayload payload) {
      payload.resetIndex();
      this.lat = payload.getDouble();
      this.lon = payload.getDouble();
      this.alt = payload.getDouble();
      this.track = payload.getDouble();
      this.groundspeed = payload.getDouble();
      this.verticalspeed = payload.getDouble();
      this.time_ms = payload.getUnsignedInt();
  }

  /**
  * Default constructor
  */
  public msg_state(){
      msgid = MAVLINK_MSG_ID_STATE;
  }

  /**
  * Constructor for a new message, initializes the message with the payload
  * from a mavlink packet
  *
  */
  public msg_state(MAVLinkPacket mavLinkPacket){
      this.sysid = mavLinkPacket.sysid;
      this.compid = mavLinkPacket.compid;
      this.msgid = MAVLINK_MSG_ID_STATE;
      unpack(mavLinkPacket.payload);        
  }

      
  /**
  * Returns a string with the MSG name and data
  */
  public String toString(){
    StringBuilder b = new StringBuilder("MAVLINK_MSG_ID_STATE - \n");
    b.append("Lat: "+ lat +"\n");
    b.append("Lon: "+ lon +"\n");
    b.append("Alt: "+ alt +"\n");
    b.append("Track: "+ track +"\n");
    b.append("Ground Speed: "+ groundspeed +"\n");
    b.append("Vertical Speed: "+ verticalspeed +"\n");
    b.append("Timestamp: " + time_ms);
    return b.toString();
  }
}