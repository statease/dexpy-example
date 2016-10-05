#include <pybind11/pybind11.h>
#include <pybind11/eval.h> // starting in pybind 1.9

void init_python()
{
    static bool isInitialized = false;

    if (!isInitialized) {
        Py_Initialize();
        isInitialized = true;
    }
}

int main(int argc, char *argv[])
{
    init_python();

    namespace pb = pybind11;
    try
    {
        pb::module mainModule = pb::module::import("__main__");
        pb::object mainNamespace = mainModule.attr("__dict__");
        pb::eval<pb::eval_statements>("from time import time,ctime\nprint('(pybind) Today is', ctime(time()))\n", mainNamespace);

    } catch (pb::error_already_set& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
