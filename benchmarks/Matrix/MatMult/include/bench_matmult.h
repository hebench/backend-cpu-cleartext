
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_MatMult_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_MatMult_H_7e5fa8c2415240ea93eff148ed73539b

#include <gsl/gsl>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "clear_benchmark.h"

template <class T>
class MatMult_Benchmark : public ClearTextBenchmark
{
private:
    HEBERROR_DECLARE_CLASS_NAME(MatMult_Benchmark)

public:
    MatMult_Benchmark(hebench::cpp::BaseEngine &engine,
                      const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                      const hebench::APIBridge::WorkloadParams &bench_params);
    ~MatMult_Benchmark() override;

    hebench::APIBridge::Handle encode(const hebench::APIBridge::PackedData *p_parameters) override;
    void decode(hebench::APIBridge::Handle encoded_data, hebench::APIBridge::PackedData *p_native) override;

    hebench::APIBridge::Handle load(const hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;
    void store(hebench::APIBridge::Handle remote_data,
               hebench::APIBridge::Handle *p_local_data, std::uint64_t count) override;

    hebench::APIBridge::Handle operate(hebench::APIBridge::Handle h_remote_packed,
                                       const hebench::APIBridge::ParameterIndexer *p_param_indexers) override;

    //std::int64_t classTag() const override { return BaseBenchmark::classTag() | ClearTextBenchmark::tag; }

protected:
    std::array<std::uint64_t, 2> m_rows;
    std::array<std::uint64_t, 2> m_cols;

private:
    static void matMult(gsl::span<T> &result,
                        const gsl::span<const T> &M0, const gsl::span<const T> &M1,
                        std::size_t columns);
};

#include "inl/bench_matmult.inl"

#endif // defined _HEBench_ClearText_MatMult_H_7e5fa8c2415240ea93eff148ed73539b
