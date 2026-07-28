/**
 * @file motion_control.h
 * @brief Stepper motor (theta-axis) control module
 */

#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#define MOTOR_STEP_ANGLE_DEG    0.9f            /* NEMA23 0.9° per step */
#define MOTOR_MICROSTEPS        4               /* TB6600 DIP: SW4=OFF SW5=ON SW6=OFF = 4x microstep = 1600 ppr */
#define MOTOR_STEPS_PER_REV     ((360.0f / MOTOR_STEP_ANGLE_DEG) * MOTOR_MICROSTEPS)  /* 400 * 4 = 1600 */
_Static_assert((int)((360.0f / 0.9f) * 4) == 1600, "MOTOR_STEPS_PER_REV must equal TB6600 ppr setting");
#define HOMING_SPEED_RPM        30              /* slow approach for home */
#define NOMINAL_SPEED_RPM       12              /* 0.2 rev/s */

/* Motion simulation is independent so the motor can be exercised on hardware
 * while thermal, pump, and safety modules remain in simulation mode. */
#ifndef MOTION_SIMULATION_MODE
#define MOTION_SIMULATION_MODE 0
#endif

typedef struct {
    float target_rpm;
    float current_rpm;
    float position_revolutions;
    bool is_homed;
    bool is_enabled;
    uint32_t step_counter;
    uint32_t last_step_time_us;
} stepper_motor_t;

typedef struct {
    float current_velocity;
    float max_acceleration;    /* rev/s^2 */
    float target_velocity;
    float position;            /* revolutions */
} motion_profile_t;

void motion_init(void);
void motion_home_axis(void);
bool motion_is_homing_active(void);
void motion_enable(bool enable);
void motion_set_target_speed(float rpm);
void motion_set_target_rpm(float rpm); /* legacy alias used by control_system */
void motion_step(void);
void motion_update(void);
float motion_get_position(void);
float motion_get_rpm(void);
bool motion_is_homed(void);
bool motion_is_enabled(void);

#endif /* MOTION_CONTROL_H */
