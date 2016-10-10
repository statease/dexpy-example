#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <chrono>

/**
 * Runs the Filtrate example via python packages.
 */
void run_filtrate_example()
{
    namespace pb = pybind11;

    int factor_count = 4;
    int run_count = (int)pow(2, factor_count);

    auto dexpyModule = pb::module::import("dexpy");

    auto start = std::chrono::high_resolution_clock::now();

    auto dexpyFunc = dexpyModule.attr("build_factorial");
    auto design = pb::handle(dexpyFunc).call(factor_count, run_count);

    std::vector<int> filtrate_data { 45, 43, 68, 75, 48, 45, 80, 70, 71, 100, 60, 86, 65, 104, 65, 96 };
    std::vector<std::string> responseNames = { "R1" };
    auto pandasModule = pb::module::import("pandas");
    design.attr("response_data") = pandasModule.attr("DataFrame").call(pb::cast(filtrate_data), pb::arg("columns") = pb::cast(responseNames));

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "time to build " << factor_count << " factor " << run_count << " run factorial: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    start = std::chrono::high_resolution_clock::now();

    //int max_order = factor_count - 1;
    std::string model = "A + C + D + A:C + A:D";

    auto modelMatrixFunc = design.attr("create_model_matrix");
    auto X = modelMatrixFunc.call(model);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "time to generate X matrix: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    auto power = pb::handle(dexpyModule.attr("f_power")).call(model, X, 2, 0.05);
    std::cout << "power: " << power.str() << std::endl;
    end = std::chrono::high_resolution_clock::now();

    std::cout << "time to calculate power: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    auto ols_data = design.attr("factor_data");
    ols_data = ols_data.attr("join").call(design.attr("response_data"));
    std::cout << "regression matrix: " << ols_data.str() << std::endl;
    auto statsModelModule = pb::module::import("statsmodels.formula.api");
    auto lmFunc = statsModelModule.attr("ols");
    // This is a bit confusing: we need to pass in eval_env = -1 here, because
    // patsy tries to capture local variables from the stack using inspect
    // and it takes a paraemter for how far up the stack to walk
    // if we don't specify a depth to statsmodels it will use an incorrect
    // depth resulting in an error in the patsy capture function that looks
    // something like: "'NoneType' object has no attribute 'f_locals'" -hpa
    auto olsFunc = lmFunc.call("R1 ~ " + model, ols_data, pb::arg("eval_env") = -1);
    auto lm = olsFunc.attr("fit").call();
    std::cout << lm.attr("summary").call().str() << std::endl;

    end = std::chrono::high_resolution_clock::now();

    std::cout << "time to calculate fit: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    auto statsModule = pb::module::import("statsmodels.api");
    // N.B. no 'e' on the 'typ' arg (controlls the sums of squares output)
    auto table = statsModule.attr("stats").attr("anova_lm").call(lm, pb::arg("typ") = 2);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "time to calculate anova: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    std::cout << table.str() << std::endl;

    auto xShape = pb::cast<std::tuple<int, int>>(X.attr("shape"));
    int residual_df = std::get<0>(xShape) - std::get<1>(xShape);
    dexpyModule.attr("plot_pareto").call(lm.attr("params"), lm.attr("bse"), residual_df);

    auto mplModule = pb::module::import("matplotlib.pyplot");
    mplModule.attr("show").call();
}
