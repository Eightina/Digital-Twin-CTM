import CTMCoreImprove as ctm
from get_vehicles import get_vehicles
from generate_init_file import generate_init_file
from generate_cons_file import generate_cons_file
from webster import phase4webster
from set_sumo_logic import set_sumo_logic

from matplotlib import pyplot as plt
import numpy as np
import xml.etree.ElementTree as et
import traci

demand0 = [400,400,200,200]
demand1 = [200,200,400,400]
duration0 = phase4webster(demand0)
duration1 = phase4webster(demand1)
print(duration0)
print(duration1)
# duration = [_ * 2 for _ in duration]

phase_num = len(duration0)
cycle_len = round(sum(duration0) + phase_num * 4)
start_offset_cycles = 10
# output_cycles = 24 
output_cycles = 1 
change_step = 1050
change_cycle = 1050 // cycle_len
total_cycles = start_offset_cycles + output_cycles
print('total time:{}'.format(total_cycles * cycle_len))


flow_record = []

# 1400  # 200开始，200-300，200-400...200-1400共12次
# switch 500 800 1100 1400
sumo_delay = []
ctm_delay = []
absolute_density_error = []
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
set_sumo_logic("0", duration0)
print(traci.trafficlight.getAllProgramLogics('0')[0].phases)
# step = 0
# traci.trafficlight.setProgram(tlsID="0", programID="0")

