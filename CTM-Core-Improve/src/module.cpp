//#include <pybind11/pybind11.h>
//#include <Windows.h>
//#include <cmath>
//
//#include <iostream>
//#include <vector>
//#include "simulation.h"
//
//float result = 1.14514f;
//
//
//
//namespace py = pybind11;
//
//
//float conduct_simulation(char* inputname) {
//	// = "i_Test2";
//	simulation* simulator0 = new simulation(inputname);
//	simulator0->initialize();
//	float delay = simulator0->excecute();
//	simulator0->output_result();
//	return delay;
//}
//
//PYBIND11_MODULE(CTMCoreImprove, m) {
//    m.def("conduct_simulation", &conduct_simulation, R"pbdoc(
//        For test usage print info
//    )pbdoc");
//
//#ifdef VERSION_INFO
//    m.attr("__version__") = VERSION_INFO;
//#else
//    m.attr("__version__") = "dev";
//#endif
//}