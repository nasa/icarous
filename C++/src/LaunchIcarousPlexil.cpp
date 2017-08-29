#include "Icarous.h"
#include "IpcIcarousAdapter.h"

int main(int argc,char* argv[]){
    

	Icarous_t ICAROUS(argc,argv,NULL);
	std::string centralhost("localhost:1381");
	IpcIcarousAdapter ipcAdapter(centralhost,&ICAROUS);
	ICAROUS.RunWithPlexil();
    return 0;
}

