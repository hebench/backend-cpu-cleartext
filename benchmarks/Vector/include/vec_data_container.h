
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_BE_VectorDataPack_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_BE_VectorDataPack_H_7e5fa8c2415240ea93eff148ed73539b

#include "clear_error.h"

#include "data_container.h"

template <class T>
/**
 * @brief Encapsulates data for a vector operation parameter.
 */
class VectorParamPack : public ClearDataPack<T>
{
private:
    HEBERROR_DECLARE_CLASS_NAME(VectorParamPack)

public:
    static constexpr std::int64_t tag = 0;

    /**
     * @brief VectorParamPack
     * @param[in] data_pack Data pack with the data for all vector samples.
     * @param[in] vector_size Number of elements in each vector sample.
     * @details Methods `setSample()` and `getSample()` still provide direct
     * access to the underlying data of the matrices.
     */
    explicit VectorParamPack(const hebench::APIBridge::DataPack &data_pack,
                             std::uint64_t vector_size);
    /**
     * @brief Creates a new data pack with specified data sizes.
     * @param[in] sample_count Number of data samples to be contained in this pack.
     * @param[in] p_sample_sizes List of number of items per data sample.
     * @param[in] position Parameter position corresponding to
     * hebench::APIBridge::DataPack::param_position.
     * @param[in] vector_size Number of elements in the vector
     * @details Methods `setSample()` and `getSample()` still provide
     * direct access to the underlying data of the matrices.
     */
    explicit VectorParamPack(std::uint64_t sample_count,
                             const std::uint64_t *p_sample_sizes,
                             std::uint64_t position,
                             std::uint64_t vector_size);

    ~VectorParamPack() override {}

    std::uint64_t getVecSize() const { return m_vector_size; }

    std::int64_t classTag() const override { return VectorParamPack::tag; }

private:
    std::uint64_t m_vector_size;
};

#include "inl/vec_data_container.inl"

#endif // defined _HEBench_ClearText_BE_VectorDataPack_H_7e5fa8c2415240ea93eff148ed73539b
