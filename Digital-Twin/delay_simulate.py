def delay_simulate(step_veh, filler)->None:
    res = step_veh.copy()
    move_steps = len(filler)
    res[move_steps:] = res[:-move_steps]
    res[:move_steps] = filler
    return res
    
