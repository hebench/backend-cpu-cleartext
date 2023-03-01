
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_EltMult_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_EltMult_H_7e5fa8c2415240ea93eff148ed73539b

#include <gsl/gsl>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "clear_benchmark.h"

template <class T>
class EltMult_Benchmark : public ClearTextBenchmark
{
private:
    HEBERROR_DECLARE_CLASS_NAME(EltMult_Benchmark)

public:
    static constexpr std::uint64_t NumOpParams = 2;

    EltMult_Benchmark(hebench::cpp::BaseEngine &engine,
                      const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                      const hebench::APIBridge::WorkloadParams &bench_params);
    ~EltMult_Benchmark() override;

    hebench::APIBridge::Handle encode(const hebench::APIBridge::DataPackCollection *p_parameters) override;
    void decode(hebench::APIBridge::Handle encoded_data, hebench::APIBridge::DataPackCollection *p_native) override;

    hebench::APIBridge::Handle load(const hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;
    void store(hebench::APIBridge::Handle remote_data,
               hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;

    hebench::APIBridge::Handle operate(hebench::APIBridge::Handle h_remote_packed,
                                       const hebench::APIBridge::ParameterIndexer *p_param_indexers,
                                       std::uint64_t indexers_count) override;

protected:
    std::uint64_t m_vector_size;

private:
    static void eltMult(gsl::span<T> &result,
                        const gsl::span<const T> &V0, const gsl::span<const T> &V1,
                        std::size_t element_count);
};

#include "inl/bench_eltmult.inl"

#endif // defined _HEBench_ClearText_EltMult_H_7e5fa8c2415240ea93eff148ed73539b
