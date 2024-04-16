#include "../adaptive_fmm_tree.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <random>

#include <unistd.h>
#include <stdlib.h>

#include <omp.h>

using namespace fmm;

int main(int argc, char *argv[]) {

    // start init timer
    const auto init_start = std::chrono::steady_clock::now();

    const size_t N = 10000;
    const size_t items_per_leaf = 50; 
    const size_t d = 2;
    const double eps = 1E1; 
    const double extent = 32;

    using Vec = Vector_<d>;
    using Src = PointSource_<d>;

    const bool field_type = 1; // gravitational   

    std::vector<Src> sources;
    for(size_t i = 0; i < N; i++) {
        Vec v;      
        for(size_t j = 0; j < d; ++j) {
            v[j] =  extent * ((double) rand() / (RAND_MAX)) - extent/2;
        }
        double q = (double) rand() / (RAND_MAX) * (i % 2 ? 1 : -1);
        sources.push_back({v,q}); 
    }

    // end init timer
    const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
    std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';


    // start comp timer
    const auto compute_start = std::chrono::steady_clock::now();

    AdaptiveFmmTree<d, field_type> t(sources, items_per_leaf, eps);  
    Vec eval_point = sources[0].position; 

    // end comp timer
    const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();
    std::cout << "Computation time (sec): " << compute_time << '\n';

    std::cout << "Potential at " << eval_point << " evaluated via the FMM is " << 
       t.evaluatePotential(eval_point) << " vs " 
       << fields::potential<d, field_type>(sources, eval_point) 
       << " when evaluated directly. \n"; 
      



}

