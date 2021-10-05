
// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#ifndef _HEBench_ClearText_BE_Engine_H_7e5fa8c2415240ea93eff148ed73539b
#define _HEBench_ClearText_BE_Engine_H_7e5fa8c2415240ea93eff148ed73539b

#include "hebench/api_bridge/cpp/hebench.hpp"

#define HEBENCH_HE_SCHEME_PLAIN  0
#define HEBENCH_HE_SECURITY_NONE 0

class ClearTextEngine : public hebench::cpp::BaseEngine
{
private:
    HEBERROR_DECLARE_CLASS_NAME(ExampleEngine)

public:
    static ClearTextEngine *create();
    static void destroy(ClearTextEngine *p);

    ~ClearTextEngine() override;

protected:
    ClearTextEngine();

    void init() override;
};

#endif // defined _HEBench_ClearText_BE_Engine_H_7e5fa8c2415240ea93eff148ed73539b
