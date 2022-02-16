
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_Bench_MatMult_SRC_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_Bench_MatMult_SRC_7e5fa8c2415240ea93eff148ed73539b

#include <cassert>
#include <cstdint>
#include <cstring>
#include <numeric>
#include <utility>

#include "../bench_matmult.h"
#include "benchmarks/Matrix/include/mat_data_container.h"
#include "clear_engine.h"
#include "clear_error.h"

//-------------------------------------------
// class MatMult_Benchmark
//-------------------------------------------

template <class T>
inline MatMult_Benchmark<T>::MatMult_Benchmark(hebench::cpp::BaseEngine &engine,
                                               const hebench::APIBridge::BenchmarkDescriptor &bench_desc,
                                               const hebench::APIBridge::WorkloadParams &bench_params) :
    ClearTextBenchmark(engine, bench_desc)
{
    hebench::cpp::WorkloadParams::MatrixMultiply w_params(bench_params);

    switch (bench_desc.workload)
    {
    case hebench::APIBridge::Workload::MatrixMultiply:
        m_rows[0] = w_params.rows_M0();
        m_cols[0] = w_params.cols_M0();
        m_rows[1] = w_params.cols_M0();
        m_cols[1] = w_params.cols_M1();
        break;
    default:
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unsupported benchmark descriptor 'bench_desc'"),
                                         HEBENCH_ECODE_INVALID_ARGS);
        break;
    } // end switch
}

template <class T>
inline MatMult_Benchmark<T>::~MatMult_Benchmark()
{
}

template <class T>
inline hebench::APIBridge::Handle MatMult_Benchmark<T>::encode(const hebench::APIBridge::PackedData *p_parameters)
{
    if (p_parameters->pack_count > 0 && !p_parameters->p_data_packs)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null 'p_parameters'"),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    auto p_internal_packed_params =
        this->getEngine().template createRAII<std::vector<std::shared_ptr<MatrixParamPack<T>>>>();

    // alias for simplicity
    std::vector<std::shared_ptr<MatrixParamPack<T>>> &internal_packed_params = *p_internal_packed_params;

    internal_packed_params.resize(p_parameters->pack_count);
    for (std::size_t i = 0; i < internal_packed_params.size(); ++i)
    {
        internal_packed_params[i] =
            this->getEngine().template createRAII<MatrixParamPack<T>>(p_parameters->p_data_packs[i],
                                                                      m_rows[i], m_cols[i],
                                                                      p_parameters->p_data_packs[i].param_position == 1);
    } // end for

    return this->getEngine().template createHandle<decltype(p_internal_packed_params)>(sizeof(MatrixParamPack<T>) * internal_packed_params.size(), MatrixParamPack<T>::tag,
                                                                                       p_internal_packed_params);
}

template <class T>
inline void MatMult_Benchmark<T>::decode(hebench::APIBridge::Handle encoded_data,
                                         hebench::APIBridge::PackedData *p_native)
{
    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    // input:
    //  encoded_data <=> PackedParams->

    const std::vector<std::shared_ptr<MatrixParamPack<T>>> &internal_packed_params =
        *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::shared_ptr<MatrixParamPack<T>>>>>(encoded_data);

    std::uint64_t min_count = std::min(p_native->pack_count, internal_packed_params.size());
    for (std::uint64_t i = 0; i < min_count; ++i)
    {
        const MatrixParamPack<T> &param = *internal_packed_params[i];
        param.fillHEBenchDataPack(p_native->p_data_packs[i], false);
    } // end for
}

template <class T>
inline hebench::APIBridge::Handle MatMult_Benchmark<T>::load(const hebench::APIBridge::Handle *p_local_data,
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
        this->getEngine().template createRAII<std::vector<std::vector<std::shared_ptr<MatrixParamPack<T>>>>>();

    // alias for simplicity
    std::vector<std::vector<std::shared_ptr<MatrixParamPack<T>>>> &remote_packed_params = *p_remote_packed_params;

    remote_packed_params.resize(count);
    assert(remote_packed_params.size() == 1);

    for (std::size_t i = 0; i < remote_packed_params.size(); ++i)
    {
        // retrieve our internal format of a PackedParams from the next handle to local data
        const std::vector<std::shared_ptr<MatrixParamPack<T>>> &local_packed_params =
            *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::shared_ptr<MatrixParamPack<T>>>>>(p_local_data[i]);

        // transfer from local host to remote:
        // we do not have to transfer data to an actual remote device/server/etc,
        // so, just make a copy
        remote_packed_params[i] = local_packed_params;
    } // end for

    return this->getEngine().template createHandle<decltype(p_remote_packed_params)>(count, 0,
                                                                                     p_remote_packed_params);
}

