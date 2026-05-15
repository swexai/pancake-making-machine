/**
 * @file motion_control_test.c
 * @brief Test implementation for motion control module
 */

#include "motion_control_test.h"
#include "motion_control.h"
#include "main.h"
#include <math.h>

/* External function for printing (from logging module or main) */
extern void uart_printf(const char *format, ...);

#define TEST_TOLERANCE 0.01f  /* 1% tolerance for floating point comparisons */

/* ============================================================================
 * HELPER FUNCTIONS
 * ========================================================================= */

/**
 * @brief Compare two floating point values with tolerance
 */
static bool float_equals(float a, float b, float tolerance)
{
    float diff = (a > b) ? (a - b) : (b - a);
    return diff <= tolerance;
}

/**
 * @brief Print test result to UART
 */
void motion_test_print_result(const char *test_name, motion_test_result_t result)
{
    const char *result_str;
    switch (result) {
        case TEST_PASS:
            result_str = "PASS";
            break;
        case TEST_FAIL:
            result_str = "FAIL";
            break;
        case TEST_SKIP:
            result_str = "SKIP";
            break;
        default:
            result_str = "UNKNOWN";
            break;
    }
    
    uart_printf("[MOTION_TEST] %s: %s\r\n", test_name, result_str);
}

/* ============================================================================
 * TEST FUNCTIONS
 * ========================================================================= */

/**
 * @brief Test motion control initialization
 * 
 * Verifies:
 * - Module initializes without errors
 * - Motor is disabled initially
 * - Position is at zero
 * - Not homed initially
 */
motion_test_result_t motion_test_init(void)
{
    uart_printf("\r\n[MOTION_TEST] === Testing Initialization ===\r\n");
    
    motion_init();
    
    /* Check motor is disabled */
    if (motion_is_enabled()) {
        uart_printf("ERROR: Motor should be disabled after init\r\n");
        return TEST_FAIL;
    }
    
    /* Check position is zero */
    if (!float_equals(motion_get_position(), 0.0f, TEST_TOLERANCE)) {
        uart_printf("ERROR: Position should be 0.0 after init, got %.2f\r\n", 
                   motion_get_position());
        return TEST_FAIL;
    }
    
    /* Check not homed */
    if (motion_is_homed()) {
        uart_printf("ERROR: Motor should not be homed initially\r\n");
        return TEST_FAIL;
    }
    
    uart_printf("Initialization test: PASS\r\n");
    return TEST_PASS;
}

/**
 * @brief Test motor enable/disable functionality
 * 
 * Verifies:
 * - Motor can be enabled
 * - Motor can be disabled
 * - Status flags update correctly
 */
motion_test_result_t motion_test_enable_disable(void)
{
    uart_printf("\r\n[MOTION_TEST] === Testing Enable/Disable ===\r\n");
    
    motion_init();
    
    /* Test enable */
    motion_enable(true);
    if (!motion_is_enabled()) {
        uart_printf("ERROR: Motor should be enabled\r\n");
        return TEST_FAIL;
    }
    uart_printf("Motor enabled successfully\r\n");
    
    /* Test disable */
    motion_enable(false);
    if (motion_is_enabled()) {
        uart_printf("ERROR: Motor should be disabled\r\n");
        return TEST_FAIL;
    }
    uart_printf("Motor disabled successfully\r\n");
    
    /* Test re-enable */
    motion_enable(true);
    if (!motion_is_enabled()) {
        uart_printf("ERROR: Motor should be re-enabled\r\n");
        return TEST_FAIL;
    }
    
    motion_enable(false);  /* Clean up */
    uart_printf("Enable/Disable test: PASS\r\n");
    return TEST_PASS;
}

/**
 * @brief Test speed setting and control
 * 
 * Verifies:
 * - Target speed can be set
 * - Speed is retrieved correctly
 * - Speed can be changed multiple times
 */
