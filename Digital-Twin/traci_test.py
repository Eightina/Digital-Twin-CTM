import traci
traci.start(['sumo','-c','.\SUMO_Input\Test4.sumocfg'])
step = 0
id_list = traci.lanearea.getIDList()
print(id_list)
with open('traci_log.txt', 'w') as log:
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
    while step <= 200 :
        # for edge in range(1, 9):
        
            # for lane in range(3):
                # ctm_arc_id = ctm_arc_ids[edge][lane]
        edge = 1
        lane = 0
        ctm_arc_id = ctm_arc_ids[(edge - 1) * 3 + lane]
        # 注意，这里取得的是上一时刻的状态，还是用占有率更好
        # vehicles = [traci.lanearea.getLastStepVehicleNumber("arc_{}_{}".format(str(ctm_arc_id), str(det))) for det in range(100)]
        vehicles = [traci.lanearea.getLastStepOccupancy("arc_{}_{}".format(str(ctm_arc_id), str(det))) for det in range(100)]
        log.write("arc_{} ".format(str(ctm_arc_id)) + str(vehicles) + "\n")
        traci.simulationStep()
        step += 1
        
    traci.close()