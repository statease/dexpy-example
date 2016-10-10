#include <pybind11/pybind11.h>
#include <pybind11/eval.h> // starting in pybind 1.9

#include <iostream>

void init_python()
{
    static bool isInitialized = false;

    if (!isInitialized) {
        _putenv_s("PYTHONPATH", "./dexpy_env");
        Py_Initialize();
        isInitialized = true;
    }
}

void run_filtrate_example();

int main(int /*argc*/, char* /*argv[]*/)
{
    init_python();

    namespace pb = pybind11;
    try
    {
        /*pb::module mainModule = pb::module::import("__main__");
        pb::object mainNamespace = mainModule.attr("__dict__");*/
        run_filtrate_example();

    } catch (pb::error_already_set& e)
    {
        std::cerr << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
