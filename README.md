# Parallel Fast Multipole Method (FMM) on Shared Memory
### Team Members: Nicole Feng (nvfeng), Marian Qian (marianq)

## URL 
https://github.com/marianqian/parallel-fmm 

## Summary
We analyzed and improved the parallelization of the Fast Multipole Method (to solve the N-body problem) using OpenMP for shared memory systems, making it comparable to parallelization on distributed memory systems. Our implementation achieves up to 7x speedup for 8-core GHC machines, and up to 83x speedup for 64-core PSC Bridges-2 machines. Our deliverables include graphs comparing speedup across different core counts, problem sizes, and particle distributions.

## Background
The N-body problem is foundational in understanding the movement of celestial bodies, and is widely used in physics and mathematics for a diverse set of problems. In its purest mathematical form, the N-body problem comes down to an O(N^2) computation (see Figure 1) of all pairwise interactions between N particles, a complexity that becomes impossibly expensive when working on problem sizes that are large enough to be practical.

The Fast Multipole Method (FMM) brings the complexity down to a linear time O(Nlog(1/ε)^3) in 3 dimensions, where ε is the tolerance. It does this by:

1. Building an octree so that any leaf node (or octant) has approximately a certain number of particles.
2. Evaluating the sum using tree traversals:
- Performing a post-order (bottom-up) traversal involving the octant and its children — this upward pass computes multipole expansions.
- Performing a pre-order (top-down) traversal involving several octants in a neighborhood around the octant being visited — this downward pass converts the multipole expansions to inner expansions.
3. Output force and energy potential calculations of each particle interaction 

The tree traversals that build the tree can benefit from parallelization since for each level, we can take advantage of data parallelism across the octants in that level. In the worst case, we have a O(log N) chain of dependencies that can’t be parallelized. However, this is only the case when the octree is evenly distributed. For nonuniform particle distributions, where the octree is highly skewed with varying leaf levels, the parallel scalability and complexity analysis becomes more difficult.

Specifically, the M2L phase of the downward pass takes up the majority of the computation time as the algorithm traverses through every level and every node of that level, then sums up all of the multipole to local expansions of the current node’s neighbors (or relevant other nodes that “interact” with the current node, whether that means they share a same parent or in the same level of the octree). The calculation of this expansion then has an additional two loops over the order of the expansion to get the final value. The M2L phase2 has 94.5 times more computations than the M2M phase and L2L phase combined. The P2P phase also takes up a decent amount of time; however this can be run in parallel with the other stages (as shown by the other direction of the arrow). Therefore, the M2L phase in the downward traversal of the tree holds the best opportunity to be parallelized. 

Data dependencies that exist within this algorithm mainly consist between stages of the algorithm that are listed in Figure 1. Each step numbered 1 to 5 must be executed one step after the other (approximate interactions: S2U → U2U → VLI + XLI → D2D → WLI + D2T) while steps that are labeled with a and b can be executed concurrently. This makes it harder to parallelize because each stage depends on the expansions that were computed in the previous stage, so there is no way to parallelize across stages. Another dependency exists between tree levels during the upward and downward traversals (computing multipole to multipole, multipole to local, and local to local expansions). In these individual stages, computation depends on the tree level before it or after it, which also makes parallelization difficult in this case. However, outside of the tree level dependencies, it is easier to find a way to parallelize the algorithm whether that is across all nodes within a level or parallelize computation of an expansion for a single node. 

The purpose of the FMM is to construct a tree such that locality is improved in the calculation of the N-body problem. Because the octree exists, evaluating potentials and forces are much quicker as spatial locality is taken advantage of by using information that is within the octants near the given point we want to calculate the potential of. 

The algorithm itself has poor temporal locality; every stage (multipole to multipole, multipole to local etc.) iterates through the same levels of the tree and often the same nodes within the tree. However, they are all spaced out across in time (as they are in different stages of the algorithm). The spatial locality within the stages is already pretty good because of how calculations are done node by node, and these calculations often use other nodes that are related to the given node (like being in the same level of having the same parents). Therefore, there is spatial locality in which nodes are accessed for these calculations. 

Data-parallel exists when we want to use the already constructed tree to evaluate potentials and forces of many different points. This would simply involve probing the tree in parallel, which does not have any dependencies. 

## The Challenge

FMM is hard to parallelize efficiently using OpenMP if we want to get the same performance benefits of a distributed memory for non-uniform particle distributions. Irregular patterns and a high volume of communications for nonuniform particle distributions makes it difficult for a straightforward OpenMP implementation to fully take advantage of parallelization.

