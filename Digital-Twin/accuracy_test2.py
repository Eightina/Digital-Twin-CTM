import CTMCoreImprove as ctm
from get_vehicles import get_vehicles
from generate_init_file import generate_init_file
from webster import phase4webster
from set_sumo_logic import set_sumo_logic

from matplotlib import pyplot as plt
import xml.etree.ElementTree as et
import traci

demand = [400,400,200,200]
duration = phase4webster(demand)
print(duration)
# duration = [_ * 2 for _ in duration]

phase_num = len(duration)
cycle_len = round(sum(duration) + phase_num * 4)
start_offset_cycles = 10
output_cycles = 15
total_cycles = start_offset_cycles + output_cycles
# time = total_cycles * cycle_len


flow_record = []

# 1400  # 200开始，200-300，200-400...200-1400共12次
# switch 500 800 1100 1400
sumo_delay = []
ctm_delay = []

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
set_sumo_logic("0", duration)
print(traci.trafficlight.getAllProgramLogics('0')[0].phases)
step = 0
# traci.trafficlight.setProgram(tlsID="0", programID="0")

for cycle in range(total_cycles):
    cycle_loss = 0.0
    cur_steps = cycle * cycle_len

    if cycle == start_offset_cycles:  # 2 ~ 11
        step_veh = get_vehicles(cell_scale=1, veh_coeff=1.25)
        sim0 = ctm.simulation("i_Test4")
        generate_init_file(
            cell_scale=1,
            seconds=cycle_len,
            offset=cur_steps,
            filename="Test4",
            arc_demand=demand,
            phases_duration=duration,
            demand_coeff=0.4,
        )
        sim0.initialize_with_occu("i_Test4", step_veh)
        delay0 = sim0.execute() * 10
        sim0.stepend()
        ctm_delay.append(delay0 / sum(demand) / (cycle_len / 3600))

    elif cycle > start_offset_cycles:
        generate_init_file(
            cell_scale=1,
            seconds=cycle_len,
            offset=cur_steps,
            filename="Test4",
            arc_demand=demand,
            phases_duration=duration,
            demand_coeff=0.4,
        )
        sim0.initialize("i_Test4")
        delay0 = sim0.execute() * 10
        sim0.stepend()
        ctm_delay.append(delay0 / sum(demand) / (cycle_len / 3600))

    for step in range(cycle_len):  # 0 ~ 99
        traci.simulationStep()
        step_loss = 0.0
        # total_steps = cycle * cycle_len + step
        id_list = traci.vehicle.getIDList()

        for veh_id in id_list:
            step_loss += traci.vehicle.getTimeLoss(veh_id)

        step_loss /= len(id_list)
        cycle_loss += step_loss

    cycle_loss /= cycle_len
    sumo_delay.append(cycle_loss)

print("og_sumo:" + str(sumo_delay))
print("og_ctm: " + str(ctm_delay))
time0 = [cycle_len * i for i in range(output_cycles)]
sumo_delay = sumo_delay[start_offset_cycles:]
plt.plot(time0, sumo_delay, label="sumo", marker=".")
plt.plot(time0, ctm_delay, label="ctm", marker=".")
plt.legend()
plt.ylim(0, 30)
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
