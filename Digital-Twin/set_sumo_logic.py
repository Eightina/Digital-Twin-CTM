import traci
from typing import List


def set_sumo_logic(tl_id: str='0', duration: List[int] = [30, 30, 30, 30]) -> None:
    logic_list = traci.trafficlight.getAllProgramLogics ('0')[0]
    for i in range(0, len(logic_list.phases), 2):
        logic_list.phases[i].duration = duration[i // 2]
    traci.trafficlight.setProgramLogic(tlsID='0', tls=logic_list)