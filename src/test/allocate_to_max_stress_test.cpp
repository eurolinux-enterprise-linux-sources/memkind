/*
 * Copyright (C) 2015 - 2017 Intel Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice(s),
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice(s),
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <chrono>
#include "common.h"
#include "allocator_perf_tool/Configuration.hpp"
#include "allocator_perf_tool/StressIncreaseToMax.h"
#include "allocator_perf_tool/HugePageOrganizer.hpp"

//memkind stress and longevity tests using Allocatr Perf Tool.
class AllocateToMaxStressTests: public :: testing::Test
{

protected:
    void SetUp()
    {}

    void TearDown()
    {}

    //Allocates memory up to 'memory_request_limit'.
    void run(unsigned kind, unsigned operations, unsigned size_from, unsigned size_to, size_t memory_request_limit)
    {
        RecordProperty("kind", AllocatorTypes::allocator_name(kind));
        RecordProperty("memory_operations", operations);
        RecordProperty("size_from", size_from);
        RecordProperty("size_to", size_to);

        TaskConf task_conf = {
            .n = operations, //number of memory operations
            .allocation_sizes_conf = {
                operations, //number of memory operations
                size_from, //no random sizes.
                size_to
            },
            .func_calls = TypesConf(FunctionCalls::MALLOC), //enable allocator function call
            .allocators_types = TypesConf(kind), //enable allocator
            .seed = 11 //random seed
        };

        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        //Execute test iterations.
        std::vector<iteration_result> results = StressIncreaseToMax::execute_test_iterations(task_conf, 120, memory_request_limit);

        end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_time = end - start;

        RecordProperty("elapsed_time", elapsed_time.count());

        //Check finish status.
        EXPECT_EQ(check_allocation_errors(results, task_conf), 0);
    }

    //Check true allocation errors over all iterations.
    //Return iteration number (>0) when error occurs, or zero
    int check_allocation_errors(std::vector<iteration_result>& results, const TaskConf& task_conf)
    {
        for (size_t i=0; i<results.size(); i++)
        {
            //Check if test ends with allocation error.
           if(results[i].is_allocation_error)
           {
                return i+1;
           }
        }

        return 0;
    }

private:
    HugePageOrganizer huge_page_organizer = HugePageOrganizer(4500);
};

TEST_F(AllocateToMaxStressTests, test_TC_MEMKIND_slts_ALLOCATE_TO_MAX_MEMKIND_HBW)
{
    run(AllocatorTypes::MEMKIND_HBW, 1024, MB, MB, GB);
}

TEST_F(AllocateToMaxStressTests, test_TC_MEMKIND_slts_ALLOCATE_TO_MAX_MEMKIND_INTERLEAVE)
{
    run(AllocatorTypes::MEMKIND_INTERLEAVE, 4096, MB, MB, 4*GB);
}

TEST_F(AllocateToMaxStressTests, test_TC_MEMKIND_slts_ALLOCATE_TO_MAX_MEMKIND_HBW_PREFERRED)
{
    run(AllocatorTypes::MEMKIND_HBW_PREFERRED, 17408, MB, MB, 17*GB);
}

TEST_F(AllocateToMaxStressTests, test_TC_MEMKIND_2MBPages_slts_ALLOCATE_TO_MAX_MEMKIND_HBW_HUGETLB)
{
    run(AllocatorTypes::MEMKIND_HBW_HUGETLB, 1024, 4*MB, 4*MB, GB);
}

TEST_F(AllocateToMaxStressTests, test_TC_MEMKIND_slts_ALLOCATE_TO_MAX_DIFFERENT_SIZES)
{
    run(AllocatorTypes::MEMKIND_HBW, 2500, 1, 8*MB, GB);
}
