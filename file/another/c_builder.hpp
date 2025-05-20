#pragma once

#include "another/builder.hpp"
#include <string>
#include <fstream>

namespace builder::c {

class CBuilder final : public Builder {
public:
    void build(ir::Module& program) noexcept override;
    void output(std::ofstream& out) noexcept override;

private:
    std::string code_;
};

} // namespace builder::c
