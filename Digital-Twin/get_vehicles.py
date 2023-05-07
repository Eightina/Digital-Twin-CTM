from typing import List
import traci

def get_vehicles(log=None, real_cell_length:int = 20):
    status_veh = []
    ctm_arc_ids = [i for i in range(1, 25)]
    for edge in range(1, 9):
        for lane in range(3):
            # ctm_arc_id = ctm_arc_ids[edge][lane]
            # edge = 1
            # lane = 0
            ctm_arc_id = ctm_arc_ids[(edge - 1) * 3 + lane]
            # 注意，这里取得的是上一时刻的状态，还是用占有率更好，一辆车占有率为44.0141
            # vehicles = [traci.lanearea.getLastStepVehicleNumber("arc_{}_{}".format(str(ctm_arc_id), str(det))) for det in range(100)]
            vehicles = [
                traci.lanearea.getLastStepOccupancy(
                    "arc_{}_{}".format(str(ctm_arc_id), str(det))
                )
                for det in range(100)
            ]
            vehicles = [
                (vehicles[a] + vehicles[a + 1]) / 2 / 100 / 0.44 / real_cell_length
                for a in range(0, 100, 2)
            ]
            if log != None:
                log.write("arc_{} ".format(str(ctm_arc_id)) + str(vehicles) + "\n")
            status_veh.extend(vehicles)
    return status_veh
