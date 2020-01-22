#include "Icarous_msg.h"

static int ReadFlightplanFromFile(char filename[50],flightplan_t *plan){
   
   FILE *fp = fopen(filename,"r");
  
   if (fp == NULL){
       return -1;
   }

   char *buffer;
   size_t size = 250; 
   int n = 1;

   buffer = ((char*) malloc(size));

   int index = -1;
   int count = 0;

   while(n > 0){
       n = getline(&buffer,&size,fp);

       if(n < 20){
           continue;
       }

       count = 0;
       index++;

       if (buffer[0] == '#'){
           continue;
       }else{
           char* tok;
           tok = strtok(buffer," ,\t");
           int command = 16;
           while(tok != NULL){
               switch(count){
                    case 0:{
                            // index
                            //printf("index: %d\n",atoi(tok));
                            plan->num_waypoints++;
                            break;
                          }

                    case 1:{
                             // current wp
                             break;
                          }

                    case 2:{

                            // coord frame
                            break;
                           }

                    case 3:{
                            // command
                            command = atoi(tok);
                            //printf("command: %d\n",command);
                            break;
                           }

                    case 4:{
                            // param1
                            break;
                           }

                    case 5:{
                            // param2
                            break;
                           }

                    case 6:{
                            // param3
                            break;
                           }

                    case 7:{
                            // param4
                            int param4 = atoi(tok);
                            //printf("Param 4: %d\n",param4);
                            char wp_name[MAX_FIX_NAME_SIZE];
                            sprintf(wp_name,"%d",param4);
                            memcpy(plan->waypoints[index].name,wp_name,MAX_FIX_NAME_SIZE);
                            break;
                           }
                    
                    case 8:{
                            // x
                            double x = atof(tok);
                            //printf("x: %f\n",x);
                            plan->waypoints[index].latitude = x;
                            break;
                           }

                    case 9:{
                            // y
                            double y = atof(tok);
                            //printf("y: %f\n",y); 
                            plan->waypoints[index].longitude = y;
                            break;
                           }

                    case 10:{
                            // z
                            double z = atof(tok);
                            //printf("z: %f\n",z);
                            plan->waypoints[index].altitude = z;
                            break;
                            }

                    case 11:{
                            // auto continue
                            break;
                            }

                    default:{
                                break;
                            }

               }
               count++;
               tok = strtok(NULL," \t");
               if(command != 16){
                   index--;
                   break;
               }
           }
       }
    }

    return 0;
}