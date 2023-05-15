from typing import List
def delay_simulate(
    step_veh,
    filler_demand=[300, 300, 300, 300],
    turn_rate: dict = {"r": 0.2, "s": 0.7, "l": 0.1},
    cell_scale=1,
    delay_units=1,
    delay_seconds_per_unit=1,
) -> List[int]:
    if (delay_seconds_per_unit <= 0):
        return step_veh
    res = step_veh.copy()
    delay_seconds = delay_units * delay_seconds_per_unit
    move_steps = delay_seconds // (2 * cell_scale)

    merge_scale = cell_scale * 2
    lane_cell_num = 1000 // (merge_scale * 10)

    lane_numbers = [
        [9, 1, 10],
        [15, 4, 16],
        [17, 5, 18],
        [23, 8, 24],
    ]
    origin_type = [
        ["r", "s", "l"],
        ["l", "s", "r"],
        ["l", "s", "r"],
        ["r", "s", "l"],
    ]
    for i in range(4):
        # group_demand = demand_origin[i
        total_demand = filler_demand[i]
        for j in range(3):
            lane_index = lane_numbers[i][j] - 1
            filler = (
                total_demand * turn_rate[origin_type[i][j]] / 3600 / cell_scale / 10
            )
            res[
                lane_index * lane_cell_num + move_steps : (lane_index + 1) * lane_cell_num
            ] = \
                res[
                lane_index * lane_cell_num : lane_index * lane_cell_num - move_steps
            ]
            
            res[
                lane_index * lane_cell_num : lane_index * lane_cell_num + move_steps
            ] = [filler] * move_steps
            # print("{} : {}".format(lane_index * lane_cell_num, lane_index * lane_cell_num + move_steps))
    return res
