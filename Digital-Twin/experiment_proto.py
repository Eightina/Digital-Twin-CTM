import CTMCoreImprove as ctm
from get_vehicles import get_vehicles
from generate_init_file import generate_init_file
from webster import phase4webster
import xml.etree.ElementTree as et
import traci

duration = phase4webster(
    [400, 400, 200, 200]
)
phase_num = len(duration)
cycle_len = round(sum(duration) + phase_num * 4)
time0 = 14 * cycle_len

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


step = 0
traci.trafficlight.setProgram(tlsID="0", programID="0")

while step <= time0:
    if (step > 0) & (step % cycle_len == 0):
        time_loss = 0.0
        for veh_id in traci.vehicle.getIDList():
            time_loss += traci.vehicle.getTimeLoss(veh_id)
        sumo_delay.append(time_loss)
        # if ((step > 300) & (step % 3 == 0)):
            
    if step == cycle_len * 2:
        step_veh = get_vehicles(real_cell_length=20)

    if step == cycle_len * 3:
        sim0 = ctm.simulation("i_Test4")
        sim1 = ctm.simulation("i_Test4")
        
        generate_init_file(
            cellscale=1,
            seconds=cycle_len,
            offset=step,
            filename="Test101",
            arc_demand=[400, 400, 200, 200],
        )
        generate_init_file(
            cellscale=1,
            seconds=cycle_len,
            offset=step,
            filename="Test102",
            arc_demand=[400, 400, 200, 200],
        )
        
        sim0.initialize_with_occu("i_Test101", step_veh)
        delay0 = sim0.execute() * 10
        sim0.stepend()
        ctm_delay.append(delay0)
        
        sim1.initialize_with_occu("i_Test102", step_veh)
        delay0 = sim1.execute() * 10
        sim1.stepend()
        ctm_delay.append(delay0)

    if (step >= 700) & (step % 100 == 0):
        generate_init_file(
        
        )
        sim.initialize("i_Test5")
        delay0 = sim.execute() * 10
        if step == time0:
            sim.output_result()
        sim.stepend()
        ctm_delay.append(delay0)

    traci.simulationStep()
    step += 1

traci.close()


print("og_sumo:" + str(sumo_delay))
print("og_ctm: " + str(ctm_delay))
time0 = [100 * i for i in range(3, 13)]

sumo_delay = sumo_delay[2:]

temp = sumo_delay[0]
for i in range(1,len(sumo_delay)):
    sumo_delay[i] += temp
    temp = sumo_delay[i]
    
temp = ctm_delay[0]
for i in range(1,len(ctm_delay)):
    ctm_delay[i] += temp
    temp = ctm_delay[i]
    
print("sumo:" + str(sumo_delay))
print("ctm: " + str(ctm_delay))
# startdelay = sumo_delay[1]
# sumo_delay = [i - startdelay for i in sumo_delay]
# sumo_delay = sumo_delay[2:]
# print("sumo:" + str(sumo_delay))

from matplotlib import pyplot as plt

plt.plot(time0, sumo_delay, label="sumo")
plt.plot(time0, ctm_delay, label="ctm")
plt.legend()
# plt.xlim(98, 200)
# plt.ylim(0, 1500)
plt.show()

plt.plot(ctm_delay, sumo_delay, marker='.')
plt.xlabel('ctm')
plt.ylabel('sumo')
plt.show()

import pandas as pd
import seaborn as sns
resultdf = pd.DataFrame({'ctm':ctm_delay, 'sumo':sumo_delay})
corr = resultdf.corr() 
print(corr)
sns.heatmap(corr, cmap='GnBu', annot=True, cbar=False)

import numpy as np
ctm_delay = np.array(ctm_delay).reshape((-1, 1))
sumo_delay = np.array(sumo_delay)
from sklearn.linear_model import LinearRegression
model = LinearRegression()
# 训练模型
model.fit(ctm_delay, sumo_delay)
# 计算预测值
y_pred = model.predict(ctm_delay)
# 绘制原始数据和拟合直线
plt.scatter(ctm_delay, sumo_delay)
plt.plot(ctm_delay, y_pred, color='red')
plt.show()
# print(step_veh)
