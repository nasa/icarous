/* Buffers.java 
 * Release: ACCoRDj-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;


//import static gov.nasa.larcfm.ACCoRD.Consts.*;
//import gov.nasa.larcfm.ACCoRD.*;
import gov.nasa.larcfm.Util.*;

class Buffers {

  static double evx(Velocity v, double eax, double egx) {
    double norm_v = v.vect2().norm();
    return Util.sqrt_safe(2*norm_v*(norm_v+egx)*(1-Math.cos(eax))+
        Util.sq(egx));
  }

  public static void main(String args[]) {
    double t;
    Velocity vo,vi;

    double eso,esi,eao,ego,eai,egi;
    eso = esi = Units.from("ft",10);
    eao = eai = Units.from("deg",3);
    ego = egi = Units.from("kn",5);

    int kind = 1;

    if (kind == 0) {
      System.out.println("vm [kn], T=1 [min], T=3 [min], T=5 [min]");
      for (int gs=50;gs <= 400; gs+=3){
        vo = Velocity.makeVxyz( 
            // Ownship: vx vy [kn], vz [fpm]
            gs,0,"kn", 0,"fpm");
        vi = Velocity.makeVxyz( 
            // Traffic: vx vy [kn], vz [fpm]
            -gs,0,"kn", 0,"fpm");
        double eps = eso+esi;
        double evo = evx(vo,eao,ego);
        double evi = evx(vi,eai,egi);
        double epv  = evo+evi;
        t = Units.from("min",1);
        double psi1 = eps+t*epv;
        t = Units.from("min",3);
        double psi3 = eps+t*epv;
        t = Units.from("min",5);
        double psi5 = eps+t*epv;
        Vect2 v = vo.vect2().Sub(vi.vect2());
        System.out.println(Units.to("kn",v.norm())+", "+
            Units.to("nm",psi1)+", "+
            Units.to("nm",psi3)+", "+
            Units.to("nm",psi5));
      }
    } else if (kind == 1) {
      System.out.println("ea [deg], T=1 [min], T=3 [min], T=5 [min]");
      for (double ea=1;ea <= 5; ea+=0.5){
        vo = Velocity.makeVxyz( 
            // Ownship: vx vy [kn], vz [fpm]
            200,0,"kn", 0,"fpm");
        vi = Velocity.makeVxyz( 
            // Traffic: vx vy [kn], vz [fpm]
            -200,0,"kn", 0,"fpm");

        eao = eai = Units.from("deg",ea);
        double eps = eso+esi;
        double evo = evx(vo,eao,ego);
        double evi = evx(vi,eai,egi);
        double epv  = evo+evi;
        t = Units.from("min",1);
        double psi1 = eps+t*epv;
        t = Units.from("min",3);
        double psi3 = eps+t*epv;
        t = Units.from("min",5);
        double psi5 = eps+t*epv;
        System.out.println(ea+", "+
            Units.to("nm",psi1)+", "+
            Units.to("nm",psi3)+", "+
            Units.to("nm",psi5));
      }
    } else if (kind == 2) {
      System.out.println("eg [knot], T=1 [min], T=3 [min], T=5 [min]");
      for (double eg=1;eg <= 10; eg+=1){
        vo = Velocity.makeVxyz( 
            // Ownship: vx vy [kn], vz [fpm]
            200,0,"kn", 0,"fpm");
        vi = Velocity.makeVxyz( 
            // Traffic: vx vy [kn], vz [fpm]
            -200,0,"kn", 0,"fpm");
        ego = egi = Units.from("kn",eg);
        double eps = eso+esi;
        double evo = evx(vo,eao,ego);
        double evi = evx(vi,eai,egi);
        double epv  = evo+evi;
        t = Units.from("min",1);
        double psi1 = eps+t*epv;
        t = Units.from("min",3);
        double psi3 = eps+t*epv;
        t = Units.from("min",5);
        double psi5 = eps+t*epv;
        System.out.println(eg+", "+
            Units.to("nm",psi1)+", "+
            Units.to("nm",psi3)+", "+
            Units.to("nm",psi5));
      }
    } 
  }
}
