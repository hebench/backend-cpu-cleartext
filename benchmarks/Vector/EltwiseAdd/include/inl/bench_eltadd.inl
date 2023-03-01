
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_Bench_EltAdd_SRC_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_Bench_EltAdd_SRC_7e5fa8c2415240ea93eff148ed73539b

#include <cassert>
#include <cstdint>
#include <cstring>
#include <numeric>
#include <utility>

#include "../bench_eltadd.h"
#include "benchmarks/Vector/include/vec_data_container.h"
#include "clear_engine.h"
#include "clear_error.h"

//-------------------------------------------
// class EltAdd_Benchmark
//-------------------------------------------

template <class T>
inline EltAdd_Benchmark<T>::EltAdd_Benchmark(hebench::cpp::BaseEngine &engine,
                                             const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                                             const hebench::APIBridge::WorkloadParams &bench_params) :
    ClearTextBenchmark(engine, bench_desc)
{
    hebench::cpp::WorkloadParams::EltwiseAdd w_params(bench_params);

    switch (bench_desc.workload)
    {
    case hebench::APIBridge::Workload::EltwiseAdd:
        m_vector_size = w_params.n();
        break;

    default:
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unsupported benchmark descriptor 'bench_desc'"),
                                         HEBENCH_ECODE_INVALID_ARGS);
        break;
    } // end switch
}

template <class T>
inline EltAdd_Benchmark<T>::~EltAdd_Benchmark()
{
}

template <class T>
inline hebench::APIBridge::Handle EltAdd_Benchmark<T>::encode(const hebench::APIBridge::DataPackCollection *p_parameters)
{
    if (p_parameters->pack_count > 0 && !p_parameters->p_data_packs)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null 'p_parameters'"),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    auto p_internal_packed_params =
        this->getEngine().template createRAII<std::vector<std::shared_ptr<VectorParamPack<T>>>>();

    // alias for simplicity
    std::vector<std::shared_ptr<VectorParamPack<T>>> &internal_packed_params = *p_internal_packed_params;

    internal_packed_params.resize(p_parameters->pack_count);
    for (std::size_t i = 0; i < internal_packed_params.size(); ++i)
    {
        internal_packed_params[i] =
            this->getEngine().template createRAII<VectorParamPack<T>>(p_parameters->p_data_packs[i],
                                                                      m_vector_size);
    } // end for

    return this->getEngine().template createHandle<decltype(p_internal_packed_params)>(sizeof(VectorParamPack<T>) * internal_packed_params.size(), VectorParamPack<T>::tag,
                                                                                       p_internal_packed_params);
}

template <class T>
inline void EltAdd_Benchmark<T>::decode(hebench::APIBridge::Handle encoded_data,
                                        hebench::APIBridge::DataPackCollection *p_native)
{
    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    // input:
    //  encoded_data <=> PackedParams->

    const std::vector<std::shared_ptr<VectorParamPack<T>>> &internal_packed_params =
        *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::shared_ptr<VectorParamPack<T>>>>>(encoded_data);

    std::uint64_t min_count = std::min(p_native->pack_count, internal_packed_params.size());
    for (std::uint64_t i = 0; i < min_count; ++i)
    {
        const VectorParamPack<T> &param = *internal_packed_params[i];
        param.fillHEBenchDataPack(p_native->p_data_packs[i], false);
    } // end for
}

template <class T>
inline hebench::APIBridge::Handle EltAdd_Benchmark<T>::load(const hebench::APIBridge::Handle *p_local_data,
                                                            std::uint64_t count)
{
    if (count != 1) // we only deal with plain text
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid number of handles. Expected 1."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    // input:
    //  PackedParams*, count
    // output:
    //  remote_data handle <=> vector<PackedParams>

    auto p_remote_packed_params = // vector<PackedParams>
        this->getEngine().template createRAII<std::vector<std::vector<std::shared_ptr<VectorParamPack<T>>>>>();

    // alias for simplicity
    std::vector<std::vector<std::shared_ptr<VectorParamPack<T>>>> &remote_packed_params = *p_remote_packed_params;

    remote_packed_params.resize(count);
    assert(remote_packed_params.size() == 1);

    for (std::size_t i = 0; i < remote_packed_params.size(); ++i)
    {
        // retrieve our internal format of a PackedParams from the next handle to local data
        const std::vector<std::shared_ptr<VectorParamPack<T>>> &local_packed_params =
            *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::shared_ptr<VectorParamPack<T>>>>>(p_local_data[i]);

        // transfer from local host to remote:
        // we do not have to transfer data to an actual remote device/server/etc,
        // so, just make a copy
        remote_packed_params[i] = local_packed_params;
    } // end for

    return this->getEngine().template createHandle<decltype(p_remote_packed_params)>(count, 0,
                                                                                     p_remote_packed_params);
}

