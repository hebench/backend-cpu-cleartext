
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_BE_MatrixDataPack_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_BE_MatrixDataPack_H_7e5fa8c2415240ea93eff148ed73539b

#include "clear_error.h"

#include "data_container.h"

template <class T>
/**
 * @brief Encapsulates data for a matrix multiplication parameter.
 * @details Matrix data is expected in row-major order.
 *
 * This class handles matrix data transpose during construction.
 */
class MatrixParamPack : public ClearDataPack<T>
{
private:
    HEBERROR_DECLARE_CLASS_NAME(MatrixParamPack)

public:
    //static constexpr std::int64_t tag = SampleDataPack<T>::tag | 0x1;
    static constexpr std::int64_t tag = 0;

    /**
     * @brief MatMultParamPack
     * @param[in] data_pack Data pack with the data for all matrices samples.
     * @param[in] rows_count Number of rows in the matrices.
     * @param[in] cols_count Number of columns in the matrices.
     * @param[in] transpose Specifies whether these matrices are to be transposed.
     * @details If \p transposed is `true`, then `getRowsCount()` and `getColsCount()`
     * will return \p cols_count and \p rows_count respectively. Methods
     * setSampleMatrix(), fillHEBenchDataPack() and fillFromHEBenchDataPack() will
     * fill out data transposed. This is, the data from \p data_pack will be
     * transposed based on the rows and columns specified by \p rows_count and
     * \p cols_count.
     * Methods `setSample()` and `getSample()` still provide direct
     * access to the underlying data of the matrices.
     */
    explicit MatrixParamPack(const hebench::APIBridge::DataPack &data_pack,
                             std::uint64_t rows_count, std::uint64_t cols_count,
                             bool transpose);
    /**
     * @brief Creates a new data pack with specified data sizes.
     * @param[in] sample_count Number of data samples to be contained in this pack.
     * @param[in] p_sample_sizes List of number of items per data sample.
     * @param[in] position Parameter position corresponding to
     * hebench::APIBridge::DataPack::param_position.
     * @param[in] rows_count Number of rows in the matrices.
     * @param[in] cols_count Number of columns in the matrices.
     * @param[in] transpose Specifies whether these matrices is transposed.
     * @details If \p transposed is `true`, then getRowsCount() and getColsCount()
     * will return \p cols_count and \p rows_count respectively. Methods
     * setSampleMatrix(), fillHEBenchDataPack() and fillFromHEBenchDataPack() will
     * fill out data transposed. Methods setSample() and getSample() still provide
     * direct, non-transposed access to the underlying data of the matrices.
     */
    explicit MatrixParamPack(std::uint64_t sample_count,
                             const std::uint64_t *p_sample_sizes,
                             std::uint64_t position,
                             std::uint64_t rows_count, std::uint64_t cols_count,
                             bool transpose);
    ~MatrixParamPack() override {}

    /**
     * @brief Fills pre-allocated HEBench data pack with the data contained in this
     * data pack.
     * @param data_pack[out] HEBench data pack where to store the data.
     * @param b_pad[in] If true, any extra space in \p data_pack will be padded with
     * zeroes. Otherwise, extra space will not be overwritten.
     * @details For non-transposed matrices, this performs a direct copy. Otherwise,
     * this performs a transpose of the matrices.
     */
    void fillHEBenchDataPack(hebench::APIBridge::DataPack &data_pack, bool b_pad) const override;
    // TODO:
    //void setSampleMatrix(std::uint64_t sample_index, gsl::span<const T> src);

    std::uint64_t getRowsCount() const { return m_rows_count; }
    std::uint64_t getColsCount() const { return m_cols_count; }
    bool isTransposed() const { return m_b_transposed; }

    std::int64_t classTag() const override { return MatrixParamPack::tag; }

protected:
    void fillFromHEBenchDataPack(const hebench::APIBridge::DataPack &data_pack) override;

private:
    std::uint64_t m_rows_count;
    std::uint64_t m_cols_count;
    bool m_b_transposed;

    void transpose(T *dst, gsl::span<const T> src,
                   std::uint64_t src_rows_count, std::uint64_t src_cols_count) const;
};

#include "inl/mat_data_container.inl"

#endif // defined _HEBench_ClearText_BE_MatrixDataPack_H_7e5fa8c2415240ea93eff148ed73539b
