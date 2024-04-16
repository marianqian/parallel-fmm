# Parallel Fast Multipole Method (FMM) on Distributed Memory
### Team Members: Nicole Feng (nvfeng), Marian Qian (marianq)

## URL 
https://github.com/marianqian/parallel-fmm 

## Summary
We are going to implement a parallelized Fast Multipole Method on distributed machines, using MPI for distributed memory parallelism and utilizing OpenMP and CUDA acceleration for shared memory parallelism. We will run our implementation on the NVIDIA GPUs in the lab and the PSC GPU Nodes to test the scalability and performance of our solution, and analyze the effectiveness of our algorithm on different problem sizes and particle distributions.

## Background

The N-body problem is foundational in understanding the movement of celestial bodies, and is widely used in physics and mathematics for a diverse set of problems. In its purest mathematical form, the N-body problem comes down to an O(N^2) computation (see Figure 1) of all pairwise interactions between N particles, a complexity that becomes impossibly expensive when working on problem sizes that are large enough to be practical.


Figure 1: N-body problem calculation.

The Fast Multipole Method brings the complexity down to a linear time O(Nlog(1/ε)^3) in 3 dimensions, where ε is the tolerance. It does this by:

1. Building an octree so that any leaf node (or octant) has approximately a certain number of particles.
2. Evaluating the sum using tree traversals:
- Performing a post-order (bottom-up) traversal involving the octant and its children — this upward pass computes multipole expansions.
- Performing a pre-order (top-down) traversal involving several octants in a neighborhood around the octant being visited — this downward pass converts the multipole expansions to inner expansions.

The tree traversals can benefit from parallelization since for each level, we can take advantage of data parallelism across the octants in that level. In the worst case, we have a O(log N) chain of dependencies that can’t be parallelized. However, this is only the case when the octree is evenly distributed. 

For nonuniform particle distributions, where the octree is highly skewed with varying leaf levels, the parallel scalability and complexity analysis becomes more difficult. Some techniques to work against the nonuniform distribution of the tree involve using local essential trees and a parallel tree construction algorithm. Previous work also involved building a communication graph to represent the octree; each node represented a tree octant and edges represented the interactions between octants. These communication graphs are bounded by O(N^⅔ (log N)^4/3), and with a good sorting algorithm such as parallel radix sort to distribute the particles, the overall algorithm scales in O(N/p) + O(log p), where p is the number of processors (which is the same overall scalable complexity as if we had uniform particle distributions). These previous works show that it is theoretically possible to parallelize FMM for any distribution (both uniform and nonuniform distributions). 


## The Challenge

FMM is hard to parallelize efficiently on distributed memory computers because of irregular patterns (especially when there is a nonuniform distribution of particles) and the high volume of communications. 

### What We Hope to Learn:
From this project, we hope to learn how to integrate different types of parallelism (across distributed memory and within shared memory) to speed up FMM calculations across the octree (so during the bottom-up and top-down traversals) and within octants of the octree (such as constructing the interactions within each leaf octant). 

### Workload:
The tree traversals means that we access the data in a way that’s not necessarily optimized for cache locality, and can seem irregular from the particles’ partitions. 

When particles interact, that computation needs to be passed through different portions of the octree, which can also mean it needs to be passed between different memories since the particles would be partitioned over distributed memory. Thus, there is a lot of communication that needs to happen when executing on distributed memory computers, which can limit gains from parallelization due to this overhead. 

In addition, if we are to write FMM that is scalable for nonuniform particle distributions, we also need to find a way to partition the particles so that they are balanced within the octree. 

### Constraints:
As mentioned previously, the structure of an octree, especially if the octree is uneven, makes the FMM difficult to parallelize. Dependencies within the octree for certain computations means that our implementation would either have to efficiently communicate information between processors or balance work in a way that removes these dependencies between processors. 

## Resources

### Computing Resources: 
We will use GHC machines for testing and debugging serial, and parallelized versions of our algorithm (OpenMP and MPI).
We will use GHC machines (NVIDIA GeForce RTX 2080 B GPUs) for when we incorporate CUDA parallelization into our algorithm.
We will use PSC machines with GPU nodes for testing the scalability of our parallelized implementation (OpenMP, MPI, CUDA).

### Algorithmic Resources:
- 2006 context paper: https://www.tandfonline.com/doi/full/10.1080/08927020600991161
J. Kurzak and B. M. Pettitt. “Fast multipole methods for particle dynamics”. Mol Simul. 2006 ; 32(10-11): 775–790. doi:10.1080/08927020600991161.
- 2005 paper talking about how to parallelize in distributed memory: https://www.sciencedirect.com/science/article/pii/S0743731505000249
“Massively parallel implementation of a fast multipole method for distributed memory machines”
- 2012 paper with combining parallel models (MPI, OpenMP, GPU): https://cims.nyu.edu/gcl/papers/lashuk2013mpa.pdf 
“A Massively Parallel Adaptive Fast Multipole Method on Heterogeneous Architectures”

### Codebase:
- FMM C++ Implementation parallelized with OpenMP: https://github.com/jrotheneder/FMM

