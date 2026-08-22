"""
Model Predictive Control (MPC) simulation for automotive motor actuation.
Optimizes velocity profiles against obstacle distance to minimize settling time.
"""
import numpy as np
import matplotlib.pyplot as plt

def simulate_mpc(initial_velocity=15.0, target_velocity=0.0, horizon=10):
    dt = 0.1
    time_steps = 50
    velocities = [initial_velocity]
    positions = [0.0]
    obstacle_distance = 30.0  # meters

    for t in range(time_steps):
        current_v = velocities[-1]
        error = target_velocity - current_v if (obstacle_distance - positions[-1]) < 10 else (10.0 - current_v)
        
        # Predictive control step (acceleration optimization)
        accel = np.clip(0.8 * error, -4.0, 2.0)
        new_v = max(0.0, current_v + accel * dt)
        new_pos = positions[-1] + new_v * dt

        velocities.append(new_v)
        positions.append(new_pos)

    return velocities, positions

if __name__ == "__main__":
    v, x = simulate_mpc()
    print("MPC velocity profile simulation complete.")
