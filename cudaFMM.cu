#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

// from local_expansion.hpp 

//kernel -- all threads run same code in a block/warp
// __shared__ can only be accessed by threads in same block 
__global__ void fmm_kernel(int num_nodes, int num_interact){ 
    int index = blockIdx.x * blockDim.x + threadIdx.x; 
    int node = blockIdx.x; 
    int interaction_node = blockDim.x; 
    int coefficient = threadIdx.x; 



    __syncthreads(); 






}



void FMMCuda(int num_nodes, int num_interact){
    const int threadsPerBlock = 10; 
    const int blocks = ((num_nodes * num_interact) + threadsPerBlock - 1) / threadsPerBlock; 
    // N = num interaction node list * num  nodes 

    fmm_kernel<<<blocks, threadsPerBlock>>>(num_nodes, num_interact); 


}

/*
#pragma omp parallel for
        for(std::size_t i = 0; i < n_nodes_at_depth; i++) {

            FmmNode& current_node = nodes[offset + i]; 

            this->localToLocal(current_node);
            this->multipoleToLocal(current_node);
            template<std::size_t d, bool field_type>
            template<typename FmmNode>
            void AbstractFmmTree<d, field_type>::multipoleToLocal(FmmNode& node) {
            
                std::vector<const ME*> incoming; //vector of incoming multipole expansions
                for(FmmNode* interaction_partner : node.interaction_list) {
                    incoming.push_back(&interaction_partner->multipole_expansion);  
                }

                if(incoming.size() > 0) {
                    node.local_expansion += LE(node.center, incoming); 
                    LocalExpansion<2>::LocalExpansion(const Vector_<2>& center, 
                        std::vector<const ME*> expansions): Super(center, expansions.at(0)->order) {
                    
                    for(const ME* me : expansions) { 
                        *this += LocalExpansion(center, *me); 
                        LocalExpansion<2>::LocalExpansion(const Vector_<2>& center, 
                            const ME& incoming): Super(center, incoming.order) {
                    
                        assert(incoming.order > 0); 
                    
                        this->coefficients = multipoleToLocal(incoming); 
                                                
                        std::vector<Complex> LocalExpansion<2>::multipoleToLocal(
                            const ME& incoming) const {

                        Complex z0 = incoming.center - this->center; // ME center rel. to this->center
                        std::vector<Complex> coefficients(this->order + 1); 

                        const tables::BinomialTable& binomial_table = Super::binomial_table; 
                        const tables::PowTable<Complex> z0_inv_pow_table(1./z0, this->order);   


                        coefficients[0] = incoming(0) * std::log(-z0); 
                        for(int k = 1; k <= this->order; ++k) {
                            double sign = k % 2 == 0 ? 1 : -1;  
                            coefficients[0] += sign * incoming(k) * z0_inv_pow_table(k); // [(4.18), 1] 
                        }

                        // Compute b_l for 1 <= l <= order
                        for(int l = 1; l <= this->order; ++l) {

                            Complex b_l = -incoming(0)/(double)l; 

                            for(int k = 1; k < this->order; ++k) { 
                                double sign = k % 2 == 0 ? 1 : -1;  
                                b_l += sign * incoming(k) * z0_inv_pow_table(k)   
                                    * binomial_table(l+k-1, k-1); // [(4.19), 1]
                            }

                            b_l *= z0_inv_pow_table(l); // [(4.19), 1]
                            coefficients[l] = b_l; 
                        }

                        return coefficients;
                        }
                    }
                    }
                }
                }
            }

        }

*/