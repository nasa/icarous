

#include "Icarous.h"

class Demo_t:public Mission_t{
public:
	void Execute(FlightManagementSystem_t* fms){};
};

int main(int argc,char* argv[]){
    
	Demo_t Sample;
	Icarous_t ICAROUS(argc,argv,&Sample);

	ICAROUS.Run();

    return 0;
}

