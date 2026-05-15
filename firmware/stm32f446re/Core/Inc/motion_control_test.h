/**
 * @file motion_control_test.h
 * @brief Test functions for motion control module
 */

#ifndef MOTION_CONTROL_TEST_H
#define MOTION_CONTROL_TEST_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Test result enumeration
 */
typedef enum {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
} motion_test_result_t;

/**
 * @brief Run all motion control tests
 * @return true if all tests passed, false otherwise
 */
bool motion_test_run_all(void);

/**
 * @brief Test motion control initialization
 * @return TEST_PASS if successful, TEST_FAIL otherwise
 */
motion_test_result_t motion_test_init(void);

/**
 * @brief Test motor enable/disable functionality
 * @return TEST_PASS if successful, TEST_FAIL otherwise
 */
motion_test_result_t motion_test_enable_disable(void);

/**
 * @brief Test speed setting and ramping
 * @return TEST_PASS if successful, TEST_FAIL otherwise
 */
motion_test_result_t motion_test_speed_control(void);

/**
 * @brief Test position tracking during motion
 * @return TEST_PASS if successful, TEST_FAIL otherwise
 */
motion_test_result_t motion_test_position_tracking(void);

/**
 * @brief Test homing procedure
 * @return TEST_PASS if successful, TEST_FAIL otherwise
 */
motion_test_result_t motion_test_homing(void);

/**
 * @brief Test motor acceleration profile
 * @return TEST_PASS if successful, TEST_FAIL otherwise
 */
motion_test_result_t motion_test_acceleration(void);

/**
 * @brief Print test results to UART
 * @param test_name Name of the test
 * @param result Test result
 */
void motion_test_print_result(const char *test_name, motion_test_result_t result);

#endif /* MOTION_CONTROL_TEST_H */
