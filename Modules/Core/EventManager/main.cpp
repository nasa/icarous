#include "EventManager.hpp"
#include <iostream>

int main(int argc,char** argv){
   int x = 5;
   EventManagement<int> obj;
   obj.Run(&x);
   std::cout<<"end"<<std::endl;

}