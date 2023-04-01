#include <pybind11/pybind11.h>
#include <Windows.h>
#include <cmath>

#include <iostream>
#include <vector>
#include "simulation.h"

float result = 1.14514f;



namespace py = pybind11;


float conduct_simulation(char* inputname) {
	// = "i_Test2";
	simulation* simulator0 = new simulation(inputname);
	simulator0->initialize();
	printf("init succeed...");
	float delay = simulator0->excecute();
	printf("excecute succeed...");
	simulator0->output_result();
	printf("output succeed...");
	return delay;
}

PYBIND11_MODULE(CTMCoreImprove, m) {
    m.def("conduct_simulation", &conduct_simulation, R"pbdoc(
        For test usage print info
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}