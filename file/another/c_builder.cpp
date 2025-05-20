#include "another/c_builder.hpp"
#include "another/log.hpp"
#include "ir_opt/ir.hpp"
#include "another/settings.hpp"
#include "ir_opt/ir_gen.hpp"
#include <unordered_set>
#include <sstream>

namespace builder::c {

namespace {
    std::stringstream output;
    std::unordered_set<const ir::BasicBlock*> processed_bbs;

    bool is_special_block(const std::string& name) {
        return name == "body_basic_block" || 
               name == "then_basic_block" ||
               name == "else_basic_block" ||
               name == "body_begin_basic_block" ||
               name == "body_end_basic_block";
    }

    void handle_basic_block(std::string_view prefix, 
                          const std::shared_ptr<ir::BasicBlock>& bb, 
                          std::stringstream& out) {
        // Skip if already processed
        if (processed_bbs.find(bb.get()) != processed_bbs.end()) return;
        processed_bbs.insert(bb.get());

        // Handle empty blocks
        if (bb->instructions_.empty()) {
            if (bb->name_ == "body_basic_block" || bb->name_ == "then_basic_block") {
                out << "{}";
            }
            return;
        }

        // Handle block opening
        if (bb->name_ == "else_basic_block") {
            out << prefix << "else {\n";
        } else if (is_special_block(bb->name_)) {
            out << prefix << "{\n";
        }

        // Process instructions
        for (const auto& inst : bb->instructions_) {
            out << prefix << inst->print();
            out << (inst->op_id_ == ir::Instruction::OpID::Br ? "\n" : ";\n");
        }

        // Process successor blocks
        for (const auto& next : bb->succ_bbs_) {
            if (auto next_block = next.lock()) {
                handle_basic_block(std::string(prefix) + "\t", next_block, out);
            }
        }

        // Handle block closing
        if (is_special_block(bb->name_)) {
            out << prefix << "}\n";
        }
    }
} // anonymous namespace

void CBuilder::build(ir::Module& program) noexcept {
    std::stringstream out;
    
    // Add standard headers
    out << "#include <stdio.h>\n"
        << "#include <stdlib.h>\n"
        << "#include <sys/types.h>\n"
        << "#include <sys/wait.h>\n"
        << "#include <unistd.h>\n";

    // Process global variables
    for (const auto& global : program.global_identifiers_) {
        out << (global->is_const_ ? "const " : "")
            << global->type_->print() << " " << global->name_;
        
        if (global->is_const_) {
            out << " = " << global->init_val_->print();
        }
        out << ";\n";
    }

    // Process functions
    for (const auto& func : program.functions_) {
        out << func->print() << "\n{\n";

        // Process local variables
        for (const auto& local : func->local_identifiers_) {
            bool is_param = false;
            for (const auto& arg : func->args_) {
                if (arg->name_ == local->name_) {
                    is_param = true;
                    break;
                }
            }
            
            if (!is_param) {
                out << (local->is_const_ ? "const " : "")
                    << local->type_->print() << " " << local->name_;
                
                if (local->is_const_) {
                    out << " = " << local->init_val_->print();
                }
                out << ";\n";
            }
        }

        // Process basic blocks
        for (const auto& bb : func->basic_blocks_) {
            handle_basic_block("", bb, out);
        }

        // Function return
        if (func->print() == "int main()") {
            out << "return 0;\n";
        } else if (auto func_type = func->func_type_.lock()) {  // 正确处理weak_ptr
            if (func_type->result_->tid_ != ir::Type::VoidTID) {
                out << "return __;\n";
            }
        }
        out << "}\n";
    }

    code_ = out.str();
    LOG_DEBUG("CBuilder: code generated");
}

void CBuilder::output(std::ofstream& out) noexcept {
    out << code_;
}

} // namespace builder::c