for cycle in range(total_cycles):
    cycle_loss = 0.0
    cycle_absolute_density_error = 0.0
    cur_steps = cycle * cycle_len

    if cycle == start_offset_cycles:  # 2 ~ 11
        step_veh = get_vehicles(cell_scale=1)
        generate_cons_file(
                cell_scale=1,
                file_name="Test4",
        )
        sim0 = ctm.simulation("i_Test4")
        generate_init_file(
            cell_scale=1,
            seconds=cycle_len,
            offset=cur_steps,
            filename="Test4",
            arc_demand=demand0,
            phases_duration=duration0,
            # demand_coeff=0.4,
        )
        sim0.initialize_with_occu("i_Test4", step_veh)
        delay0 = sim0.execute()
        delay0 = (
                delay0
                * 10
                # )
                / (
                    (sum(demand0) * cycle_len / 3600)
                    + (sum(step_veh) * 1 * 2 * 10)
                )
            )
        vehicles = sim0.get_occu_record()
        sim0.stepend()
        ctm_delay.append(delay0)

    elif (change_cycle > cycle > start_offset_cycles):
        generate_init_file(
            cell_scale=1,
            seconds=cycle_len,
            offset=cur_steps,
            filename="Test4",
            arc_demand=demand0,
            phases_duration=duration0,
            # demand_coeff=0.4,
        )
        sim0.initialize("i_Test4")
        delay0 = sim0.execute()
        delay0 = (
                delay0
                * 10
                # )
                / (
                    (sum(demand0) * cycle_len / 3600)
                    + (sum(step_veh) * 1 * 2 * 10)
                )
            )
        vehicles = sim0.get_occu_record()
        sim0.stepend()
        ctm_delay.append(delay0)
    
    elif cycle >= change_cycle:
        if cycle == change_cycle:
            set_sumo_logic("0", duration1)
        generate_init_file(
            cell_scale=1,
            seconds=cycle_len,
            offset=cur_steps,
            filename="Test4",
            arc_demand=demand1,
            phases_duration=duration1,
            # demand_coeff=0.4,
        )
        sim0.initialize("i_Test4")
        delay0 = sim0.execute() * 10
        delay0 = (
                delay0
                * 10
                # )
                / (
                    (sum(demand0) * cycle_len / 3600)
                    + (sum(step_veh) * 1 * 2 * 10)
                )
            )
        vehicles = sim0.get_occu_record()
        sim0.stepend()
        ctm_delay.append(delay0)
        
    scan_delay_steplen = 5
    times = cycle_len // scan_delay_steplen
    veh_delays = {}
    step_absolute_density_error_in_one_cycle = []
    for step in range(cycle_len):  # 0 ~ 99
        # print(cycle_len)
        if (step % scan_delay_steplen == 0) or (step == cycle_len - 1):
            ## 记录sumo延误，准备后续计算
            id_list = traci.vehicle.getIDList()
            for veh_id in id_list:
                if veh_id not in veh_delays:
                    veh_delays[veh_id] = [traci.vehicle.getTimeLoss(veh_id)]
                else:
                    veh_delays[veh_id].append(traci.vehicle.getTimeLoss(veh_id))
        if (cycle >= start_offset_cycles):
            temp_veh = np.array(get_vehicles(cell_scale=1))
            ctm_veh = np.array(vehicles[step//2][:len(temp_veh)])
            print(step, len(vehicles))
            step_absolute_density_error = np.sum(np.abs(ctm_veh - temp_veh))
            cycle_absolute_density_error += step_absolute_density_error
            step_absolute_density_error_in_one_cycle.append(step_absolute_density_error)
            print(len(step_absolute_density_error_in_one_cycle))
        traci.simulationStep()
        step_loss = 0.0
        # total_steps = cycle * cycle_len + step
        # id_list = traci.vehicle.getIDList()

        # for veh_id in id_list:
        #     step_loss += traci.vehicle.getTimeLoss(veh_id)

        # step_loss /= len(id_list)
        # cycle_loss += step_loss

    # cycle_loss /= cycle_len
    # sumo_delay.append(cycle_loss)
    if (cycle >= start_offset_cycles):
        absolute_density_error.append(cycle_absolute_density_error)
        
    if len(veh_delays.keys()) == 0:
        sumo_delay.append(0)
    else:
        for key in veh_delays.keys():
            cycle_loss += veh_delays[key][-1] - veh_delays[key][0]
        cycle_loss /= len(veh_delays.keys())
        # og_ctm_delay[-1] /= len(veh_delays.keys())
        if len(ctm_delay) > 0:
            ctm_delay[-1] /= len(veh_delays.keys())
        # sumo_delay.append(cycle_loss * sumo_coeff)
        sumo_delay.append(cycle_loss)
        
print("absolute_density_error=" + str(absolute_density_error))
print("step_absolute_density_error_in_one_cycle=" + str(step_absolute_density_error_in_one_cycle))

print("og_sumo=" + str(sumo_delay))
print("og_ctm= " + str(ctm_delay))
time0 = [cycle_len * i for i in range(output_cycles)]
sumo_delay = sumo_delay[start_offset_cycles:]

plt.plot(time0, absolute_density_error, label="sumo", marker=".")
plt.show()

plt.plot(time0, sumo_delay, label="sumo", marker=".")
plt.plot(time0, ctm_delay, label="ctm", marker=".")
plt.legend()
plt.ylim(0, 30)
plt.show()


# temp = sumo_delay[0]
# for i in range(1, len(sumo_delay)):
#     sumo_delay[i] += temp
#     temp = sumo_delay[i]

# temp = ctm_delay[0]
# for i in range(1, len(ctm_delay)):
#     ctm_delay[i] += temp
#     temp = ctm_delay[i]

# print("sumo:" + str(sumo_delay))
# print("ctm: " + str(ctm_delay))

# plt.plot(time0, sumo_delay, label="sumo")
# plt.plot(time0, ctm_delay, label="ctm")
# plt.legend()
# plt.show()

sim0.output_result()
traci.close()


# while step <= time0:
#     if (step > 0) & (step % cycle_len == 0):
#         time_loss = 0.0
#         for veh_id in traci.vehicle.getIDList():
#             time_loss += traci.vehicle.getTimeLoss(veh_id)
#         sumo_delay.append(time_loss)
#         # if ((step > 300) & (step % 3 == 0)):

#     if step == cycle_len * 2:
#         step_veh = get_vehicles(real_cell_length=20)

#     if step == cycle_len * 3:
#         sim0 = ctm.simulation("i_Test4")
#         sim1 = ctm.simulation("i_Test4")

#         generate_init_file(
#             cellscale=1,
#             seconds=cycle_len,
#             offset=step,
#             filename="Test101",
#             arc_demand=[400, 400, 200, 200],
#         )
#         generate_init_file(
#             cellscale=1,
#             seconds=cycle_len,
#             offset=step,
#             filename="Test102",
#             arc_demand=[400, 400, 200, 200],
#         )

#         sim0.initialize_with_occu("i_Test101", step_veh)
#         delay0 = sim0.execute() * 10
#         sim0.stepend()
#         ctm_delay.append(delay0)

#         sim1.initialize_with_occu("i_Test102", step_veh)
#         delay0 = sim1.execute() * 10
#         sim1.stepend()
#         ctm_delay.append(delay0)

#     if (step >= 700) & (step % 100 == 0):
#         generate_init_file()
#         sim.initialize("i_Test5")
#         delay0 = sim.execute() * 10
#         if step == time0:
#             sim.output_result()
#         sim.stepend()
#         ctm_delay.append(delay0)

#     traci.simulationStep()
#     step += 1

# traci.close()


# print("og_sumo:" + str(sumo_delay))
# print("og_ctm: " + str(ctm_delay))
# time0 = [100 * i for i in range(3, 13)]

# sumo_delay = sumo_delay[2:]

# temp = sumo_delay[0]
# for i in range(1, len(sumo_delay)):
#     sumo_delay[i] += temp
#     temp = sumo_delay[i]

# temp = ctm_delay[0]
# for i in range(1, len(ctm_delay)):
#     ctm_delay[i] += temp
#     temp = ctm_delay[i]

# print("sumo:" + str(sumo_delay))
# print("ctm: " + str(ctm_delay))
# # startdelay = sumo_delay[1]
# # sumo_delay = [i - startdelay for i in sumo_delay]
# # sumo_delay = sumo_delay[2:]
# # print("sumo:" + str(sumo_delay))

# from matplotlib import pyplot as plt

# plt.plot(time0, sumo_delay, label="sumo")
# plt.plot(time0, ctm_delay, label="ctm")
# plt.legend()
# # plt.xlim(98, 200)
# # plt.ylim(0, 1500)
# plt.show()

# plt.plot(ctm_delay, sumo_delay, marker=".")
# plt.xlabel("ctm")
# plt.ylabel("sumo")
# plt.show()

# import pandas as pd
# import seaborn as sns

# resultdf = pd.DataFrame({"ctm": ctm_delay, "sumo": sumo_delay})
# corr = resultdf.corr()
# print(corr)
# sns.heatmap(corr, cmap="GnBu", annot=True, cbar=False)

# import numpy as np

# ctm_delay = np.array(ctm_delay).reshape((-1, 1))
# sumo_delay = np.array(sumo_delay)
# from sklearn.linear_model import LinearRegression

# model = LinearRegression()
# # 训练模型
# model.fit(ctm_delay, sumo_delay)
# # 计算预测值
# y_pred = model.predict(ctm_delay)
# # 绘制原始数据和拟合直线
# plt.scatter(ctm_delay, sumo_delay)
# plt.plot(ctm_delay, y_pred, color="red")
# plt.show()
# # print(step_veh)
