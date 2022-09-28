#ifndef _HEBench_ClearText_SimpleSetIntersection_H_
#define _HEBench_ClearText_SimpleSetIntersection_H_

#include <gsl/gsl>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "clear_benchmark.h"

template <class T>
class SimpleSetIntersection_Benchmark : public ClearTextBenchmark
{
private:
    HEBERROR_DECLARE_CLASS_NAME(SimpleSetIntersection_Benchmark)

public:
    SimpleSetIntersection_Benchmark(hebench::cpp::BaseEngine &engine,
                     const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                     const hebench::APIBridge::WorkloadParams &bench_params);
    ~SimpleSetIntersection_Benchmark() override;

    hebench::APIBridge::Handle encode(const hebench::APIBridge::DataPackCollection *p_parameters) override;
    void decode(hebench::APIBridge::Handle encoded_data, hebench::APIBridge::DataPackCollection *p_native) override;

    hebench::APIBridge::Handle load(const hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;
    void store(hebench::APIBridge::Handle remote_data,
               hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;

    hebench::APIBridge::Handle operate(hebench::APIBridge::Handle h_remote_packed,
                                       const hebench::APIBridge::ParameterIndexer *p_param_indexers) override;

protected:
    std::uint64_t m_set_size_x;
    std::uint64_t m_set_size_y;
    std::uint64_t m_item_size_k;
    hebench::APIBridge::Workload m_workload;

private:
    static void mySetIntersection(const T *dataset_X, const T *dataset_Y, gsl::span<T> &result,
                                  std::size_t n, std::size_t m, std::size_t k);
    static bool isMemberOf(const T *dataset, const T *value, std::size_t n, std::size_t k);
    static void SimpleSetIntersection(gsl::span<T> &result,
                                      const gsl::span<const T> &X, const gsl::span<const T> &Y,
                                      std::size_t n, std::size_t m, std::size_t k);
};

#include "inl/bench_simplesetint.inl"

#endif // defined _HEBench_ClearText_SimpleSetIntersection_H_
