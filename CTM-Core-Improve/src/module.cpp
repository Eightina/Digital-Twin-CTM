#include <pybind11/pybind11.h>
#include <Windows.h>
#include <cmath>

#include <iostream>
#include <vector>
#include "simulation.h"


namespace py = pybind11;


//float conduct_simulation(char* inputname) {
//	// = "i_Test2";
//	simulation* simulator0 = new simulation(inputname);
//	simulator0->initialize(inputname);
//	printf("init succeed...");
//	float delay = simulator0->excecute();
//	printf("excecute succeed...");
//	simulator0->output_result();
//	printf("output succeed...");
//	delete simulator0;
//	return delay;
//}


//PYBIND11_MODULE(CTMCoreImprove, m) {
//    m.def("conduct_simulation", &conduct_simulation, R"pbdoc(
//        For test usage print info
//    )pbdoc");


PYBIND11_MODULE(CTMCoreImprove, m) {
    py::class_<simulation>(m, "simulation")
        .def(py::init<char*>())
        .def("initialize", &simulation::initialize)
        .def("execute", &simulation::excecute)
        .def("output_result", &simulation::output_result);


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}