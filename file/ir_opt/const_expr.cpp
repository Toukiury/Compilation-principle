#include "ir_opt/const_expr.hpp"

#include "another/log.hpp"
#include "ir_opt/ir.hpp"
#include <memory>


namespace opt {

constexpr int kMaxRecursionDepth = 100;
ir::Module* current_module = nullptr;

/* Helper function to perform unary operations on literals */
std::shared_ptr<ir::Literal> PerformUnaryOp(std::shared_ptr<ir::Literal> operand, 
                                          ir::Instruction::OpID operation, 
                                          bool is_floating_point) 
{
    if (is_floating_point) {
        const double value = operand->get_real();
        switch(operation) {
            case ir::Instruction::OpID::Sub:
                return ir::Literal::make_literal(-value);
            case ir::Instruction::OpID::Null:
                return ir::Literal::make_literal(value);
            default:
                LOG_WARN("Unexpected floating point operation: %s on %f", 
                        ir::Instruction::op_to_string(operation).c_str(), 
                        value);
                break;
        }
    } 
    else {
        const long value = operand->get_int();
        switch(operation) {
            case ir::Instruction::OpID::Minus:
                return ir::Literal::make_literal(-value);
            case ir::Instruction::OpID::Not: 
            case ir::Instruction::OpID::BitReverse:
                return ir::Literal::make_literal(~value);
            case ir::Instruction::OpID::Null:
                return ir::Literal::make_literal(value);
            default:
                LOG_ERROR("Unsupported unary operation ID: %d", operation);
                break;
        }
    }
    return nullptr;
}

/* Helper function to perform binary operations on literals */
std::shared_ptr<ir::Literal> PerformBinaryOp(std::shared_ptr<ir::Literal> left, 
                                           std::shared_ptr<ir::Literal> right,
                                           ir::Instruction::OpID operation, 
                                           bool is_floating_point) 
{
    if (is_floating_point) {
        const double left_val = left->get_real();
        const double right_val = right->get_real();
        
        switch(operation) {
            case ir::Instruction::OpID::Add:  return ir::Literal::make_literal(left_val + right_val);
            case ir::Instruction::OpID::Sub:  return ir::Literal::make_literal(left_val - right_val);
            case ir::Instruction::OpID::Mul:  return ir::Literal::make_literal(left_val * right_val);
            case ir::Instruction::OpID::Div:  return ir::Literal::make_literal(left_val / right_val);
            default:
                LOG_WARN("Unexpected floating point operation: %f %s %f", 
                        left_val, 
                        ir::Instruction::op_to_string(operation).c_str(), 
                        right_val);
                break;
        }
    } 
    else {
        const long left_val = left->get_int();
        const long right_val = right->get_int();
        
        switch(operation) {
            case ir::Instruction::OpID::Add:  return ir::Literal::make_literal(left_val + right_val);
            case ir::Instruction::OpID::Sub:  return ir::Literal::make_literal(left_val - right_val);
            case ir::Instruction::OpID::Mul:  return ir::Literal::make_literal(left_val * right_val);
            case ir::Instruction::OpID::Div:  return ir::Literal::make_literal(left_val / right_val);
            case ir::Instruction::OpID::Mod:  return ir::Literal::make_literal(left_val % right_val);
            case ir::Instruction::OpID::And: 
            case ir::Instruction::OpID::AndThen: return ir::Literal::make_literal(left_val && right_val);
            case ir::Instruction::OpID::Or: 
            case ir::Instruction::OpID::OrElse:  return ir::Literal::make_literal(left_val || right_val);
            case ir::Instruction::OpID::Eq:   return ir::Literal::make_literal(left_val == right_val);
            case ir::Instruction::OpID::Ne:   return ir::Literal::make_literal(left_val != right_val);
            case ir::Instruction::OpID::Gt:   return ir::Literal::make_literal(left_val > right_val);
            case ir::Instruction::OpID::Ge:   return ir::Literal::make_literal(left_val >= right_val);
            case ir::Instruction::OpID::Lt:   return ir::Literal::make_literal(left_val < right_val);
            case ir::Instruction::OpID::Le:   return ir::Literal::make_literal(left_val <= right_val);
            default:
                LOG_ERROR("Unsupported binary operation ID: %d", operation);
                break;
        }
    }
    return nullptr;
}

/* Recursively optimize arithmetic instructions */
std::shared_ptr<ir::Value> OptimizeArithmeticExpr(std::shared_ptr<ir::Instruction> instruction, 
                                                 int recursion_depth) 
{
    if (!instruction->is_arith_inst() || recursion_depth > kMaxRecursionDepth) {
        return instruction;
    }

    if (instruction->is_binary_inst()) {
        // Optimize both operands first
        auto left_operand = instruction->get_operand(0).lock();
        auto right_operand = instruction->get_operand(1).lock();
        
        if (left_operand->is_inst()) {
            auto left_inst = std::dynamic_pointer_cast<ir::Instruction>(left_operand);
            if (left_inst->is_arith_inst()) {
                auto optimized_left = OptimizeArithmeticExpr(left_inst, recursion_depth + 1);
                if (optimized_left) {
                    instruction->set_operand(0, optimized_left);
                    current_module->all_values_.emplace_back(optimized_left);
                }
            }
        }
        
        if (right_operand->is_inst()) {
            auto right_inst = std::dynamic_pointer_cast<ir::Instruction>(right_operand);
            if (right_inst->is_arith_inst()) {
                auto optimized_right = OptimizeArithmeticExpr(right_inst, recursion_depth + 1);
                if (optimized_right) {
                    instruction->set_operand(1, optimized_right);
                    current_module->all_values_.emplace_back(optimized_right);
                }
            }
        }
        
        // Re-get the operands after optimization
        left_operand = instruction->get_operand(0).lock();
        right_operand = instruction->get_operand(1).lock();
        
        // If both are literals, compute the result
        if (left_operand->is_literal() && right_operand->is_literal()) {
            auto left_literal = std::dynamic_pointer_cast<ir::Literal>(left_operand);
            auto right_literal = std::dynamic_pointer_cast<ir::Literal>(right_operand);
            return PerformBinaryOp(left_literal, right_literal, 
                                 instruction->get_op_id(), 
                                 instruction->is_real());
        }
    } 
    else if (instruction->is_unary_inst()) {
        auto operand = instruction->get_operand(0).lock();
        
        // Handle parentheses specially
        if (instruction->op_id_ == ir::Instruction::OpID::Bracket) {
            OptimizeArithmeticExpr(std::dynamic_pointer_cast<ir::Instruction>(operand), 
                                 recursion_depth + 1);
        }
        
        if (operand->is_inst()) {
            auto operand_inst = std::dynamic_pointer_cast<ir::Instruction>(operand);
            if (operand_inst->is_arith_inst()) {
                auto optimized_operand = OptimizeArithmeticExpr(operand_inst, recursion_depth + 1);
                if (optimized_operand) {
                    instruction->set_operand(0, optimized_operand);
                    current_module->all_values_.emplace_back(optimized_operand);
                }
            }
        }
        
        operand = instruction->get_operand(0).lock();
        if (operand->is_literal()) {
            auto literal = std::dynamic_pointer_cast<ir::Literal>(operand);
            return PerformUnaryOp(literal, instruction->get_op_id(), 
                                instruction->is_real());
        }
    }
    
    return instruction;
}

void ConstExprOpt::optimize(ir::Module &program) {
    current_module = &program;
    
    for (auto &function : program.functions_) {
        for (auto &basic_block : function->basic_blocks_) {
            for (auto &instruction : basic_block->instructions_) {
                if (instruction->is_assign_inst()) {
                    auto left_value = instruction->get_operand(0).lock();
                    
                    if (left_value->is_inst()) {
                        auto left_inst = std::dynamic_pointer_cast<ir::Instruction>(left_value);
                        while (left_inst->is_array_visit_inst()) {
                            auto array_index = left_inst->get_operand(1).lock();
                            if (array_index->is_inst()) {
                                auto index_inst = std::dynamic_pointer_cast<ir::Instruction>(array_index);
                                if (index_inst->is_arith_inst()) {
                                    auto optimized_index = OptimizeArithmeticExpr(index_inst, 0);
                                    if (optimized_index) {
                                        left_inst->set_operand(1, optimized_index);
                                        program.all_values_.emplace_back(optimized_index);
                                    }
                                }
                            }
                            
                            left_value = left_inst->get_operand(0).lock();
                            if (left_value->is_inst()) {
                                left_inst = std::dynamic_pointer_cast<ir::Instruction>(left_value);
                            } else {
                                break;
                            }
                        }
                    }
                    
                    auto right_value = instruction->get_operand(1).lock();
                    if (right_value->is_inst()) {
                        auto right_inst = std::dynamic_pointer_cast<ir::Instruction>(right_value);
                        if (right_inst->is_arith_inst()) {
                            auto optimized_right = OptimizeArithmeticExpr(right_inst, 0);
                            if (optimized_right) {
                                instruction->set_operand(1, optimized_right);
                                program.all_values_.emplace_back(optimized_right);
                            }
                        }
                    }
                } 
                else if (instruction->is_br_inst()) {
                    auto condition = instruction->get_operand(0).lock();
                    if (condition->is_inst()) {
                        auto cond_inst = std::dynamic_pointer_cast<ir::Instruction>(condition);
                        if (cond_inst->is_arith_inst()) {
                            auto optimized_cond = OptimizeArithmeticExpr(cond_inst, 0);
                            if (optimized_cond) {
                                instruction->set_operand(0, optimized_cond);
                                program.all_values_.emplace_back(optimized_cond);
                            }
                        }
                    }
                } 
                else if (instruction->is_io_inst()) {
                    const unsigned num_operands = instruction->get_num_ops();
                    for (unsigned i = 0; i < num_operands; i++) {
                        auto operand = instruction->get_operand(i).lock();
                        if (operand->is_inst()) {
                            auto operand_inst = std::dynamic_pointer_cast<ir::Instruction>(operand);
                            
                            if (operand_inst->is_arith_inst()) {
                                auto optimized_operand = OptimizeArithmeticExpr(operand_inst, 0);
                                if (optimized_operand) {
                                    instruction->set_operand(i, optimized_operand);
                                    program.all_values_.emplace_back(optimized_operand);
                                }
                            } 
                            else if (operand_inst->is_array_visit_inst()) {
                                while (operand_inst->is_array_visit_inst()) {
                                    auto array_index = operand_inst->get_operand(1).lock();
                                    if (array_index->is_inst()) {
                                        auto index_inst = std::dynamic_pointer_cast<ir::Instruction>(array_index);
                                        if (index_inst->is_arith_inst()) {
                                            auto optimized_index = OptimizeArithmeticExpr(index_inst, 0);
                                            if (optimized_index) {
                                                operand_inst->set_operand(1, optimized_index);
                                                program.all_values_.emplace_back(optimized_index);
                                            }
                                        }
                                    }
                                    
                                    operand = operand_inst->get_operand(0).lock();
                                    if (operand->is_inst()) {
                                        operand_inst = std::dynamic_pointer_cast<ir::Instruction>(operand);
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

} // namespace opt
