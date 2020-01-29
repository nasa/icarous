#include <iostream>
#include "PC_Quadcopter_Simulation.h"
#include "UtilFunctions.h"

void Publish(Position,Velocity){


}

int main(int argc,char** argv){

   double position[3] = {0.0,0.0,0.0}; 
   double velocity[3] = {0.0,0.0,0.0};
   
   PC_Quadcopter_Simulation_initialize();
   memset(PC_Quadcopter_Simulation_Y.yout,0,sizeof(real_T)*30);

   PC_Quadcopter_Simulation_U.Xd_I = 1;
   PC_Quadcopter_Simulation_U.Yd_I = 1;
   PC_Quadcopter_Simulation_U.Zd_I = 0;

   PC_Quadcopter_Simulation_step();

   Publish();
}
