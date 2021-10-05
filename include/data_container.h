
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_BE_DataPack_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_BE_DataPack_H_7e5fa8c2415240ea93eff148ed73539b

#include <memory>
#include <vector>

#include <gsl/gsl>

#include "clear_error.h"

#include "hebench/api_bridge/types.h"

#include "hebench/api_bridge/cpp/hebench.hpp"

template <class T>
/**
 * @brief Encapsulates a HEBench DataPack.
 */
class ClearDataPack : public hebench::cpp::ITaggedObject
{
private:
    HEBERROR_DECLARE_CLASS_NAME(ClearDataPack)

public:
    explicit ClearDataPack(const hebench::APIBridge::DataPack &data_pack);
    /**
     * @brief Creates a new data pack with specified data sizes.
     * @param sample_count[in] Number of data samples to be contained in this pack.
     * @param p_sample_sizes[in] List of number of items per data sample.
     * @param position[in] Parameter position corresponding to hebench::APIBridge::DataPack::param_position.
     * @details This constructor initializes vector<vector<T>> data structure of the shape
     * @code
     * data = vector<vector<T>>(sample_count)
     * @endcode
     * and
     * @code
     * data[i] = vector<T>(p_sample_sizes[i])
     * @endcode
     */
    explicit ClearDataPack(std::uint64_t sample_count,
                           const std::uint64_t *p_sample_sizes,
                           std::uint64_t position);
    ~ClearDataPack() override {}

    /**
     * @brief Fills pre-allocated HEBench data pack with the data contained in this
     * data pack.
     * @param data_pack[out] HEBench data pack where to store the data.
     * @param b_pad[in] If true, any extra space in \p data_pack will be padded with
     * zeroes. Otherwise, extra space will not be overwritten.
     * @details This method is the inverse of the constructor. Thus, if the constructor
     * performs some type of conversion, it must be reversible, and this method
     * reverses the conversion.
     *
     * Default implementation is a simple copy of the data.
     */
    virtual void fillHEBenchDataPack(hebench::APIBridge::DataPack &data_pack, bool b_pad) const;

    std::uint64_t getSamplesCount() const { return m_data.size(); }
    gsl::span<T> getSample(std::uint64_t sample_index);
    gsl::span<const T> getSample(std::uint64_t sample_index) const;
    /**
     * @brief Copies the data contained in \p sample_data into this data pack's
     * sample specified by \p sample index.
     * @param sample_index[in] Index of data sample in this data pack to overwrite.
     * @param sample_data[in] Data to copy into this data pack sample.
     * @details The size of this data pack sample will be resized to match the
     * size of specified \p sample-data.
     */
    void setSample(std::uint64_t sample_index, const gsl::span<T> &sample_data);
    /**
     * @brief Copies the data contained in \p sample_data into this data pack's
     * sample specified by \p sample index.
     * @param sample_index[in] Index of data sample in this data pack to overwrite.
     * @param sample_data[in] Data to copy into this data pack sample.
     * @param pad_value[in] Value to be used for padding if needed.
     * @details The size of this data pack sample will remain constant. If not
     * enough space, this method will copy as much data as it can from \p sample_data
     * into the specified sample. If this data pack's sample's size is more than
     * the supplied data in \p sample_data, the remaining space will be padded
     * with the specified padding value \p pad_value.
     */
    void setSample(std::uint64_t sample_index, const gsl::span<T> &sample_data, const T &pad_value);

    std::uint64_t position() const { return m_position; }

protected:
    std::vector<std::vector<T>> m_data;

    ClearDataPack(std::uint64_t position = 0) :
        m_position(position) {}
    void allocateBuffers(std::uint64_t sample_count, const std::uint64_t *p_sample_sizes);
    void allocateBuffers(const hebench::APIBridge::DataPack &data_pack);
    /**
     * @brief Performs a simple copy.
     * @param data_pack
     */
    virtual void fillFromHEBenchDataPack(const hebench::APIBridge::DataPack &data_pack);

private:
    std::uint64_t m_position;
};

#include "inl/data_container.inl"

#endif // defined _HEBench_ClearText_BE_DataPack_H_7e5fa8c2415240ea93eff148ed73539b
