import CTMCoreImprove as ctm
from get_vehicles import get_vehicles
from generate_init_file import generate_init_file
import traci

traci.start(["sumo-gui", "-c", "./SUMO_Input/Test4.sumocfg"])
step = 0
id_list = traci.lanearea.getIDList()
# print(id_list)
with open("traci_log.txt", "w") as log:
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
    while step <= 1000:
        if step == 1000:
            step_veh = get_vehicles(log, 20)
            generate_init_file(
                cellscale = 1,
                seconds = 50,
                offset = step,
                filename = "Test4"
            )
            # print(vehicles)
            sim = ctm.simulation("i_Test4")
            print("simulation geo defined")
            sim.initialize_with_occu("i_Test4", step_veh)
            print("simulation specs defined")
            delay0 = sim.execute()
            print("test counting: 0 " + str(delay0))
            sim.output_result()
            sim.stepend()
            print("test counting: 0 done")
        traci.simulationStep()
        step += 1

    traci.close()
