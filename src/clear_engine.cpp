
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include <cstring>

#include "../include/clear_engine.h"
#include "../include/clear_error.h"
#include "../include/clear_version.h"

// include all benchmarks
#include "benchmarks/Matrix/MatMult/latency/include/bench_matmul_l.h"
#include "benchmarks/Matrix/MatMult/offline/include/bench_matmul_o.h"
#include "benchmarks/Vector/DotProduct/latency/include/bench_dotprod_l.h"
#include "benchmarks/Vector/DotProduct/offline/include/bench_dotprod_o.h"
#include "benchmarks/Vector/EltwiseAdd/latency/include/bench_eltadd_l.h"
#include "benchmarks/Vector/EltwiseAdd/offline/include/bench_eltadd_o.h"
#include "benchmarks/Vector/EltwiseMult/latency/include/bench_eltmult_l.h"
#include "benchmarks/Vector/EltwiseMult/offline/include/bench_eltmult_o.h"
#include "benchmarks/Vector/LogisticRegression/latency/include/bench_logreg_l.h"
#include "benchmarks/Vector/LogisticRegression/offline/include/bench_logreg_o.h"
#include "benchmarks/Vector/SimpleSetIntersection/latency/include/bench_simplesetint_l.h"
#include "benchmarks/Vector/SimpleSetIntersection/offline/include/bench_simplesetint_o.h"

//-----------------
// Engine creation
//-----------------

namespace hebench {
namespace cpp {

BaseEngine *createEngine(const std::int8_t *p_buffer, std::uint64_t size)
{
    // engine does not require extra init data
    (void)p_buffer;
    (void)size;
    if (HEBENCH_API_VERSION_MAJOR != HEBENCH_API_VERSION_NEEDED_MAJOR
        || HEBENCH_API_VERSION_MINOR != HEBENCH_API_VERSION_NEEDED_MINOR
        || HEBENCH_API_VERSION_REVISION < HEBENCH_API_VERSION_NEEDED_REVISION
        //|| std::strcmp(HEBENCH_API_VERSION_BUILD, HEBENCH_API_VERSION_NEEDED_BUILD) != 0
    )
        throw hebench::cpp::HEBenchError(HEBERROR_MSG("Critical: Invalid HEBench API version detected."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);

    return ClearTextEngine::create();
}

void destroyEngine(BaseEngine *p)
{
    ClearTextEngine *_p = dynamic_cast<ClearTextEngine *>(p);
    ClearTextEngine::destroy(_p);
}

} // namespace cpp
} // namespace hebench

//---------------------
// class ExampleEngine
//---------------------

ClearTextEngine *ClearTextEngine::create()
{
    ClearTextEngine *p_retval = new ClearTextEngine();
    p_retval->init();
    return p_retval;
}

void ClearTextEngine::destroy(ClearTextEngine *p)
{
    if (p)
        delete p;
}

ClearTextEngine::ClearTextEngine()
{
}

ClearTextEngine::~ClearTextEngine()
{
}

void ClearTextEngine::init()
{
    // add any new error codes

    // add supported schemes

    addSchemeName(HEBENCH_HE_SCHEME_PLAIN, "Plain");

    // add supported security

    addSecurityName(HEBENCH_HE_SECURITY_NONE, "None");

    // add the all benchmark descriptors
    addBenchmarkDescription(std::make_shared<MatMult_L_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<MatMult_L_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<MatMult_L_Description<float>>());
    addBenchmarkDescription(std::make_shared<MatMult_L_Description<double>>());

    addBenchmarkDescription(std::make_shared<MatMult_O_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<MatMult_O_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<MatMult_O_Description<float>>());
    addBenchmarkDescription(std::make_shared<MatMult_O_Description<double>>());

    addBenchmarkDescription(std::make_shared<EltAdd_L_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<EltAdd_L_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<EltAdd_L_Description<float>>());
    addBenchmarkDescription(std::make_shared<EltAdd_L_Description<double>>());

    addBenchmarkDescription(std::make_shared<EltAdd_O_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<EltAdd_O_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<EltAdd_O_Description<float>>());
    addBenchmarkDescription(std::make_shared<EltAdd_O_Description<double>>());

    addBenchmarkDescription(std::make_shared<EltMult_L_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<EltMult_L_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<EltMult_L_Description<float>>());
    addBenchmarkDescription(std::make_shared<EltMult_L_Description<double>>());

    addBenchmarkDescription(std::make_shared<EltMult_O_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<EltMult_O_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<EltMult_O_Description<float>>());
    addBenchmarkDescription(std::make_shared<EltMult_O_Description<double>>());

    addBenchmarkDescription(std::make_shared<DotProd_L_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<DotProd_L_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<DotProd_L_Description<float>>());
    addBenchmarkDescription(std::make_shared<DotProd_L_Description<double>>());

    addBenchmarkDescription(std::make_shared<DotProd_O_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<DotProd_O_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<DotProd_O_Description<float>>());
    addBenchmarkDescription(std::make_shared<DotProd_O_Description<double>>());

    addBenchmarkDescription(std::make_shared<LogReg_L_Description<float>>(hebench::APIBridge::Workload::LogisticRegression));
    addBenchmarkDescription(std::make_shared<LogReg_L_Description<double>>(hebench::APIBridge::Workload::LogisticRegression));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<float>>(hebench::APIBridge::Workload::LogisticRegression));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<double>>(hebench::APIBridge::Workload::LogisticRegression));

    addBenchmarkDescription(std::make_shared<LogReg_L_Description<float>>(hebench::APIBridge::Workload::LogisticRegression_PolyD3));
    addBenchmarkDescription(std::make_shared<LogReg_L_Description<double>>(hebench::APIBridge::Workload::LogisticRegression_PolyD3));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<float>>(hebench::APIBridge::Workload::LogisticRegression_PolyD3));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<double>>(hebench::APIBridge::Workload::LogisticRegression_PolyD3));

    addBenchmarkDescription(std::make_shared<LogReg_L_Description<float>>(hebench::APIBridge::Workload::LogisticRegression_PolyD5));
    addBenchmarkDescription(std::make_shared<LogReg_L_Description<double>>(hebench::APIBridge::Workload::LogisticRegression_PolyD5));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<float>>(hebench::APIBridge::Workload::LogisticRegression_PolyD5));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<double>>(hebench::APIBridge::Workload::LogisticRegression_PolyD5));

    addBenchmarkDescription(std::make_shared<LogReg_L_Description<float>>(hebench::APIBridge::Workload::LogisticRegression_PolyD7));
    addBenchmarkDescription(std::make_shared<LogReg_L_Description<double>>(hebench::APIBridge::Workload::LogisticRegression_PolyD7));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<float>>(hebench::APIBridge::Workload::LogisticRegression_PolyD7));
    addBenchmarkDescription(std::make_shared<LogReg_O_Description<double>>(hebench::APIBridge::Workload::LogisticRegression_PolyD7));

    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_O_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_O_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_O_Description<float>>());
    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_O_Description<double>>());

    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_L_Description<std::int32_t>>());
    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_L_Description<std::int64_t>>());
    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_L_Description<float>>());
    addBenchmarkDescription(std::make_shared<SimpleSetIntersection_L_Description<double>>());
}