### What We Hope to Learn:
From this project, we hope to learn how to analyze a complex algorithm for opportunities of parallelization, using profiling tools to be able to identify bottlenecks and limiters of speedup. We also want to gain a better understanding of how different architectures (OpenMP, MPI, and CUDA/GPU) provide different types of parallelizatin performance benefits, and learn how to discern which platform is best suited for different portions of the FMM algorithm.

### Workload:
The tree traversals means that we access the data in a way that’s not necessarily optimized for cache locality, and can seem irregular from the particles’ partitions. When particles interact, that computation needs to be passed through different portions of the octree. This portion may not be able to be parallelized using OpenMP due to data dependencies. However, within each level of the tree, particle interactions (multipole expansions) can be calculated in parallel. In addition, if we are to write FMM that is scalable for nonuniform particle distributions, we also need to find a way to partition the particles so that they are balanced within the octree. 

### Constraints:
As mentioned previously, the structure of an octree, especially if the octree is uneven, makes the FMM difficult to parallelize. Dependencies within the octree for certain computations means that our implementation would either have to efficiently communicate information between processors or balance work in a way that removes these dependencies between processors. 

## Resources

### Computing Resources: 
We will use GHC machines for testing and debugging serial, and OpenMP parallelized version of our algorithm.
We will use PSC machines with CPU nodes for testing the scalability of our parallelized implementation.

### Algorithmic Resources:
1. Ilya Lashuk, Aparna Chandramowlishwaran, Harper Langston, Tuan-Anh Nguyen, Rahul Sampath, Aashay Shringarpure, Richard Vuduc, Lexing Ying, Denis Zorin, and George Biros. 2012. A massively parallel adaptive fast multipole method on heterogeneous architectures. Commun. ACM 55, 5 (May 2012), 101–109. https://doi.org/10.1145/2160718.2160740
2. Kohnke B, Kutzner C, Beckmann A, et al. A CUDA fast multipole method with highly efficient M2L far field evaluation. The International Journal of High Performance Computing Applications. 2021;35(1):97-117. https://doi.org/10.1177/1094342020964857 
3. Pan, Xiao-Min & Pi, Wei-Chao. (2011). On OpenMP parallelization of the multilevel fast multipole algorithm. Progress In Electromagnetics Research, 112, 199-213. https://doi.org/10.2528/PIER10120802 
4. Kurzak, J., & Pettitt, B. M. (2005). Massively parallel implementation of a fast multipole method for distributed memory machines. Journal of Parallel and Distributed Computing, 65(7), 870-881. https://doi.org/10.1016/j.jpdc.2005.02.001 
5. Kurzak, J., & Pettitt, B. M. (2006). Fast multipole methods for particle dynamics. Molecular Simulation, 32(10–11), 775–790. https://doi.org/10.1080/08927020600991161 
6. Nakano, Aiichiro. Handwritten Notes on FMM Algorithm, aiichironakano.github.io/cs653/01-1FMM.pdf.
7. Nakano, Aiichiro. Slides on Fast Multipole Method, https://aiichironakano.github.io/cs653/01-1FMM.pdf.
8. Jrotheneder. (2024). Implementation of the fast multipole method (FMM) for the evaluation of potentials and force fields in gravitation/electrostatics problems. GitHub. https://github.com/jrotheneder/FMM.git 

### Codebase:
- FMM C++ Implementation parallelized with OpenMP: https://github.com/jrotheneder/FMM

We plan to use the repo listed under "Codebase" to gather data and measurements for sequential and OpenMP implementations. Then we plan to optimize the OpenMP to attain the same performance as an MPI implementation.

## Goals and Deliverables 

### Plan to Achieve:
- We aim to have our parallel implementation at least exhibit some sort of speedup compared to the sequential implementation, perhaps almost linear speedup for up to 8 cores (7x speedup for 8 cores) — as this is what they achieved in the paper.
- We aim to have our parallel FMM perform comparable or better speedup on non-uniformly distributed particle data compared to uniformly distributed data.

### Hope to Achieve:
- We hope to achieve an extremely scalable solution that has good speedup on larger core counts (up to 128), maybe around 64x speedup (just extrapolating from the speedup numbers in the paper).

### Poster Session Demo:
For the poster session demo, we plan to show our speedup graphs for our OpenMP parallel FMM for various problem sizes, particle distributions, and core counts.


## Platform Choice 

We plan to implement our algorithm in C++ so that we can easily optimize it using OpenMP calls like we’ve done in previous class assignments. 

The GHC machines are a good platform to test our code on because they can handle OpenMP for small problem sizes that will allow us to debug for correctness and perform basic analysis on synchronization costs and how well our solution scales. 

Since FMM is necessary due to the large problem sizes it is used on in practice, using the PSC machines makes sense since we will be able to see how well our solution scales on larger problem sizes.


## Schedule (from Milestone Onwards)

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
