import CTMCoreImprove as ctm
from get_vehicles import get_vehicles
from generate_init_file import generate_init_file
from webster import phase4webster
from set_sumo_logic import set_sumo_logic

from matplotlib import pyplot as plt
import xml.etree.ElementTree as et
import traci
import math

cur_demand = [400, 400, 200, 200]
# demand1 = [200, 200, 400, 400]
cur_duration = phase4webster(cur_demand)
# duration1 = phase4webster(demand1)
# print(duration0)
# print(duration1)
# duration = [_ * 2 for _ in duration]

# phase_num = len(duration0)
phase_num = 4
cur_cycle_len = round(sum(cur_duration) + phase_num * 4)
start_offset_cycles = 10
output_cycles = 30
total_cycles = start_offset_cycles + output_cycles
# change_step = start_offset_cycles + output_cycles // 2
change_step = 1650

# print("total time:{}".format(total_cycles * cycle_len))

readjust_coeff = 1.0

flow_record = []

# 1400  # 200开始，200-300，200-400...200-1400共12次
# switch 500 800 1100 1400
sumo_delay = []
ctm_delay = []
time0 = [0]

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
print(traci.trafficlight.getAllProgramLogics("0")[0].phases)
step_cnt = 0
# traci.trafficlight.setProgram(tlsID="0", programID="0")

for cycle in range(total_cycles):
    cycle_loss = 0.0
    cur_steps = cycle * cur_cycle_len

    if cycle >= start_offset_cycles:  # 2 ~ 11
        cell_scale = 1
        veh_coeff = 1.25
        step_veh = get_vehicles(cell_scale=cell_scale, veh_coeff=veh_coeff)
        merge_scale = cell_scale * 2
        edge_cells = 100 // merge_scale * 3
        cur_demand = [
            sum(step_veh[edge_cells * (i - 1) : edge_cells * (i)])
            * merge_scale
            * 10
            * (3600 / 100)
            / veh_coeff
            * math.sqrt(readjust_coeff)
            for i in [1, 4, 5, 8]
        ]
        cur_duration = phase4webster(cur_demand)
        cur_cycle_len = round(sum(cur_duration) + phase_num * 4)

        sim0 = ctm.simulation("i_Test4")
        generate_init_file(
            cellscale=1,
            seconds=cur_cycle_len,
            offset=cur_steps,
            filename="Test4",
            arc_demand=cur_demand,
            phases_duration=cur_duration,
            demand_coeff=0.4,
        )
        sim0.initialize_with_occu("i_Test4", step_veh)
        delay0 = sim0.execute() * 10
        sim0.stepend()
        ctm_delay.append(delay0 / sum(cur_demand) / (cur_cycle_len / 3600))

        set_sumo_logic("0", cur_duration)
        print("--------------------------------\n")
        print("set traffic\n")
        print("demand:{}\n".format(str(cur_demand)))
        print("duration:{}\n".format(str(cur_duration)))
        print("--------------------------------\n")

    time0.append(time0[-1] + cur_cycle_len)

    for step in range(cur_cycle_len):  # 0 ~ 99
        traci.simulationStep()
        step_cnt += 1
        if step_cnt == change_step:
            print("--------------------------------\n")
            print("change traffic\n")
            print("--------------------------------\n")

        step_loss = 0.0
        # total_steps = cycle * cycle_len + step
        id_list = traci.vehicle.getIDList()

        for veh_id in id_list:
            step_loss += traci.vehicle.getTimeLoss(veh_id)

        step_loss /= len(id_list)
        cycle_loss += step_loss

    cycle_loss /= cur_cycle_len
    sumo_delay.append(cycle_loss)

    if len(ctm_delay) > 0:
        readjust_coeff = sumo_delay[-1] / ctm_delay[-1] 

time0 = time0[1+start_offset_cycles:]
print("og_sumo:" + str(sumo_delay))
print("og_ctm: " + str(ctm_delay))
print(time0)
# time0 = [cycle_len * i for i in range(output_cycles)]
sumo_delay = sumo_delay[start_offset_cycles:]
plt.plot(time0, sumo_delay, label="sumo", marker=".")
plt.plot(time0, ctm_delay, label="ctm", marker=".")
plt.legend()
# plt.ylim(0, 30)
plt.show()

temp = sumo_delay[0]
for i in range(1, len(sumo_delay)):
    sumo_delay[i] += temp
    temp = sumo_delay[i]

temp = ctm_delay[0]
for i in range(1, len(ctm_delay)):
    ctm_delay[i] += temp
    temp = ctm_delay[i]

print("sumo:" + str(sumo_delay))
print("ctm: " + str(ctm_delay))

plt.plot(time0, sumo_delay, label="sumo")
plt.plot(time0, ctm_delay, label="ctm")
plt.legend()
plt.show()

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
