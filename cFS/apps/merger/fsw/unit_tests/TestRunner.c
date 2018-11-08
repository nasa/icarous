//
// Created by swee on 7/22/18.
//

#include "uttest.h"
#include "merger_test.h"

int main(void){

    UtTest_Add(Test_ComputeEntryPoint, Test_Setup, Test_TearDown, "Test_ComputeEntryPoint");
    UtTest_Add(Test_ComputeArrivalData, Test_Setup, Test_TearDown, "Test_ComputeArrivalData");
    UtTest_Add(Test_CheckIntersectionConflict, Test_Setup, Test_TearDown, "Test_CheckIntersectionConflict");

    return(UtTest_Run());
}