template <class T>
inline void MatMult_Benchmark<T>::store(hebench::APIBridge::Handle remote_data,
                                        hebench::APIBridge::Handle *p_local_data, std::uint64_t count)
{
    // remote_data handle <=> vector<PackedParams>
    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>
    // DataPack <=> MatrixParamPack

    // retrieve internal format from handle
    const std::vector<std::vector<std::shared_ptr<MatrixParamPack<T>>>> &remote_packed_params_collection =
        *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::vector<std::shared_ptr<MatrixParamPack<T>>>>>>(remote_data);

    // unpack every PackedData and make a copy since host == remote for this backend
    std::uint64_t min_size = std::min(remote_packed_params_collection.size(), count);
    for (std::uint64_t i = 0; i < min_size; ++i)
    {
        auto p_local_packed_params =
            this->getEngine().template createRAII<std::vector<std::shared_ptr<MatrixParamPack<T>>>>();

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
inline hebench::APIBridge::Handle MatMult_Benchmark<T>::operate(hebench::APIBridge::Handle h_remote_packed,
                                                                const hebench::APIBridge::ParameterIndexer *p_param_indexers)
{
    if (!p_param_indexers)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid null parameter 'p_param_indexers"),
                                         HEBENCH_ECODE_INVALID_ARGS);

    const std::vector<std::vector<std::shared_ptr<MatrixParamPack<T>>>> &internal_loaded_params =
        *this->getEngine().retrieveFromHandle<std::shared_ptr<std::vector<std::vector<std::shared_ptr<MatrixParamPack<T>>>>>>(h_remote_packed);

    if (internal_loaded_params.size() != 1)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid handle format. Expected 1 parameter pack loaded in handle."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    // we have a single ParamPack because all parameters are plain text

    // PackedParams <=> std::vector<std::shared_ptr<MatrixParamPack>>

    // we have 2 parameters (DataPacks) of plain text inside our PackedParams

    const std::vector<std::shared_ptr<MatrixParamPack<T>>> &params = internal_loaded_params.front();

    // compute actual number of results

    std::uint64_t result_sample_count = 1;
    for (std::size_t i = 0; i < params.size(); ++i)
        result_sample_count *= params[i]->getSamplesCount();
    std::vector<std::uint64_t> result_sample_sizes(result_sample_count, m_rows[0] * m_cols[1]);

    // allocate buffers for results
    std::shared_ptr<MatrixParamPack<T>> p_result = std::make_shared<MatrixParamPack<T>>(result_sample_count,
                                                                                        result_sample_sizes.data(),
                                                                                        0,
                                                                                        m_rows[0], m_cols[1],
                                                                                        false);

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

            matMult(result_data, data[0], data[1], m_cols[0]);
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
void MatMult_Benchmark<T>::matMult(gsl::span<T> &result,
                                   const gsl::span<const T> &M0,
                                   const gsl::span<const T> &M1,
                                   std::size_t columns)
{
    if (M0.size() % columns != 0)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed matrix in parameter 0."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    if (M1.size() % columns != 0)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed matrix in parameter 1."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    if ((result.size() % (M0.size() / columns) != 0) || (result.size() % (M1.size() / columns) != 0))
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Malformed matrix for result."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    const T *row0 = M0.data();
    const T *row1 = M1.data();

    std::size_t result_i = 0;
    for (std::uint64_t offset0 = 0; offset0 < M0.size(); offset0 += columns)
    {
        for (std::uint64_t offset1 = 0; offset1 < M1.size(); offset1 += columns)
        {
            result[result_i] = std::inner_product(row0 + offset0, row0 + offset0 + columns,
                                                  row1 + offset1, static_cast<T>(0));

            ++result_i;
        } // end for
    } // end for
}

#endif // defined _HEBench_Bench_MatMult_SRC_7e5fa8c2415240ea93eff148ed73539b
