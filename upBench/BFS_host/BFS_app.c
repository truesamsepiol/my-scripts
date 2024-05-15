/**
* app.c
* BFS Host Application Source File
*
*/

#include "../header.h"

#include "mram-management.h"
#include "../BFS_support/common.h"
#include "../BFS_support/graph.h"
#include "../BFS_support/params.h"
#include "../BFS_support/timer.h"
#include "../BFS_support/utils.h"

#ifndef ENERGY
#define ENERGY 0
#endif
#if ENERGY
#include <dpu_probe.h>
#endif

#define DPU_BINARY "./BFS_bin/dpu_code"

// Main of the Host Application
void bfs(int argc, char *argv[]) {
    // Process parameters EO -> on our own parameters
    struct Params p = input_params(argc, argv);

    uint32_t numDPUs;
    //DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
    //DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
    DPU_ASSERT(dpu_get_nr_dpus(set, &numDPUs));
    PRINT_INFO(p.verbosity >= 1, "Allocated %d DPU(s)", numDPUs);

    // Initialize BFS data structures
    PRINT_INFO(p.verbosity >= 1, "Reading graph %s", p.fileName);
    struct COOGraph cooGraph = readCOOGraph(p.fileName);
    PRINT_INFO(p.verbosity >= 1, "    Graph has %d nodes and %d edges", cooGraph.numNodes, cooGraph.numEdges);
    struct CSRGraph csrGraph = coo2csr(cooGraph);
    uint32_t numNodes = csrGraph.numNodes;
    uint32_t* nodePtrs = csrGraph.nodePtrs;
    uint32_t* neighborIdxs = csrGraph.neighborIdxs;
    uint32_t* nodeLevel = calloc(numNodes, sizeof(uint32_t)); // Node's BFS level (initially all 0 meaning not reachable)
    uint64_t* visited = calloc(numNodes/64, sizeof(uint64_t)); // Bit vector with one bit per node
    uint64_t* currentFrontier = calloc(numNodes/64, sizeof(uint64_t)); // Bit vector with one bit per node
    uint64_t* nextFrontier = calloc(numNodes/64, sizeof(uint64_t)); // Bit vector with one bit per node
    setBit(nextFrontier[0], 0); // Initialize frontier to first node
    uint32_t level = 1;

    // Partition data structure across DPUs
    uint32_t numNodesPerDPU = ROUND_UP_TO_MULTIPLE_OF_64((numNodes - 1)/numDPUs + 1);
    PRINT_INFO(p.verbosity >= 1, "Assigning %u nodes per DPU", numNodesPerDPU);
    struct DPUParams dpuParams[numDPUs];
    uint32_t dpuParams_m[numDPUs];
    unsigned int dpuIdx = 0;
    // EO -> I move the loop and I adapt for each dpus
    //DPU_FOREACH (set, dpu) {
        // Allocate parameters
        struct mram_heap_allocator_t allocator;
        init_allocator(&allocator);
        dpuParams_m[dpuIdx] = mram_heap_alloc(&allocator, sizeof(struct DPUParams));

        // Find DPU's nodes
        uint32_t dpuStartNodeIdx = dpuIdx*numNodesPerDPU;
        uint32_t dpuNumNodes;
        if(dpuStartNodeIdx > numNodes) {
            dpuNumNodes = 0;
        } else if(dpuStartNodeIdx + numNodesPerDPU > numNodes) {
            dpuNumNodes = numNodes - dpuStartNodeIdx;
        } else {
            dpuNumNodes = numNodesPerDPU;
        }
        dpuParams[dpuIdx].dpuNumNodes = dpuNumNodes;
        PRINT_INFO(p.verbosity >= 2, "    DPU %u:", dpuIdx);
        PRINT_INFO(p.verbosity >= 2, "        Receives %u nodes", dpuNumNodes);

        // Partition edges and copy data
        if(dpuNumNodes > 0) {

            // Find DPU's CSR graph partition
            uint32_t* dpuNodePtrs_h = &nodePtrs[dpuStartNodeIdx];
            uint32_t dpuNodePtrsOffset = dpuNodePtrs_h[0];
            uint32_t* dpuNeighborIdxs_h = neighborIdxs + dpuNodePtrsOffset;
            uint32_t dpuNumNeighbors = dpuNodePtrs_h[dpuNumNodes] - dpuNodePtrsOffset;
            uint32_t* dpuNodeLevel_h = &nodeLevel[dpuStartNodeIdx];

            // Allocate MRAM
            uint32_t dpuNodePtrs_m = mram_heap_alloc(&allocator, (dpuNumNodes + 1)*sizeof(uint32_t));
            uint32_t dpuNeighborIdxs_m = mram_heap_alloc(&allocator, dpuNumNeighbors*sizeof(uint32_t));
            uint32_t dpuNodeLevel_m = mram_heap_alloc(&allocator, dpuNumNodes*sizeof(uint32_t));
            uint32_t dpuVisited_m = mram_heap_alloc(&allocator, numNodes/64*sizeof(uint64_t));
            uint32_t dpuCurrentFrontier_m = mram_heap_alloc(&allocator, dpuNumNodes/64*sizeof(uint64_t));
            uint32_t dpuNextFrontier_m = mram_heap_alloc(&allocator, numNodes/64*sizeof(uint64_t));
            PRINT_INFO(p.verbosity >= 2, "        Total memory allocated is %d bytes", allocator.totalAllocated);

            // Set up DPU parameters
            dpuParams[dpuIdx].numNodes = numNodes;
            dpuParams[dpuIdx].dpuStartNodeIdx = dpuStartNodeIdx;
            dpuParams[dpuIdx].dpuNodePtrsOffset = dpuNodePtrsOffset;
            dpuParams[dpuIdx].level = level;
            dpuParams[dpuIdx].dpuNodePtrs_m = dpuNodePtrs_m;
            dpuParams[dpuIdx].dpuNeighborIdxs_m = dpuNeighborIdxs_m;
            dpuParams[dpuIdx].dpuNodeLevel_m = dpuNodeLevel_m;
            dpuParams[dpuIdx].dpuVisited_m = dpuVisited_m;
            dpuParams[dpuIdx].dpuCurrentFrontier_m = dpuCurrentFrontier_m;
            dpuParams[dpuIdx].dpuNextFrontier_m = dpuNextFrontier_m;

            // Send data to DPU
            PRINT_INFO(p.verbosity >= 2, "        Copying data to DPU");
            copyToDPU(dpu, (uint8_t*)dpuNodePtrs_h, dpuNodePtrs_m, (dpuNumNodes + 1)*sizeof(uint32_t));
            copyToDPU(dpu, (uint8_t*)dpuNeighborIdxs_h, dpuNeighborIdxs_m, dpuNumNeighbors*sizeof(uint32_t));
            copyToDPU(dpu, (uint8_t*)dpuNodeLevel_h, dpuNodeLevel_m, dpuNumNodes*sizeof(uint32_t));
            copyToDPU(dpu, (uint8_t*)visited, dpuVisited_m, numNodes/64*sizeof(uint64_t));
            copyToDPU(dpu, (uint8_t*)nextFrontier, dpuNextFrontier_m, numNodes/64*sizeof(uint64_t));
            // NOTE: No need to copy current frontier because it is written before being read

        }

        // Send parameters to DPU
        PRINT_INFO(p.verbosity >= 2, "        Copying parameters to DPU");
        copyToDPU(dpu, (uint8_t*)&dpuParams[dpuIdx], dpuParams_m[dpuIdx], sizeof(struct DPUParams));

        ++dpuIdx;

    //}
//EO -> end for each dpus

	/*
    // Iterate until next frontier is empty
    uint32_t nextFrontierEmpty = 0;
    while(!nextFrontierEmpty) {

        PRINT_INFO(p.verbosity >= 1, "Processing current frontier for level %u", level);

        // Run all DPUs
        PRINT_INFO(p.verbosity >= 1, "    Booting DPUs");
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));



        // Copy back next frontier from all DPUs and compute their union as the current frontier
        dpuIdx = 0;
        DPU_FOREACH (set, dpu) {
            uint32_t dpuNumNodes = dpuParams[dpuIdx].dpuNumNodes;
            if(dpuNumNodes > 0) {
                if(dpuIdx == 0) {
                    copyFromDPU(dpu, dpuParams[dpuIdx].dpuNextFrontier_m, (uint8_t*)currentFrontier, numNodes/64*sizeof(uint64_t));
                } else {
                    copyFromDPU(dpu, dpuParams[dpuIdx].dpuNextFrontier_m, (uint8_t*)nextFrontier, numNodes/64*sizeof(uint64_t));
                    for(uint32_t i = 0; i < numNodes/64; ++i) {
                        currentFrontier[i] |= nextFrontier[i];
                    }
                }
                ++dpuIdx;
            }
        }

        // Check if the next frontier is empty, and copy data to DPU if not empty
        nextFrontierEmpty = 1;
        for(uint32_t i = 0; i < numNodes/64; ++i) {
            if(currentFrontier[i]) {
                nextFrontierEmpty = 0;
                break;
            }
        }
        if(!nextFrontierEmpty) {
            ++level;
            dpuIdx = 0;
            DPU_FOREACH (set, dpu) {
                uint32_t dpuNumNodes = dpuParams[dpuIdx].dpuNumNodes;
                if(dpuNumNodes > 0) {
                    // Copy current frontier to all DPUs (place in next frontier and DPU will update visited and copy to current frontier)
                    copyToDPU(dpu, (uint8_t*)currentFrontier, dpuParams[dpuIdx].dpuNextFrontier_m, numNodes/64*sizeof(uint64_t));
                    // Copy new level to DPU
                    dpuParams[dpuIdx].level = level;
                    copyToDPU(dpu, (uint8_t*)&dpuParams[dpuIdx], dpuParams_m[dpuIdx], sizeof(struct DPUParams));
                    ++dpuIdx;
                }
            }
        }
    }

    // Copy back node levels
    PRINT_INFO(p.verbosity >= 1, "Copying back the result");
    dpuIdx = 0;
    DPU_FOREACH (set, dpu) {
        uint32_t dpuNumNodes = dpuParams[dpuIdx].dpuNumNodes;
        if(dpuNumNodes > 0) {
            uint32_t dpuStartNodeIdx = dpuIdx*numNodesPerDPU;
            copyFromDPU(dpu, dpuParams[dpuIdx].dpuNodeLevel_m, (uint8_t*)(nodeLevel + dpuStartNodeIdx), dpuNumNodes*sizeof(float));
        }
        ++dpuIdx;
    }

    // Calculating result on CPU
    PRINT_INFO(p.verbosity >= 1, "Calculating result on CPU");
    uint32_t* nodeLevelReference = calloc(numNodes, sizeof(uint32_t)); // Node's BFS level (initially all 0 meaning not reachable)
    memset(nextFrontier, 0, numNodes/64*sizeof(uint64_t));
    setBit(nextFrontier[0], 0); // Initialize frontier to first node
    nextFrontierEmpty = 0;
    level = 1;
    while(!nextFrontierEmpty) {
        // Update current frontier and visited list based on the next frontier from the previous iteration
        for(uint32_t nodeTileIdx = 0; nodeTileIdx < numNodes/64; ++nodeTileIdx) {
            uint64_t nextFrontierTile = nextFrontier[nodeTileIdx];
            currentFrontier[nodeTileIdx] = nextFrontierTile;
            if(nextFrontierTile) {
                visited[nodeTileIdx] |= nextFrontierTile;
                nextFrontier[nodeTileIdx] = 0;
                for(uint32_t node = nodeTileIdx*64; node < (nodeTileIdx + 1)*64; ++node) {
                    if(isSet(nextFrontierTile, node%64)) {
                        nodeLevelReference[node] = level;
                    }
                }
            }
        }
        // Visit neighbors of the current frontier
        nextFrontierEmpty = 1;
        for(uint32_t nodeTileIdx = 0; nodeTileIdx < numNodes/64; ++nodeTileIdx) {
            uint64_t currentFrontierTile = currentFrontier[nodeTileIdx];
            if(currentFrontierTile) {
                for(uint32_t node = nodeTileIdx*64; node < (nodeTileIdx + 1)*64; ++node) {
                    if(isSet(currentFrontierTile, node%64)) { // If the node is in the current frontier
                        // Visit its neighbors
                        uint32_t nodePtr = nodePtrs[node];
                        uint32_t nextNodePtr = nodePtrs[node + 1];
                        for(uint32_t i = nodePtr; i < nextNodePtr; ++i) {
                            uint32_t neighbor = neighborIdxs[i];
                            if(!isSet(visited[neighbor/64], neighbor%64)) { // Neighbor not previously visited
                                // Add neighbor to next frontier
                                setBit(nextFrontier[neighbor/64], neighbor%64);
                                nextFrontierEmpty = 0;
                            }
                        }
                    }
                }
            }
        }
        ++level;
    }
*/
    // Deallocate data structures
    freeCOOGraph(cooGraph);
    freeCSRGraph(csrGraph);
    free(nodeLevel);
    free(visited);
    free(currentFrontier);
    free(nextFrontier);
}

