
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_EltAdd_O_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_EltAdd_O_H_7e5fa8c2415240ea93eff148ed73539b

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "benchmarks/Vector/EltwiseAdd/include/bench_eltadd.h"

template <class T>
class EltAdd_O_Description : public hebench::cpp::BenchmarkDescription
{
public:
    EltAdd_O_Description();
    ~EltAdd_O_Description() override {}

    hebench::cpp::BaseBenchmark *createBenchmark(hebench::cpp::BaseEngine &engine, const hebench::APIBridge::WorkloadParams *p_params) override;
    void destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench) override;

private:
    void init();
    HEBERROR_DECLARE_CLASS_NAME(EltAdd_O_Description)
};

template <>
inline EltAdd_O_Description<std::int32_t>::EltAdd_O_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Int32;
}

template <>
inline EltAdd_O_Description<std::int64_t>::EltAdd_O_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Int64;
}

template <>
inline EltAdd_O_Description<float>::EltAdd_O_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Float32;
}

template <>
inline EltAdd_O_Description<double>::EltAdd_O_Description()
{
    init();
    m_descriptor.data_type = hebench::APIBridge::DataType::Float64;
}

template <class T>
inline void EltAdd_O_Description<T>::init()
{
    std::memset(&m_descriptor, 0, sizeof(hebench::APIBridge::BenchmarkDescriptor));
    m_descriptor.workload                         = hebench::APIBridge::Workload::EltwiseAdd;
    m_descriptor.category                         = hebench::APIBridge::Category::Offline;
    m_descriptor.cat_params.offline.data_count[0] = 30;
    m_descriptor.cat_params.offline.data_count[1] = 40;
    m_descriptor.cipher_param_mask                = HEBENCH_HE_PARAM_FLAGS_ALL_PLAIN; // all plain text params for this example
    //
    m_descriptor.scheme   = HEBENCH_HE_SCHEME_PLAIN;
    m_descriptor.security = HEBENCH_HE_SECURITY_NONE;
    m_descriptor.other    = 0;

    hebench::cpp::WorkloadParams::EltwiseAdd default_workload_params;
    default_workload_params.n() = 999;
    this->addDefaultParameters(default_workload_params);
}

template <class T>
inline hebench::cpp::BaseBenchmark *EltAdd_O_Description<T>::createBenchmark(hebench::cpp::BaseEngine &engine, const hebench::APIBridge::WorkloadParams *p_params)
{
    return engine.template createObj<EltAdd_Benchmark<T>>(engine, m_descriptor, *p_params);
}

template <class T>
inline void EltAdd_O_Description<T>::destroyBenchmark(hebench::cpp::BaseBenchmark *p_bench)
{
    if (p_bench)
    {
        hebench::cpp::BaseEngine &engine = p_bench->getEngine();
        engine.template destroyObj<hebench::cpp::BaseBenchmark>(p_bench);
    } // end if
}

#endif // defined _HEBench_ClearText_EltAdd_O_H_7e5fa8c2415240ea93eff148ed73539b
