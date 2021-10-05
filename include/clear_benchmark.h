
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_BE_Benchmark_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_BE_Benchmark_H_7e5fa8c2415240ea93eff148ed73539b

#include <array>

#include "hebench/api_bridge/cpp/hebench.hpp"

class ClearTextEngine;

class ClearTextBenchmark : public hebench::cpp::BaseBenchmark
{
private:
    HEBERROR_DECLARE_CLASS_NAME(ExampleBenchmark)

public:
    ClearTextBenchmark(hebench::cpp::BaseEngine &engine,
                       const hebench::APIBridge::BenchmarkDescriptor &bench_desc);
    ~ClearTextBenchmark() override;

    hebench::APIBridge::Handle encrypt(hebench::APIBridge::Handle encoded_data) override;
    hebench::APIBridge::Handle decrypt(hebench::APIBridge::Handle encrypted_data) override;
};

#endif // define _HEBench_ClearText_BE_Benchmark_H_7e5fa8c2415240ea93eff148ed73539b
