#include "Icarous.h"
#include "Demo.h"

int main(int argc,char* argv[]){
    
	Demo_t Sample;
	Icarous_t ICAROUS(argc,argv,&Sample);

	ICAROUS.Run();

    return 0;
}

