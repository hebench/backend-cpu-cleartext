
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_Sample_Data_SRC_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_Sample_Data_SRC_7e5fa8c2415240ea93eff148ed73539b

#include <algorithm>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "../data_container.h"

template <class T>
inline ClearDataPack<T>::ClearDataPack(const hebench::APIBridge::DataPack &data_pack) :
    ClearDataPack<T>(data_pack.param_position)
{
    fillFromHEBenchDataPack(data_pack);
}

template <class T>
inline ClearDataPack<T>::ClearDataPack(std::uint64_t sample_count,
                                       const std::uint64_t *p_sample_sizes,
                                       std::uint64_t position) :
    ClearDataPack<T>(position)
{
    allocateBuffers(sample_count, p_sample_sizes);
}

template <class T>
inline void ClearDataPack<T>::allocateBuffers(std::uint64_t sample_count,
                                              const std::uint64_t *p_sample_sizes)
{
    m_data.resize(sample_count);
    for (std::size_t i = 0; i < m_data.size(); ++i)
        m_data[i].resize(p_sample_sizes[i]);
}

template <class T>
inline void ClearDataPack<T>::allocateBuffers(const hebench::APIBridge::DataPack &data_pack)
{
    m_data.resize(data_pack.buffer_count);
    for (std::size_t i = 0; i < m_data.size(); ++i)
        m_data[i].resize(data_pack.p_buffers[i].size / sizeof(T));
}

template <class T>
inline void ClearDataPack<T>::fillFromHEBenchDataPack(const hebench::APIBridge::DataPack &data_pack)
{
    m_position = data_pack.param_position;
    allocateBuffers(data_pack);
    for (std::size_t i = 0; i < m_data.size(); ++i)
    {
        if (data_pack.p_buffers[i].size % sizeof(T) != 0)
            throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Expected data size must be multiple of data type."),
                                             HEBENCH_ECODE_INVALID_ARGS);
        T *buffer = reinterpret_cast<T *>(data_pack.p_buffers[i].p);
        std::copy(buffer, buffer + m_data[i].size(), m_data[i].data());
    } // end for
}

template <class T>
inline void ClearDataPack<T>::fillHEBenchDataPack(hebench::APIBridge::DataPack &data_pack, bool b_pad) const
{
    data_pack.param_position     = m_position;
    std::size_t min_buffer_count = std::min(static_cast<std::size_t>(data_pack.buffer_count), m_data.size());
    if (data_pack.buffer_count > 0 && !data_pack.p_buffers)
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid empty data pack."),
                                         HEBENCH_ECODE_INVALID_ARGS);

    for (std::size_t i = 0; i < min_buffer_count; ++i)
    {
        // copy as much as possible
        std::size_t min_data_size = std::min(static_cast<std::size_t>(data_pack.p_buffers[i].size), m_data[i].size() * sizeof(T));
        if (data_pack.p_buffers[i].size > 0 && !data_pack.p_buffers[i].p)
            throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Invalid empty data buffer."),
                                             HEBENCH_ECODE_INVALID_ARGS);

        const std::byte *src_buffer = reinterpret_cast<const std::byte *>(m_data[i].data());
        std::byte *dst_buffer       = reinterpret_cast<std::byte *>(data_pack.p_buffers[i].p);
        std::copy_n(src_buffer, min_data_size, dst_buffer);
        if (b_pad && min_data_size < data_pack.p_buffers[i].size)
        {
            // pad remaining space
            dst_buffer += min_data_size;
            std::fill_n(dst_buffer,
                        data_pack.p_buffers[i].size - min_data_size,
                        static_cast<std::byte>(0));
        } // end if
    } // end for

    if (b_pad)
        for (std::size_t i = min_buffer_count; i < data_pack.buffer_count; ++i)
            std::fill_n(reinterpret_cast<std::byte *>(data_pack.p_buffers[i].p),
                        data_pack.p_buffers[i].size, static_cast<std::byte>(0));
}

template <class T>
inline gsl::span<T> ClearDataPack<T>::getSample(std::uint64_t sample_index)
{
    gsl::span<T> retval;
    try
    {
        std::vector<T> &sample_data = m_data.at(static_cast<std::size_t>(sample_index));
        retval                      = gsl::span<T>(sample_data.data(), sample_data.size());
    }
    catch (std::out_of_range &)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Index is out of range."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }
    catch (std::exception &ex)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS(ex.what()),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }
    catch (...)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unexpected error."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }

    return retval;
}

template <class T>
inline gsl::span<const T> ClearDataPack<T>::getSample(std::uint64_t sample_index) const
{
    gsl::span<const T> retval;
    try
    {
        const std::vector<T> &sample_data = m_data.at(static_cast<std::size_t>(sample_index));
        retval                            = gsl::span<const T>(sample_data);
    }
    catch (std::out_of_range &)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Index is out of range."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }
    catch (std::exception &ex)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS(ex.what()),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }
    catch (...)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unexpected error."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }

    return retval;
}

template <class T>
inline void ClearDataPack<T>::setSample(std::uint64_t sample_index, const gsl::span<T> &sample_data)
{
    try
    {
        std::vector<T> &dst_sample_data = m_data.at(sample_index);
        dst_sample_data.assign(sample_data.begin(), sample_data.end());
    }
    catch (std::out_of_range &)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Index is out of range."),
                                         HEBENCH_ECODE_INVALID_ARGS);
    }
    catch (std::exception &ex)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS(ex.what()),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }
    catch (...)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unexpected error."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }
}

template <class T>
inline void ClearDataPack<T>::setSample(std::uint64_t sample_index, const gsl::span<T> &sample_data, const T &pad_value)
{
    try
    {
        gsl::span<T> dst_sample_data = getSample(sample_index);
        std::size_t min_data_count   = std::min(sample_data.size(), dst_sample_data.size());
        std::copy_n(sample_data.data(), min_data_count, dst_sample_data.data());
        // check if padding needed
        if (min_data_count < dst_sample_data.size())
            std::fill_n(dst_sample_data.data() + min_data_count,
                        dst_sample_data.size() - min_data_count,
                        pad_value);
    }
    catch (hebench::cpp::HEBenchError &)
    {
        throw;
    }
    catch (std::exception &ex)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS(ex.what()),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }
    catch (...)
    {
        throw hebench::cpp::HEBenchError(HEBERROR_MSG_CLASS("Unexpected error."),
                                         HEBENCH_ECODE_CRITICAL_ERROR);
    }
}

#endif // defined _HEBench_API_Bridge_Sample_Data_H_7e5fa8c2415240ea93eff148ed73539b
