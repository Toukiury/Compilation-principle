#pragma once

#include <string>

namespace ir {
    class Module;
} // namespace ir

namespace opt {
class Optimize {
public:
    Optimize() = default;
    virtual ~Optimize() = default;

    
    virtual void optimize(ir::Module& program) = 0;

    virtual std::string get_name() = 0;

}; 

} // namespace opt
