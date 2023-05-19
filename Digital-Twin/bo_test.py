from webster import phase4webster, bo_best_duration
from delay_simulate import delay_simulate

step_veh = [0] * 1200
step_veh = delay_simulate(
    step_veh, 
    [400, 400, 200, 200], 
    # [200, 200, 400, 400], 
    delay_seconds_per_unit=100
)
print(step_veh)
res = bo_best_duration(step_veh, max_iter=100)
print(res)

print('webster:{}'.format(phase4webster([400, 400, 200, 200])))
