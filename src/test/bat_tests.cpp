/*
 * Copyright (C) 2014 - 2016 Intel Corporation.
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

#include <fstream>
#include <algorithm>

#include "common.h"
#include "check.h"
#include "omp.h"
#include "memkind.h"
#include "trial_generator.h"

/* Set of basic acceptance tests for PREFERRED policy, the goal of this set of tests
 * is to prove that you can do incremental allocations of memory with different
 * sizes and that pages are actually allocated in HBW node.
 */
class BATest: public TGTest
{
};


TEST_F(BATest, TC_Memkind_HBW_Pref_CheckAvailable)
{
    ASSERT_EQ(0, hbw_check_available());
}

TEST_F(BATest, TC_Memkind_HBW_Pref_Policy)
{
    hbw_set_policy(HBW_POLICY_PREFERRED);
    EXPECT_EQ(HBW_POLICY_PREFERRED, hbw_get_policy());
}

TEST_F(BATest, TC_Memkind_HBW_Pref_MallocIncremental)
{
    tgen->generate_incremental(HBW_MALLOC);
    tgen->run(num_bandwidth, bandwidth);
}

TEST_F(BATest, TC_Memkind_HBW_Pref_CallocIncremental)
{
    tgen->generate_incremental(HBW_CALLOC);
    tgen->run(num_bandwidth, bandwidth);
}


TEST_F(BATest, TC_Memkind_HBW_Pref_ReallocIncremental)
{
    tgen->generate_incremental(HBW_REALLOC);
    tgen->run(num_bandwidth, bandwidth);
}

TEST_F(BATest, TC_Memkind_HBW_Pref_MemalignIncremental)
{
    tgen->generate_incremental(HBW_MEMALIGN);
    tgen->run(num_bandwidth, bandwidth);
}

TEST_F(BATest, TC_Memkind_HBW_Pref_MemalignPsizeIncremental)
{
    tgen->generate_incremental(HBW_MEMALIGN_PSIZE);
    tgen->run(num_bandwidth, bandwidth);
}

TEST_F(BATest, TC_Memkind_HBW_Pref_MallocRecycle)
{
    tgen->generate_recycle_incremental(MEMKIND_MALLOC);
    tgen->run(num_bandwidth, bandwidth);
}

TEST_F(BATest, TC_Memkind_HBW_Pref_MallocRecyclePsize)
{
    tgen->generate_recycle_psize_incremental(MEMKIND_MALLOC);
    tgen->run(num_bandwidth, bandwidth);
}
