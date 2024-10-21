#include <gtest/gtest.h>
#include <vector>
#include <numeric>
#include <chrono>
#include "core/perf/include/perf.hpp"
#include "seq/sotskov_a_sum_element_matrix/include/ops_seq.hpp"

namespace sotskov_a_sum_element_matrix_seq {

void run_perf_test(int rows, int columns) {
    std::vector<int> in(rows * columns, 1);
    std::vector<int> out(1, 0);

    std::shared_ptr<ppc::core::TaskData> taskDataSeq = std::make_shared<ppc::core::TaskData>();
    taskDataSeq->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskDataSeq->inputs_count.emplace_back(in.size());
    taskDataSeq->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskDataSeq->outputs_count.emplace_back(out.size());

    auto testTaskSequential = std::make_shared<TestTaskSequential<int>>(taskDataSeq);

    auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
    perfAttr->num_running = 10;

    const auto t0 = std::chrono::high_resolution_clock::now();
    perfAttr->current_timer = [&] {
        auto current_time_point = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
        return static_cast<double>(duration) * 1e-9;
    };

    auto perfResults = std::make_shared<ppc::core::PerfResults>();
    auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testTaskSequential);
    perfAnalyzer->task_run(perfAttr, perfResults);
    ppc::core::Perf::print_perf_statistic(perfResults);
    ASSERT_EQ(std::accumulate(in.begin(), in.end(), 0), out[0]);
}

TEST(sotskov_a_sum_element_matrix, test_pipeline_run) {
    run_perf_test(10000, 10000);
}

TEST(sotskov_a_sum_element_matrix, test_task_run) {
    run_perf_test(8000, 8000);
}

}  // namespace sotskov_a_sum_element_matrix_seq
