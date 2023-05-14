from webster import ga_best_duration
from delay_simulate import delay_simulate
step_veh = [0]*1200
step_veh = delay_simulate(step_veh,[400,400,200,200],delay_seconds_per_unit=100)
print(step_veh)
res = ga_best_duration(
    step_veh
)
print(res)