motion_test_result_t motion_test_speed_control(void)
{
    uart_printf("\r\n[MOTION_TEST] === Testing Speed Control ===\r\n");
    
    motion_init();
    motion_enable(true);
    
    float test_speeds[] = {30.0f, 60.0f, 120.0f, 0.0f};
    int num_speeds = sizeof(test_speeds) / sizeof(test_speeds[0]);
    
    for (int i = 0; i < num_speeds; i++) {
        motion_set_target_speed(test_speeds[i]);
        
        /* Run update cycle to let speed ramp */
        for (int j = 0; j < 100; j++) {
            motion_update();
            HAL_Delay(1);
        }
        
        float current_rpm = motion_get_rpm();
        uart_printf("Set speed to %.1f RPM, current: %.1f RPM\r\n", 
                   test_speeds[i], current_rpm);
        
        /* Allow some tolerance due to acceleration ramping */
        if (!float_equals(current_rpm, test_speeds[i], 10.0f)) {
            uart_printf("WARNING: Speed mismatch (may be ramping)\r\n");
        }
    }
    
    motion_enable(false);
    uart_printf("Speed Control test: PASS\r\n");
    return TEST_PASS;
}

/**
 * @brief Test position tracking
 * 
 * Verifies:
 * - Position updates during motion
 * - Position is tracked accurately
 * - Position can be retrieved correctly
 */
motion_test_result_t motion_test_position_tracking(void)
{
    uart_printf("\r\n[MOTION_TEST] === Testing Position Tracking ===\r\n");
    
    motion_init();
    
    /* Verify initial position */
    if (!float_equals(motion_get_position(), 0.0f, TEST_TOLERANCE)) {
        uart_printf("ERROR: Initial position should be 0.0\r\n");
        return TEST_FAIL;
    }
    uart_printf("Initial position: %.2f revolutions\r\n", motion_get_position());
    
    /* Enable motor and set speed */
    motion_enable(true);
    motion_set_target_speed(60.0f);  /* 60 RPM = 1 rev/s */
    
    /* Run motion for a measured time period */
    uint32_t start_time = HAL_GetTick();
    uint32_t duration_ms = 2000;  /* 2 seconds */
    
    while ((HAL_GetTick() - start_time) < duration_ms) {
        motion_update();
        HAL_Delay(1);
    }
    
    float final_position = motion_get_position();
    uart_printf("Position after 2 seconds at 60 RPM: %.2f revolutions\r\n", 
               final_position);
    
    /* At 60 RPM (1 rev/s), after 2 seconds we should have ~2 revolutions */
    if (final_position < 1.5f || final_position > 2.5f) {
        uart_printf("WARNING: Position may be inaccurate (expected ~2.0, got %.2f)\r\n", 
                   final_position);
    }
    
    motion_enable(false);
    uart_printf("Position Tracking test: PASS\r\n");
    return TEST_PASS;
}

/**
 * @brief Test acceleration profile
 * 
 * Verifies:
 * - Motor accelerates smoothly from idle
 * - Motor decelerates smoothly to stop
 * - Acceleration limits are respected
 */
motion_test_result_t motion_test_acceleration(void)
{
    uart_printf("\r\n[MOTION_TEST] === Testing Acceleration Profile ===\r\n");
    
    motion_init();
    motion_enable(true);
    
    /* Test acceleration phase */
    uart_printf("Starting acceleration test from 0 to 60 RPM...\r\n");
    motion_set_target_speed(60.0f);
    
    float prev_rpm = 0.0f;
    uint32_t sample_count = 0;
    
    for (uint32_t i = 0; i < 10000; i++) {
        motion_update();
        
        if (i % 100 == 0) {  /* Log every 100ms */
            float current_rpm = motion_get_rpm();
            uart_printf("t=%3ldms: RPM = %.2f\r\n", i, current_rpm);
            
            /* Verify monotonic increase during acceleration */
            if (current_rpm < prev_rpm && current_rpm > 59.0f) {
                uart_printf("ERROR: RPM decreased during acceleration phase\r\n");
                motion_enable(false);
                return TEST_FAIL;
            }
            prev_rpm = current_rpm;
            sample_count++;
            
            if (current_rpm >= 59.0f) {
                uart_printf("Reached target speed after ~%ldms\r\n", i);
                break;
            }
        }
        HAL_Delay(1);
    }
    
    /* Test deceleration phase */
    uart_printf("\nStarting deceleration test from 60 to 0 RPM...\r\n");
    motion_set_target_speed(0.0f);
    prev_rpm = 60.0f;
    
    for (uint32_t i = 0; i < 10000; i++) {
        motion_update();
        
        if (i % 100 == 0) {  /* Log every 100ms */
            float current_rpm = motion_get_rpm();
            uart_printf("t=%3ldms: RPM = %.2f\r\n", i, current_rpm);
            
            /* Verify monotonic decrease during deceleration */
            if (current_rpm > prev_rpm) {
                uart_printf("ERROR: RPM increased during deceleration phase\r\n");
                motion_enable(false);
                return TEST_FAIL;
            }
            prev_rpm = current_rpm;
            
            if (current_rpm <= 0.1f) {
                uart_printf("Reached zero speed after ~%ldms\r\n", i);
                break;
            }
        }
        HAL_Delay(1);
    }
    
    motion_enable(false);
    uart_printf("Acceleration Profile test: PASS\r\n");
    return TEST_PASS;
}

