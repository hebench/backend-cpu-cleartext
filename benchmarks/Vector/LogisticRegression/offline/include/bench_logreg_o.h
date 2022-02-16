
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_LogReg_O_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_LogReg_O_H_7e5fa8c2415240ea93eff148ed73539b

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "benchmarks/Vector/LogisticRegression/include/bench_logreg.h"

template <class T>
class LogReg_O_Description : public hebench::cpp::BenchmarkDescription
{
public:
    static constexpr std::uint64_t NumWorkloadParams = 1;
    LogReg_O_Description(const hebench::APIBridge::Workload workload);
    ~LogReg_O_Description() override {}

    hebench::cpp::BaseBenchmark *createBenchmark(hebench::cpp::BaseEngine &engine,
                                                 const hebench::APIBridge::WorkloadParams *p_params) override;
    void destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench) override;

private:
    void init(const hebench::APIBridge::Workload workload);
    HEBERROR_DECLARE_CLASS_NAME(LogReg_O_Description)
};

template <>
inline LogReg_O_Description<float>::LogReg_O_Description(const hebench::APIBridge::Workload workload)
{
    init(workload);
    m_descriptor.data_type = hebench::APIBridge::DataType::Float32;
}

template <>
inline LogReg_O_Description<double>::LogReg_O_Description(const hebench::APIBridge::Workload workload)
{
    init(workload);
    m_descriptor.data_type = hebench::APIBridge::DataType::Float64;
}

template <class T>
inline void LogReg_O_Description<T>::init(const hebench::APIBridge::Workload workload)
{
    std::memset(&m_descriptor, 0, sizeof(hebench::APIBridge::BenchmarkDescriptor));
    m_descriptor.workload                         = workload;
    m_descriptor.category                         = hebench::APIBridge::Category::Offline;
    m_descriptor.cat_params.offline.data_count[0] = 1;
    m_descriptor.cat_params.offline.data_count[1] = 1;
    m_descriptor.cat_params.offline.data_count[2] = 29;
    m_descriptor.cipher_param_mask                = HEBENCH_HE_PARAM_FLAGS_ALL_PLAIN; // all plain text params for this example
    //
    m_descriptor.scheme   = HEBENCH_HE_SCHEME_PLAIN;
    m_descriptor.security = HEBENCH_HE_SECURITY_NONE;
    m_descriptor.other    = 0;

    hebench::cpp::WorkloadParams::LogisticRegression default_workload_params;
    default_workload_params.n() = 999;
    this->addDefaultParameters(default_workload_params);
}

template <class T>
inline hebench::cpp::BaseBenchmark *LogReg_O_Description<T>::createBenchmark(hebench::cpp::BaseEngine &engine, const hebench::APIBridge::WorkloadParams *p_params)
{
    return engine.template createObj<LogReg_Benchmark<T>>(engine, m_descriptor, *p_params);
}

template <class T>
inline void LogReg_O_Description<T>::destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench)
{
    if (p_bench)
    {
        hebench::cpp::BaseEngine &engine = p_bench->getEngine();
        engine.template destroyObj<hebench::cpp::BaseBenchmark>(p_bench);
    } // end if
}

#endif // defined _HEBench_ClearText_LogReg_O_H_7e5fa8c2415240ea93eff148ed73539b
