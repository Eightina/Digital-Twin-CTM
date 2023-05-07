import CTMCoreImprove as ctm
from get_vehicles import get_vehicles
from generate_init_file import generate_init_file
import xml.etree.ElementTree as et
import traci

time0 = [100 * i for i in range(1, 13)]
sumo_delay = []
ctm_delay = []
for time in time0:
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
    # id_list = traci.lanearea.getIDList()
    # with open("traci_log.txt", "w") as log:
    # ctm_arc_ids = [[],
    #             [9, 1, 10],
    #             [12, 2, 11],
    #             [13, 3, 14],
    #             [16, 4, 15],
    #             [18, 5, 17],
    #             [19, 6, 20],
    #             [22, 7, 21],
    #             [23, 8, 24],
    #             ]
    ctm_arc_ids = [i for i in range(1, 25)]
    process_delay = []
    while step <= time:
        traci.simulationStep()
        step += 1

        # if step == 100:
        #     step_veh = get_vehicles(log, 20)
        #     generate_init_file(
        #         cellscale = 1,
        #         seconds = 50,
        #         offset = step,
        #         filename = "Test4"
        #     )
        #     # print(vehicles)
        #     sim = ctm.simulation("i_Test4")
        #     print("simulation geo defined")
        #     sim.initialize_with_occu("i_Test4", step_veh)
        #     print("simulation specs defined")
        #     delay0 = sim.execute()
        #     print("test counting: 0 " + str(delay0))
        #     sim.output_result()
        #     sim.stepend()
        #     print("test counting: 0 done")
    traci.close()

    # tree = et.parse('SUMO_Output\statistic.xml')
    # root = tree.getroot()
    # vehicleTripStatistics = root.find('vehicleTripStatistics')
    # total_depart_delay = vehicleTripStatistics.get('totalDepartDelay')
    # python D:\programs\envs\python38\Lib\site-packages\sumo\tools\plot_trajectories.py SUMO_Output\fcd.xml -t ty -o plot.png -s
    # total_depart_delay = float(total_depart_delay)
    # print("by statistic total_depart_delay: " + (total_depart_delay))

    tree = et.parse("SUMO_Output\\tripinfos.xml")
    root = tree.getroot()
    total_delay = 0
    for tripinfo in root.findall("tripinfo"):
        # 获取该行程的延误时间，如果延误时间小于0，则将其设为0
        delay = float(tripinfo.get("timeLoss"))
        if delay < 0:
            delay = 0
        # 累加延误时间
        total_delay += delay
    print("Total delay (sum timeLoss):", total_delay, "seconds")
    sumo_delay.append(total_delay)
    
    generate_init_file(
        cellscale=1,
        seconds=time,
        offset=0,
        filename="Test4",
        arc_demand = [300] * 4)
    sim = ctm.simulation("i_Test4")
    sim.initialize("i_Test4")
    delay0 = sim.execute() * 10
    print("ctm delay time: " + str(delay0))
    sim.output_result()
    sim.stepend()
    ctm_delay.append(delay0)
print("sumo:" + str(sumo_delay))
print("ctm: " + str(ctm_delay))
time0 = [100 * i for i in range(3, 13)]
from matplotlib import pyplot as plt
plt.plot(time0, sumo_delay, label='sumo')
plt.plot(time0, ctm_delay, label='ctm')
plt.legend()
plt.xlim(98, 200)
plt.ylim(0, 1500)
plt.show()