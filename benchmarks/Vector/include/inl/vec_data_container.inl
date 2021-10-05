
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_API_Bridge_EA_Sample_Data_SRC_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_API_Bridge_EA_Sample_Data_SRC_7e5fa8c2415240ea93eff148ed73539b

#include <algorithm>

#include "hebench/api_bridge/cpp/hebench.hpp"

#include "../vec_data_container.h"

template <class T>
inline VectorParamPack<T>::VectorParamPack(const hebench::APIBridge::DataPack &data_pack,
                                           std::uint64_t vector_size) :
    ClearDataPack<T>(data_pack.param_position)
{
    m_vector_size = vector_size;
    ClearDataPack<T>::fillFromHEBenchDataPack(data_pack);
}

template <class T>
inline VectorParamPack<T>::VectorParamPack(std::uint64_t sample_count,
                                           const std::uint64_t *p_sample_sizes,
                                           std::uint64_t position,
                                           std::uint64_t vector_size) :
    ClearDataPack<T>(position)
{
    m_vector_size = vector_size;
    ClearDataPack<T>::allocateBuffers(sample_count, p_sample_sizes);
}

#endif // defined _HEBench_API_Bridge_EA_Sample_Data_H_7e5fa8c2415240ea93eff148ed73539b
