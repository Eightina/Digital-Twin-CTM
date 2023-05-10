from typing import List
import traci


def get_vehicles(
    log=None,
    cell_scale: int = 1,
    veh_coeff: float = 1.0
    # real_cell_length: int = 20
):
    merge_scale = cell_scale * 2
    cell_length = merge_scale * 10
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
            agg_vehicles = [
                # sum(vehicles[a : a + merge_scale]) / merge_scale / 100 / 0.44 / real_cell_length
                sum(vehicles[a : a + merge_scale])
                / 100
                / 0.44
                / cell_length
                * veh_coeff
                for a in range(0, 100, merge_scale)
            ]
            if log != None:
                log.write("arc_{} ".format(str(ctm_arc_id)) + str(agg_vehicles) + "\n")
            status_veh.extend(agg_vehicles)
    return status_veh


def get_demand(
    vehicles, cell_scale: int = 1, demand_coeff:float = 1.0
):
    merge_scale = cell_scale * 2
    demand = [0, 0, 0, 0]
    lane_numbers = [
        [9, 1, 10],
        [15, 4, 16],
        [17, 5, 18],
        [23, 8, 24],
    ]
    lane_cell_num = 100 // merge_scale
    for i in range(4):
        edge_flow = 0
        lanes = lane_numbers[i]
        for j in range(3):
            edge_flow += sum(
                vehicles[
                    ((lanes[j] - 1) * lane_cell_num) : (lanes[j] * lane_cell_num) - 1
                ]
            )
        edge_flow = (
            edge_flow * merge_scale * 10 * (3600 / 100) * demand_coeff
        )
        demand[i] = edge_flow
    return demand
