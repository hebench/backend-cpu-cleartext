
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cstring>
#include <memory>
#include <vector>

#include "../include/clear_benchmark.h"
#include "../include/clear_engine.h"
#include "../include/clear_error.h"

//------------------------
// class ExampleBenchmark
//------------------------

ClearTextBenchmark::ClearTextBenchmark(hebench::cpp::BaseEngine &engine,
                                       const hebench::APIBridge::BenchmarkDescriptor &bench_desc) :
    hebench::cpp::BaseBenchmark(engine, bench_desc)
{
    //
}

ClearTextBenchmark::~ClearTextBenchmark()
{
    // nothing needed in this example
}

hebench::APIBridge::Handle ClearTextBenchmark::encrypt(hebench::APIBridge::Handle encoded_data)
{
    // we only do plain text in this example, so, just return a copy of our internal data
    std::shared_ptr<void> p_encoded_data =
        this->getEngine().template retrieveFromHandle<std::shared_ptr<void>>(encoded_data);

    // the copy is shallow, but shared_ptr ensures correct destruction using reference counting
    return this->getEngine().template createHandle<decltype(p_encoded_data)>(encoded_data.size, 0,
                                                                             p_encoded_data);
}

hebench::APIBridge::Handle ClearTextBenchmark::decrypt(hebench::APIBridge::Handle encrypted_data)
{
    // we only do plain text in this example, so, just return a copy of our internal data
    std::shared_ptr<void> p_encrypted_data =
        this->getEngine().template retrieveFromHandle<std::shared_ptr<void>>(encrypted_data);

    // the copy is shallow, but shared_ptr ensures correct destruction using reference counting
    return this->getEngine().template createHandle<decltype(p_encrypted_data)>(encrypted_data.size, 0,
                                                                               p_encrypted_data);
}
