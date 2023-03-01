// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_Bench_SimpleSetIntersection_SRC_
#define _HEBench_Bench_SimpleSetIntersection_SRC_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <numeric>
#include <utility>

#include "../bench_simplesetint.h"
#include "benchmarks/Vector/include/vec_data_container.h"
#include "clear_engine.h"
#include "clear_error.h"

//-------------------------------------------
// class SimpleSetIntersection_Benchmark
//-------------------------------------------

template <class T>
inline SimpleSetIntersection_Benchmark<T>::SimpleSetIntersection_Benchmark(hebench::cpp::BaseEngine &engine,
                                                                           const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                                                                           const hebench::APIBridge::WorkloadParams &bench_params) :
    ClearTextBenchmark(engine, bench_desc)
{
    hebench::cpp::WorkloadParams::SimpleSetIntersection w_params(bench_params);

    switch (bench_desc.workload)
    {
    case hebench::APIBridge::Workload::SimpleSetIntersection:
        m_set_size_x  = w_params.n();
        m_set_size_y  = w_params.m();
        m_item_size_k = w_params.k();
        break;
    default:
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unsupported benchmark descriptor 'bench_desc'"),
                                         HEBENCH_ECODE_INVALID_ARGS);
        break;
    } // end switch
    m_workload = bench_desc.workload;
}

template <class T>
inline SimpleSetIntersection_Benchmark<T>::~SimpleSetIntersection_Benchmark()
{
}

template <class T>
inline hebench::APIBridge::Handle SimpleSetIntersection_Benchmark<T>::encode(const hebench::APIBridge::DataPackCollection *p_parameters)
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
    //for (std::size_t i = 0; i < internal_packed_params.size(); ++i)
    //{
    internal_packed_params[0] =
        this->getEngine().template createRAII<VectorParamPack<T>>(p_parameters->p_data_packs[0],
                                                                  m_set_size_x * m_item_size_k);
    internal_packed_params[1] =
        this->getEngine().template createRAII<VectorParamPack<T>>(p_parameters->p_data_packs[1],
                                                                  m_set_size_y * m_item_size_k);
    //} // end for

    return this->getEngine().template createHandle<decltype(p_internal_packed_params)>(sizeof(VectorParamPack<T>) * internal_packed_params.size(), VectorParamPack<T>::tag,
                                                                                       p_internal_packed_params);
}

template <class T>
inline void SimpleSetIntersection_Benchmark<T>::decode(hebench::APIBridge::Handle encoded_data,
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
inline hebench::APIBridge::Handle SimpleSetIntersection_Benchmark<T>::load(const hebench::APIBridge::Handle *p_local_data,
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
inline void SimpleSetIntersection_Benchmark<T>::store(hebench::APIBridge::Handle remote_data,
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
inline hebench::APIBridge::Handle SimpleSetIntersection_Benchmark<T>::operate(hebench::APIBridge::Handle h_remote_packed,
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

    // we have 3 parameters (DataPacks) of plain text inside our PackedParams

    const std::vector<std::shared_ptr<VectorParamPack<T>>> &params = internal_loaded_params.front();

    // compute actual number of results
    std::uint64_t result_sample_count = 1;
    for (std::size_t i = 0; i < params.size(); ++i)
        result_sample_count *= params[i]->getSamplesCount();
    // if X in Y or Y in X, there's one contained, so the maximun size is the minimum between the two sets.
    std::vector<std::uint64_t> result_sample_sizes(result_sample_count, std::min(m_set_size_x, m_set_size_y) * m_item_size_k);

    // allocate buffers for results
    std::shared_ptr<VectorParamPack<T>> p_result = std::make_shared<VectorParamPack<T>>(result_sample_count,
                                                                                        result_sample_sizes.data(),
                                                                                        0,
                                                                                        1);

    std::array<std::uint64_t, 2> param_i;
    std::array<gsl::span<T>, param_i.size()> data;
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

            SimpleSetIntersection(result_data, data[0], data[1], m_set_size_x, m_set_size_y, m_item_size_k);
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
bool SimpleSetIntersection_Benchmark<T>::isMemberOf(const T *dataset, const T *value, std::size_t n, std::size_t k)
{
    bool retval = false;
    for (size_t i = 0; !retval && i < n; ++i)
    {
        std::uint64_t members = 0;
        bool flag             = true;
        for (size_t j = 0; flag && j < k; ++j)
        {
            flag = dataset[(i * k) + j] == value[j];
            if (flag)
            {
                ++members;
            }
        }
        retval = members == k;
    }
    return retval;
}

template <class T>
void SimpleSetIntersection_Benchmark<T>::mySetIntersection(const T *dataset_X, const T *dataset_Y, gsl::span<T> &result,
                                                           std::size_t n, std::size_t m, std::size_t k)
{
    size_t idx_result = m - 1;
    for (size_t idx_x = 0; idx_x < n; ++idx_x)
    {
        if (isMemberOf(dataset_Y, dataset_X + (idx_x * k), m, k))
        {
            if (!isMemberOf(result.data(), dataset_X + (idx_x * k), m, k))
            {
                std::copy(dataset_X + (idx_x * k), dataset_X + (idx_x * k) + k, result.begin() + (idx_result * k));
                --idx_result;
            } // end if
        }
    }
}

template <class T>
void SimpleSetIntersection_Benchmark<T>::SimpleSetIntersection(gsl::span<T> &result,
                                                               const gsl::span<const T> &X, const gsl::span<const T> &Y,
                                                               std::size_t n, std::size_t m, std::size_t k)
{
    if (k == 0)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vectors in parameter 0 & 1, since k is 0."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }
    if (X.size() != n * k)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vector in parameter 0."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }
    if (Y.size() != m * k)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vector in parameter 1."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }
    if (result.size() != std::min(n, m) * k)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed vector for result."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }

    const T *m_X = X.data();
    const T *m_Y = Y.data();

    if (n > m)
    {
        mySetIntersection(m_X, m_Y, result, n, m, k);
    }
    else
    {
        mySetIntersection(m_Y, m_X, result, m, n, k);
    }
}

#endif // defined _HEBench_Bench_SimpleSetIntersection_SRC_