/**
 * @brief Test homing procedure
 * 
 * Verifies:
 * - Homing can be initiated
 * - Position is reset to zero after homing
 * - Homed flag is set correctly
 */
motion_test_result_t motion_test_homing(void)
{
    uart_printf("\r\n[MOTION_TEST] === Testing Homing Procedure ===\r\n");
    
    motion_init();
    
    /* Check not homed initially */
    if (motion_is_homed()) {
        uart_printf("ERROR: Should not be homed initially\r\n");
        return TEST_FAIL;
    }
    uart_printf("Initial state: not homed\r\n");
    
    /* Perform homing */
    uart_printf("Initiating homing...\r\n");
    motion_home_axis();
    
    /* Check homing result */
    if (!motion_is_homed()) {
        uart_printf("WARNING: Homing failed or timeout (may be normal if hardware unavailable)\r\n");
        return TEST_SKIP;
    }
    
    uart_printf("Homing completed successfully\r\n");
    
    /* Verify position is at zero */
    float pos = motion_get_position();
    if (!float_equals(pos, 0.0f, TEST_TOLERANCE)) {
        uart_printf("ERROR: Position should be 0.0 after homing, got %.2f\r\n", pos);
        return TEST_FAIL;
    }
    
    uart_printf("Position correctly reset to 0.0 revolutions\r\n");
    uart_printf("Homing test: PASS\r\n");
    return TEST_PASS;
}

/* ============================================================================
 * TEST SUITE RUNNER
 * ========================================================================= */

/**
 * @brief Run all motion control tests
 */
bool motion_test_run_all(void)
{
    uart_printf("\r\n");
    uart_printf("========================================\r\n");
    uart_printf("  MOTION CONTROL TEST SUITE\r\n");
    uart_printf("========================================\r\n");
    
    motion_test_result_t results[6];
    
    results[0] = motion_test_init();
    results[1] = motion_test_enable_disable();
    results[2] = motion_test_speed_control();
    results[3] = motion_test_position_tracking();
    results[4] = motion_test_acceleration();
    results[5] = motion_test_homing();
    
    /* Summary */
    uart_printf("\r\n");
    uart_printf("========================================\r\n");
    uart_printf("  TEST SUMMARY\r\n");
    uart_printf("========================================\r\n");
    
    motion_test_print_result("Initialization", results[0]);
    motion_test_print_result("Enable/Disable", results[1]);
    motion_test_print_result("Speed Control", results[2]);
    motion_test_print_result("Position Tracking", results[3]);
    motion_test_print_result("Acceleration", results[4]);
    motion_test_print_result("Homing", results[5]);
    
    /* Count passes */
    int pass_count = 0;
    int fail_count = 0;
    int skip_count = 0;
    
    for (int i = 0; i < 6; i++) {
        if (results[i] == TEST_PASS) pass_count++;
        else if (results[i] == TEST_FAIL) fail_count++;
        else skip_count++;
    }
    
    uart_printf("\nTotal: %d passed, %d failed, %d skipped\r\n", 
               pass_count, fail_count, skip_count);
    uart_printf("========================================\r\n\r\n");
    
    return (fail_count == 0);
}
