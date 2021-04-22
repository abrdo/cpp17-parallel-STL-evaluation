////////////////////////////////     Forditas, futtatas:     /////////////////////////////////////////////////
// Makefile-al:
//  make -B sort_cpu
//  make -B sort_gpu



// ------GPU---------
/*
    salloc -pgpu2 --nodelist=neumann srun --pty --preserve-env /bin/bash -l
    module load gpu/cuda/11.0rc
    module load nvhpc/20.9
    nvc++ -I/home/shared/software/cuda/hpc_sdk/Linux_x86_64/20.9/compilers/include-stdpar         vector_copy_gpu.cpp -std=c++11 -O3 -o gpu_test -stdpar
    ./gpu_test

        // GPU Summary:
        nvprof --print-gpu-summary 
*/

// ------CPU---------
// icpc vector_copy.cpp -std=c++11 -ltbb -qopenmp-simd -O3 -xHOST
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "sortByLocationsApp.hpp"
#include "LocChangeHandlingApp.hpp"
#include "../include/printers.h"



int main(void){
    std::ofstream file("times/locChanges/___times_locChHandel_100000.txt");
    std::vector<int> times_manualUpdate, times_sortAgain;
    LocChangeHandlingApp app;
    
    app.run(times_manualUpdate, times_sortAgain);
    
    //printer::to_file(app.get_range(), file, "range = ");
    printer::to_file(times_manualUpdate, file, "times_manualUpdate = ");
    printer::to_file(times_sortAgain, file, "times_sortAgain = ");

    file.close();
    return 0;
}