We plan to use the repo listed under "Codebase" to gather data and measurements for sequential and OpenMP implementations. Then we plan to parallelize the codebase using CUDA instead of OpenMP, and choose one as an accelerant to use in conjunction with MPI for distributed memory. 



## Goals and Deliverables 

### Plan to Achieve:
- Correct implementation of FMM in C++, parallelized using OpenMP and CUDA to exploit data parallelism within a shared memory.
- Correct implementation of FMM in C++, parallelized using MPI to make the implementation adaptable and scalable across distributed machines/memory.
- We aim to have our parallel implementations at least exhibit some sort of speedup compared to the sequential implementation, perhaps almost linear speedup for up to 8 cores (7x speedup for 8 cores) — as this is what they achieved in the paper.
- We aim to have our MPI version perform better (speedup) on non-uniformly distributed particle data compared to the implementation that just has parallelism in the shared memory space (CUDA, OpenMP).

### Hope to Achieve:
- We hope to achieve an extremely scalable solution that has good speedup on larger core counts (up to 128), maybe around 64x speedup (just extrapolating from the speedup numbers in the paper).

### Poster Session Demo:
- For the poster session demo, we plan to show our speedup graphs — one for a shared memory parallelization implementation (OpenMP and CUDA), and one for a combined shared and distributed memory approach (MPI, OpenMP, and CUDA). 
- We will also show speedup graphs for our 2 implementations on different problem sizes and particle distributions, so we can demonstrate the benefits of a heterogeneous architecture (distributed memory) in FMMs with uneven particle distributions.


## Platform Choice 

We plan to implement our algorithm in C++ so that we can easily rewrite it using CUDA for GPU parallelization, as well as implement MPI and OpenMP calls like we’ve done in previous class assignments. 

The GHC machines with the NVIDIA GPUs are a good platform to test our code on because they can handle CUDA, MPI, and OpenMP for small problem sizes that will allow us to debug for correctness and perform basic analysis on synchronization costs and how well our solution scales. 

Since FMM is necessary due to the large problem sizes it is used on in practice, using the PSC machines and PSC GPU nodes makes sense since we will be able to see how well our solution scales on larger problem sizes.

## Progress 
So far, we have a sequential and OpenMP parallelized versions of the FMM. This FMM can be used in multiple configurations—for gravitational and Coulombic interactions, in 2D and 3D, and for uniform and non-uniform particle distributions. Additionally, we have begun parallelizing the FMM using CUDA instead of OpenMP, in hopes of choosing the better parallelization strategy to use in conjunction with MPI in the future. 

Originally, we had planned to implement a sequential version of the FMM ourselves from scratch by rewriting existing python code for the algorithm in C++. However, it took us a while to understand exactly how FMM works in terms of the algorithmic and mathematical details. We had a hard time digesting the primary research paper we were going off of (A Massively Parallel Adaptive Fast Multipole Method on Heterogeneous Architectures)[https://cims.nyu.edu/gcl/papers/lashuk2013mpa.pdf], and had to read through many supplemental materials and watch videos in order to get a better understanding of how exactly FMMs work and how the formulas, algorithms and data structures are built to accomplish this. 

When we finally started to write our sequential code, we soon realized that it would take a lot of time and effort to write modular and clean C++ code for implementing the FMM, when this was not even the main portion or intent of our project. We wanted to be able to focus on the real challenge, which was focusing on how to parallelize the FMM for distributed memory using MPI. Of course, parallelizing the FMM using OpenMP and CUDA would also be a significant portion of the project, but this parallelization is more straightforward than distributed memory parallelization due to the fairly obvious parallelizable portions of the algorithm. As such, we decided to pivot and use an existing C++ codebase that already implemented the FMM with OpenMP optimizations. We stripped it down to exclude any OpenMP directives and used that as our sequential code, and then put the OpenMP optimizations back in and used that as our OpenMP code.

Now, our efforts will mainly be focused on optimizing the FMM using CUDA instead of OpenMP, and then incorporating MPI to parallelize it for distributed memory. Then, we plan to perform a thorough analysis on the performance benefits of our various parallelization strategies.

## Schedule

### Thur, 4/20
- Finish setting up CUDA/test run code for CUDA.
### Fri, 4/19
- Finish implementing CUDA version of code. 
### Sat, 4/20
- Record performance metrics on OpenMP and make speedup graphs (include problem size and particle distribution sensitivity studies). 
- Record performance metrics on CUDA and make speedup graphs (include problem size and particle distribution sensitivity studies). 
- Create a detailed outline for MPI integration, following the paper. 
### Mon, 4/22
- Finish one iteration of MPI implementation on GHC machines.
### Wed, 4/24
- Finish incorporating MPI parallelization into implementation. 
- Record performance metrics on MPI and (include problem size and particle distribution sensitivity studies).
### Sat, 4/27
- If speedup is not ideal on GHC, do performance debugging to identify bottlenecks.
- Collect and record performance metrics on PSC machines to see if the solution is scalable. If not, do performance debugging to identify bottlenecks.
### Wed, 5/1
- Finish iterating on algorithm design. 
### Sat, 5/4
- Finish collecting all finalized performance metrics for the report.
### Sun, 5/5
- Final Report DUE
### Mon, 5/6
- Poster Session DUE
