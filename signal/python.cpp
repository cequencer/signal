#include "signal.h"

using namespace libsignal;

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(Node, NodeRefT<Node>);

void init_libsignal(py::module &m)
{
    py::class_<Graph>(m, "Graph")
		.def(py::init<>())
		.def("get_output", &Graph::get_output);
	py::class_<Node, NodeRefT<Node>>(m, "Node")
		.def(py::init<>())
		.def("add_input", &Node::add_input)
		.def("set_param", &Node::set_param);
	py::class_<Sine, NodeRefT<Node>>(m, "Sine", py::base<Node>())
		.def(py::init<NodeRef>())
		.def(py::init<float>());
	py::class_<ASR, NodeRefT<Node>>(m, "ASR", py::base<Node>())
		.def(py::init<NodeRef, NodeRef, NodeRef>())
		.def(py::init<float, NodeRef, NodeRef>())
		.def(py::init<NodeRef, float, NodeRef>())
		.def(py::init<NodeRef, NodeRef, float>())
		.def(py::init<float, float, NodeRef>())
		.def(py::init<float, NodeRef, float>())
		.def(py::init<float, float, float>());
	py::class_<Multiply, NodeRefT<Node>>(m, "Multiply", py::base<Node>())
		.def(py::init<NodeRef, NodeRef>());
	py::class_<Constant, NodeRefT<Node>>(m, "Constant", py::base<Node>())
		.def(py::init<float>());
	py::class_<Pan, NodeRefT<Node>>(m, "Pan", py::base<Node>())
		.def(py::init<int, NodeRef, NodeRef>())
		.def(py::init<int, float, NodeRef>())
		.def(py::init<int, NodeRef, float>())
		.def(py::init<int, float, float>());
	py::class_<Delay, NodeRefT<Node>>(m, "Delay", py::base<Node>())
		.def(py::init<NodeRef, NodeRef, NodeRef, float>())
		.def(py::init<NodeRef, float, NodeRef, float>())
		.def(py::init<NodeRef, NodeRef, float, float>())
		.def(py::init<NodeRef, float, float, float>())
		.def(py::init<float, NodeRef, NodeRef, float>())
		.def(py::init<float, float, NodeRef, float>())
		.def(py::init<float, NodeRef, float, float>())
		.def(py::init<float, float, float, float>());
	py::class_<Tick, NodeRefT<Node>>(m, "Tick", py::base<Node>())
		.def(py::init<NodeRef>())
		.def(py::init<float>());
	py::class_<NodeRef>(m, "NodeRef")
		.def(py::init<>());
	py::class_<Sampler, NodeRefT<Node>>(m, "Sampler", py::base<Node>())
		.def(py::init<Buffer *>());
	// py::class_<std::shared_ptr <Buffer>>(m, "Buffer")
	//	.def(py::init<const char *>());
}

PYBIND11_PLUGIN(libsignal) {
    py::module m("libsignal", "libsignal");

    init_libsignal(m);

    return m.ptr();
}

