import CTMCoreImprove as ctm
from get_vehicles import get_vehicles, get_demand
from generate_init_file import generate_init_file
from generate_cons_file import generate_cons_file
from webster import phase4webster, best_duration
from set_sumo_logic import set_sumo_logic
from delay_simulate import delay_simulate

from matplotlib import pyplot as plt
import xml.etree.ElementTree as et
import traci
import math

# 注意！修改需求后要运行runsim.bat重新初始化sumo
cur_demand = [100, 100, 100, 100]
cur_duration = phase4webster(cur_demand)

phase_num = 4
cur_cycle_len = round(sum(cur_duration) + phase_num * 4)
start_offset_cycles = 10
output_cycles = 24
total_cycles = start_offset_cycles + output_cycles

cell_scale = 2
readjust_coeff = 1
demand_coeff = 1
sim_cycle_num = 2
sumo_coeff = 2
ctm_coeff = 44
search_backup_num = 7
delay_units = 1
delay_seconds_per_unit = 400

# flow_record = []
sumo_delay = []
time0 = [0]
og_ctm_delay = []

traci.start(
    [
        "sumo",
        "-c",
        "./SUMO_Input/Test4.sumocfg",
        "--statistic-output",
        "SUMO_Output/statistic.xml",
        "--duration-log.statistics",
        "--tripinfo-output.write-unfinished",
    ]
)
set_sumo_logic("0", cur_duration)
traci.simulationStep()
print(traci.trafficlight.getAllProgramLogics("0")[0].phases)
step_cnt = 0

for cycle in range(total_cycles):
    cycle_loss = 0.0

    if cycle >= start_offset_cycles:  # 2 ~ 11
        step_veh = get_vehicles(
            cell_scale=cell_scale, veh_coeff=math.sqrt(readjust_coeff)
        )
        
        step_veh = delay_simulate(
            step_veh=step_veh,
            filler_demand=cur_demand,
            cell_scale=cell_scale,
            delay_units=delay_units,
            delay_seconds_per_unit=delay_seconds_per_unit
        )
        
        # cur_duration = phase4webster(cur_demand)
        # cur_cycle_len = round(sum(cur_duration) + phase_num * 4)
        # generate_cons_file(
        #     cell_scale=cell_scale,
        #     file_name="Test4",
        # )
        # sim0 = ctm.simulation("i_Test4")
        # generate_init_file(
        #     cell_scale=cell_scale,
        #     seconds=cur_cycle_len * sim_cycle_num,
        #     offset=0,
        #     filename="Test4",
        #     arc_demand=cur_demand,
        #     phases_duration=cur_duration,
        # )
        # sim0.initialize_with_occu("i_Test4", step_veh)
        # delay0 = sim0.execute() * 10 / ctm_coeff
        # sim0.output_result()
        # sim0.stepend()
        
        [delay0, cur_duration] = best_duration(
            step_veh,
            arc_demand=cur_demand,
            num=search_backup_num,
            cell_scale=cell_scale,
            ctm_coeff=ctm_coeff,
            sim_cycle_num=sim_cycle_num,
        )

        set_sumo_logic("0", cur_duration)
        og_ctm_delay.append(delay0)

        print("------------cycle{}--------------------\n".format(cycle))
        print("set traffic\n")
        print("demand:{}\n".format(str(cur_demand)))
        print("duration:{}\n".format(str(cur_duration)))
        print("og_ctm:{}\n".format(str(delay0)))
        print("--------------------------------\n")

    time0.append(time0[-1] + cur_cycle_len)
    cur_demand = [0, 0, 0, 0]
    scan_delay_steplen = 5
    scan_demand_steplen = 5
    times = cur_cycle_len // scan_demand_steplen
    veh_delays = {}
    for step in range(cur_cycle_len):  # 0 ~ 99
        # if step % scan_demand_steplen == 0:
        if (step % scan_delay_steplen == 0) or (step == cur_cycle_len - 1):
            ## 记录sumo延误，准备后续计算
            id_list = traci.vehicle.getIDList()
            for veh_id in id_list:
                if veh_id not in veh_delays:
                    veh_delays[veh_id] = [traci.vehicle.getTimeLoss(veh_id)]
                else:
                    veh_delays[veh_id].append(traci.vehicle.getTimeLoss(veh_id))

        if step % scan_demand_steplen == 0:
            ## 从sumo采集需求demand
            step_veh = get_vehicles(
                cell_scale=cell_scale, veh_coeff=math.sqrt(readjust_coeff)
            )
            temp_cur_demand = get_demand(
                vehicles=step_veh, cell_scale=cell_scale, demand_coeff=demand_coeff
            )
            cur_demand = [
                (cur_demand[i] + temp_cur_demand[i] / times) for i in range(4)
            ]

        traci.simulationStep()

    # 计算sumo的延误
    if len(veh_delays.keys()) == 0:
        sumo_delay.append(0)
    else:
        for key in veh_delays.keys():
            cycle_loss += veh_delays[key][-1] - veh_delays[key][0]
        cycle_loss /= len(veh_delays.keys())
        sumo_delay.append(cycle_loss * sumo_coeff)
        print("sumo_delay:{}\n".format(str(sumo_delay[-1])))

sumo_delay = sumo_delay[start_offset_cycles:]
time0 = time0[1 + start_offset_cycles :]
print("sumo=" + str(sumo_delay))
print("ctm= " + str(og_ctm_delay))
print("time0= " + str(time0))

plt.plot(time0, sumo_delay, label="sumo", marker=".")
plt.plot(time0, og_ctm_delay, label="ctm", marker=".")
plt.legend()
plt.show()

temp = sumo_delay[0]
for i in range(1, len(sumo_delay)):
    sumo_delay[i] += temp
    temp = sumo_delay[i]

temp = og_ctm_delay[0]
for i in range(1, len(og_ctm_delay)):
    og_ctm_delay[i] += temp
    temp = og_ctm_delay[i]

print("sumo:" + str(sumo_delay))
print("ctm: " + str(og_ctm_delay))

plt.plot(time0, sumo_delay, label="sumo")
plt.plot(time0, og_ctm_delay, label="ctm")
plt.legend()
plt.show()

traci.close()