template <class T>
inline void EltAdd_Benchmark<T>::store(hebench::APIBridge::Handle remote_data,
                                       hebench::APIBridge::Handle *p_local_data, std::uint64_t count)
{
    // remote_data handle <=> vector<PackedParams>
    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    // retrieve internal format from handle
    const std::vector<std::vector<std::shared_ptr<VectorParamPack<T>>>> &remote_packed_params_collection =
        *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::vector<std::shared_ptr<VectorParamPack<T>>>>>>(remote_data);

    // unpack every DataPackCollection and make a copy since host == remote for this backend
    std::uint64_t min_size = std::min(remote_packed_params_collection.size(), count);
    for (std::uint64_t i = 0; i < min_size; ++i)
    {
        auto p_local_packed_params =
            this->getEngine().template createRAII<std::vector<std::shared_ptr<VectorParamPack<T>>>>();

        // transfer from remote to local host:
        // we do not have to transfer data from an actual remote device/server/etc,
        // so, just make a copy
        *p_local_packed_params = remote_packed_params_collection[i];

        p_local_data[i] = this->getEngine().template createHandle<decltype(p_local_packed_params)>(count, 0,
                                                                                                   p_local_packed_params);
    } // end for

    // pad any remaining handles with null/0 as per specification of store()
    for (std::uint64_t i = min_size; i < count; ++i)
        std::memset(&p_local_data[i], 0, sizeof(hebench::APIBridge::Handle));
}

template <class T>
inline hebench::APIBridge::Handle EltAdd_Benchmark<T>::operate(hebench::APIBridge::Handle h_remote_packed,
                                                               const hebench::APIBridge::ParameterIndexer *p_param_indexers,
                                                               std::uint64_t indexers_count)
{
    assert(indexers_count >= NumOpParams);
    if (!p_param_indexers)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null parameter 'p_param_indexers"),
                                         HEBENCH_ECODE_INVALID_ARGS);

    const std::vector<std::vector<std::shared_ptr<VectorParamPack<T>>>> &internal_loaded_params =
        *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::vector<std::shared_ptr<VectorParamPack<T>>>>>>(h_remote_packed);

    if (internal_loaded_params.size() != 1)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid handle format. Expected 1 parameter pack loaded in handle."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // we have a single ParamPack because all parameters are plain text

    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>

    // we have 2 parameters (DataPacks) of plain text inside our PackedParams

    const std::vector<std::shared_ptr<VectorParamPack<T>>> &params = internal_loaded_params.front();

    // compute actual number of results
    std::uint64_t result_sample_count = 1;
    for (std::size_t i = 0; i < params.size(); ++i)
        result_sample_count *= params[i]->getSamplesCount();
    std::vector<std::uint64_t> result_sample_sizes(result_sample_count, m_vector_size);

    // allocate buffers for results
    std::shared_ptr<VectorParamPack<T>> p_result = std::make_shared<VectorParamPack<T>>(result_sample_count,
                                                                                        result_sample_sizes.data(),
                                                                                        0,
                                                                                        m_vector_size);

    std::array<std::uint64_t, 2> param_i;
    std::array<gsl::span<const T>, param_i.size()> data;
    std::uint64_t next_result = 0;

    for (param_i[0] = p_param_indexers[0].value_index;
         param_i[0] < p_param_indexers[0].value_index + p_param_indexers[0].batch_size;
         ++param_i[0])
    {
        for (param_i[1] = p_param_indexers[1].value_index;
             param_i[1] < p_param_indexers[1].value_index + p_param_indexers[1].batch_size;
             ++param_i[1])
        {
            for (std::size_t i = 0; i < param_i.size(); ++i)
                data[i] = params[i]->getSample(param_i[i]);

            gsl::span<T> result_data = p_result->getSample(next_result);

            eltAdd(result_data, data[0], data[1], m_vector_size);
            ++next_result;
        } // end for
    } // end for

    // make compatible output with store()
    std::vector<decltype(p_result)> tmp;
    tmp.push_back(p_result);

    auto p_retval =
        this->getEngine().template createRAII<std::vector<std::vector<decltype(p_result)>>>();
    p_retval->emplace_back(std::move(tmp));
    return this->getEngine().template createHandle<decltype(p_retval)>(p_retval->size(), 0,
                                                                       p_retval);
}

template <class T>
void EltAdd_Benchmark<T>::eltAdd(gsl::span<T> &result,
                                 const gsl::span<const T> &V0,
                                 const gsl::span<const T> &V1,
                                 std::size_t element_count)
{
    if (V0.size() != element_count)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vector in parameter 0."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    if (V1.size() != element_count)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vector in parameter 1."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    if (result.size() != element_count)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vector for result."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    for (std::size_t i = 0; i < element_count; ++i)
        result[i] = V0[i] + V1[i];
}

#endif // defined _HEBench_Bench_EltAdd_SRC_7e5fa8c2415240ea93eff148ed73539b
