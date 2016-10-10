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
    auto lm = pb::handle(lmFunc.call("R1 ~ " + model, ols_data)).call("fit");
    std::cout << lm.call("summary").str() << std::endl;

    end = std::chrono::high_resolution_clock::now();

    std::cout << "time to calculate fit: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    //table = sm.stats.anova_lm(lm, typ=2)
    end = std::chrono::high_resolution_clock::now();

    std::cout << "time to calculate anova: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

    //print(table)

    /*residual_df = X.shape[0] - X.shape[1]
    dexpy.plot_pareto(lm.params, lm.bse, residual_df)
    #pp.show()*/
}
