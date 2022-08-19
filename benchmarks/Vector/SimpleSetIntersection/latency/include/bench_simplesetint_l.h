#ifndef _HEBench_ClearText_SimpleSetIntersection_L_H_
#define _HEBench_ClearText_SimpleSetIntersection_L_H_

#include <string>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "benchmarks/Vector/SimpleSetIntersection/include/bench_simplesetint.h"

template <class T>
class SimpleSetIntersection_L_Description : public hebench::cpp::BenchmarkDescription
{
public:
    SimpleSetIntersection_L_Description();
    ~SimpleSetIntersection_L_Description() override {}

    hebench::cpp::BaseBenchmark *createBenchmark(hebench::cpp::BaseEngine &engine,
                                                 const hebench::APIBridge::WorkloadParams *p_params) override;
    void destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench) override;

private:
    void init();
    HEBERROR_DECLARE_CLASS_NAME(SimpleSetIntersection_L_Description)
};

template <>
inline SimpleSetIntersection_L_Description<float>::SimpleSetIntersection_L_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Float32;
}

template <>
inline SimpleSetIntersection_L_Description<double>::SimpleSetIntersection_L_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Float64;
}

template <>
inline SimpleSetIntersection_L_Description<std::int32_t>::SimpleSetIntersection_L_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Int32;
}

template <>
inline SimpleSetIntersection_L_Description<std::int64_t>::SimpleSetIntersection_L_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Int64;
}

template <>
inline SimpleSetIntersection_L_Description<std::string>::SimpleSetIntersection_L_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::String;
}

template <class T>
inline void SimpleSetIntersection_L_Description<T>::init()
{
    std::memset(&m_descriptor, 0, sizeof(hebench::APIBridge::BenchmarkDescriptor));
    m_descriptor.workload                                   = hebench::APIBridge::Workload::SimpleSetIntersection;
    m_descriptor.category                                   = hebench::APIBridge::Category::Latency;
    m_descriptor.cat_params.latency.warmup_iterations_count = 1; // warm up for 1 iteration
    m_descriptor.cat_params.min_test_time_ms                = 0; // default
    m_descriptor.cipher_param_mask                          = HEBENCH_HE_PARAM_FLAGS_ALL_PLAIN; // all plain text params for this example
    //
    m_descriptor.scheme   = HEBENCH_HE_SCHEME_PLAIN;
    m_descriptor.security = HEBENCH_HE_SECURITY_NONE;
    m_descriptor.other    = 0;

    hebench::cpp::WorkloadParams::LogisticRegression default_workload_params;
    default_workload_params.n() = 999;
    this->addDefaultParameters(default_workload_params);
}

template <class T>
inline hebench::cpp::BaseBenchmark *SimpleSetIntersection_L_Description<T>::createBenchmark(hebench::cpp::BaseEngine &engine, const hebench::APIBridge::WorkloadParams *p_params)
{
    return engine.template createObj<SimpleSetIntersection_Benchmark<T>>(engine, m_descriptor, *p_params);
}

template <class T>
inline void SimpleSetIntersection_L_Description<T>::destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench)
{
    if (p_bench)
    {
        hebench::cpp::BaseEngine &engine = p_bench->getEngine();
        engine.template destroyObj<hebench::cpp::BaseBenchmark>(p_bench);
    } // end if
}

#endif // defined _HEBench_ClearText_SimpleSetIntersection_L_H_
