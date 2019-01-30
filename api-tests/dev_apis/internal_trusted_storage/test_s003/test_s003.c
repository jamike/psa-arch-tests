/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_interfaces.h"
#include "val_target.h"
#include "test_s003.h"
#if ITS_TEST
#include "test_its_data.h"
#elif PS_TEST
#include "test_ps_data.h"
#endif

#define TEST_BUFF_SIZE 256
#define NUM_ITERATIONS 5
#define TEST_BASE_UID_VALUE 20

client_test_t test_s003_sst_list[] = {
    NULL,
    psa_sst_insufficient_space,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE];

int32_t psa_sst_insufficient_space(security_t caller)
{
    uint32_t status = PSA_SST_SUCCESS;
    psa_sst_uid_t uid;
    uint32_t count = 0, results[NUM_ITERATIONS] = {0};
    int i = 0;

    /* Saturate the storage for NUM_ITERATION times, and remove them after */
    for (i = 0 ; i < NUM_ITERATIONS; i++)
    {
        val->print(PRINT_TEST, "[Check %d] Overload storage space\n", i + 1 );
        for (uid = TEST_BASE_UID_VALUE; status == PSA_SST_SUCCESS; uid++)
        {
            val->print(PRINT_INFO, "Setting 0x%x bytes for ", TEST_BUFF_SIZE);
            val->print(PRINT_INFO, "UID %d\n", uid);
            status = SST_FUNCTION(s003_data[1].api, uid, TEST_BUFF_SIZE, write_buff, 0);
            if (status != PSA_SST_SUCCESS)
            {
                val->print(PRINT_TEST, "UID %d set failed, Storage Space is exhausted\n", uid);
                break;
            }
        }
        TEST_ASSERT_EQUAL(status, s003_data[1].status, TEST_CHECKPOINT_NUM(1));

        /* Store number of set()s it took to saturate the storage */
        count = uid - TEST_BASE_UID_VALUE;
        results[i] = uid - TEST_BASE_UID_VALUE;

        val->print(PRINT_TEST, "Remove all registered UIDs\n", 0);
        for (uid = TEST_BASE_UID_VALUE; uid < count + TEST_BASE_UID_VALUE; uid++)
        {
            val->print(PRINT_INFO, "Removing UID %d\n", uid);
            status = SST_FUNCTION(s003_data[2].api, uid);
            if (status != PSA_SST_SUCCESS)
                return VAL_STATUS_ERROR;
        }
        TEST_ASSERT_EQUAL(status, s003_data[2].status, TEST_CHECKPOINT_NUM(2));
    }

    /* Check that it takes equal number of UIDs to fill up the storage each time */
    for (i = 0; i < (NUM_ITERATIONS -1); i++)
    {
        if (results[i] != results[i+1])
        {
            val->print(PRINT_ERROR, "\tERROR : Mismatch between number of UIDs required to\n", 0);
            val->print(PRINT_ERROR, "\t fill up the storage between iteration %d", i);
            val->print(PRINT_ERROR, " and iteration %d\n", i+1);
            return VAL_STATUS_ERROR;
        }
    }
    return VAL_STATUS_SUCCESS;
}