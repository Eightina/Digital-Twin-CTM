from webster import ga_best_duration,phase4webster
from delay_simulate import delay_simulate
step_veh = [0]*1200
step_veh = delay_simulate(step_veh,[400,400,200,200],delay_seconds_per_unit=100)
print(step_veh)
res = ga_best_duration(
    step_veh,
    size_pop=10,
    max_iter=20
)
print(res)

print('webster:{}'.format(phase4webster([400, 400, 200, 200])))