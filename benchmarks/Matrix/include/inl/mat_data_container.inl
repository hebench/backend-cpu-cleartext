
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_MM_Sample_Data_SRC_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_MM_Sample_Data_SRC_7e5fa8c2415240ea93eff148ed73539b

#include <algorithm>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "../mat_data_container.h"

template <class T>
inline MatrixParamPack<T>::MatrixParamPack(const hebench::APIBridge::DataPack &data_pack,
                                           std::uint64_t rows_count, std::uint64_t cols_count,
                                           bool transpose) :
    ClearDataPack<T>(data_pack.param_position),
    m_b_transposed(transpose)
{
    m_rows_count = (m_b_transposed ? cols_count : rows_count);
    m_cols_count = (m_b_transposed ? rows_count : cols_count);
    fillFromHEBenchDataPack(data_pack);
}

template <class T>
inline MatrixParamPack<T>::MatrixParamPack(std::uint64_t sample_count,
                                           const std::uint64_t *p_sample_sizes,
                                           std::uint64_t position,
                                           std::uint64_t rows_count, std::uint64_t cols_count,
                                           bool transpose) :
    ClearDataPack<T>(position),
    m_b_transposed(transpose)
{
    m_rows_count = (m_b_transposed ? cols_count : rows_count);
    m_cols_count = (m_b_transposed ? rows_count : cols_count);
    ClearDataPack<T>::allocateBuffers(sample_count, p_sample_sizes);
}

template <class T>
inline void MatrixParamPack<T>::transpose(T *dst, gsl::span<const T> src,
                                          std::uint64_t src_rows_count, std::uint64_t src_cols_count) const
{
    // transpose a matrix's data (in row-major)
    // dst is expected to be the same size as src.

    for (std::uint64_t r = 0; r < src_rows_count; ++r)
        for (std::uint64_t c = 0; c < src_cols_count; ++c)
        {
            std::size_t src_i = r * src_cols_count + c;
            std::size_t dst_i = c * src_rows_count + r;
            if (src_i < src.size() && dst_i < src.size())
                dst[dst_i] = src[src_i];
        } // end for
}

template <class T>
inline void MatrixParamPack<T>::fillFromHEBenchDataPack(const hebench::APIBridge::DataPack &data_pack)
{
    if (m_b_transposed)
    {
        this->allocateBuffers(data_pack);

        // transpose the data
        for (std::size_t i = 0; i < this->m_data.size(); ++i)
        {
            if (data_pack.p_buffers[i].size % sizeof(T) != 0)
                throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Expected data size must be multiple of data type."),
                                                 HEBENCH_ECODE_INVALID_ARGS);
            T *buffer = reinterpret_cast<T *>(data_pack.p_buffers[i].p);
            transpose(this->m_data[i].data(), gsl::span<T>(buffer, this->m_data[i].size()),
                      m_cols_count, m_rows_count); // transposed
        } // end for
    } // end if
    else
        ClearDataPack<T>::fillFromHEBenchDataPack(data_pack);
}

template <class T>
inline void MatrixParamPack<T>::fillHEBenchDataPack(hebench::APIBridge::DataPack &data_pack, bool b_pad) const
{
    if (m_b_transposed)
    {
        // transpose the data
        data_pack.param_position     = this->position();
        std::size_t min_buffer_count = std::min(static_cast<std::size_t>(data_pack.buffer_count), this->m_data.size());
        if (data_pack.buffer_count > 0 && !data_pack.p_buffers)
            throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid empty data pack."),
                                             HEBENCH_ECODE_INVALID_ARGS);

        for (std::size_t i = 0; i < min_buffer_count; ++i)
        {
            // copy as much as possible
            std::size_t min_data_size = std::min(static_cast<std::size_t>(data_pack.p_buffers[i].size), this->m_data[i].size() * sizeof(T));
            if (data_pack.p_buffers[i].size > 0 && !data_pack.p_buffers[i].p)
                throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid empty data buffer."),
                                                 HEBENCH_ECODE_INVALID_ARGS);

            const T *src_buffer = this->m_data[i].data();
            T *dst_buffer       = reinterpret_cast<T *>(data_pack.p_buffers[i].p);
            transpose(dst_buffer, gsl::span<const T>(src_buffer, min_data_size / sizeof(T)),
                      m_rows_count, m_cols_count); // transposed
            if (b_pad && min_data_size < data_pack.p_buffers[i].size)
            {
                // pad remaining space
                dst_buffer += min_data_size;
                std::fill_n(reinterpret_cast<std::byte *>(dst_buffer),
                            data_pack.p_buffers[i].size - min_data_size,
                            static_cast<std::byte>(0));
            } // end if
        } // end for

        if (b_pad)
            for (std::size_t i = min_buffer_count; i < data_pack.buffer_count; ++i)
                std::fill_n(reinterpret_cast<std::byte *>(data_pack.p_buffers[i].p),
                            data_pack.p_buffers[i].size, static_cast<std::byte>(0));
    } // end if
    else
        ClearDataPack<T>::fillHEBenchDataPack(data_pack, b_pad);
}

#endif // defined _HEBench_API_Bridge_MM_Sample_Data_H_7e5fa8c2415240ea93eff148ed73539b
