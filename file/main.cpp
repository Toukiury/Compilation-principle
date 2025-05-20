#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_map>

#include "another/thpool.hpp"
#include "another/log.hpp"
#include "ir_opt/opt.hpp"
#include "ast/ast_stmt.hpp"
#include "ast/stmt_print.hpp" // 包含 printSyntaxTree 声明
#include "ir_opt/ir_gen.hpp"
#include "another/settings.hpp"
#include "another/c_builder.hpp"
#include "ir_opt/const.hpp"
#include "lexical_pasrser/yacc.hpp"

int code_parse(const char *code, ProgramStmt **program_stmt);

// 简单的辅助函数，用于生成Token文件
bool generateTokenFile(const std::string& source_code, const std::string& output_path) {
    // 获取Token文件路径（将.c扩展名替换为.tokens）
    std::string token_path = output_path;
    size_t pos = token_path.find_last_of('.');
    if (pos != std::string::npos) {
        token_path = token_path.substr(0, pos) + ".tokens";
    } else {
        token_path += ".tokens";
    }
    
    // 创建Token文件
    std::ofstream token_file(token_path);
    if (!token_file.is_open()) {
        LOG_ERROR("无法创建Token文件: %s", token_path.c_str());
        return false;
    }
    
    // 写入文件头部
    token_file << "# PASCC词法分析器 - Token流\n\n";
    token_file << "源文件: " << G_SETTINGS.input_file << "\n\n";
    
    // 简单解析Pascal代码并生成token
    std::vector<std::string> tokens;
    
    // 按行读取源代码
    std::istringstream stream(source_code);
    std::string line;
    int line_num = 1;
    
    // Pascal关键字表
    const std::unordered_map<std::string, bool> keywords = {
        {"program", true}, {"begin", true}, {"end", true}, {"if", true},
        {"then", true}, {"else", true}, {"var", true}, {"integer", true},
        {"real", true}, {"boolean", true}, {"char", true}, {"string", true},
        {"procedure", true}, {"function", true}, {"for", true}, {"to", true},
        {"downto", true}, {"while", true}, {"do", true}, {"array", true},
        {"of", true}, {"record", true}, {"case", true}, {"type", true},
        {"const", true}
    };
    
    // 处理每行代码
    while (std::getline(stream, line)) {
        bool in_string = false;
        bool in_comment = false;
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            // 处理注释
            if (c == '{' && !in_string) {
                in_comment = true;
                continue;
            } else if (c == '}' && in_comment) {
                in_comment = false;
                continue;
            }
            
            // 在注释中跳过
            if (in_comment) continue;
            
            // 处理字符串
            if (c == '\'') {
                in_string = !in_string;
                if (in_string) {
                    // 开始字符串
                    size_t start = i;
                    ++i;
                    while (i < line.length() && line[i] != '\'') ++i;
                    
                    if (i < line.length() && line[i] == '\'') {
                        std::string str_val = line.substr(start, i - start + 1);
                        std::stringstream token;
                        token << "STRING|" << str_val << "|" << line_num << "|" << (start + 1);
                        tokens.push_back(token.str());
                    }
                }
                continue;
            }
            
            // 处理标识符和关键字
            if (std::isalpha(c) || c == '_') {
                size_t start = i;
                while (i < line.length() && (std::isalnum(line[i]) || line[i] == '_')) ++i;
                
                std::string identifier = line.substr(start, i - start);
                std::string lower_identifier = identifier;
                std::transform(lower_identifier.begin(), lower_identifier.end(), 
                               lower_identifier.begin(), ::tolower);
                
                std::stringstream token;
                if (keywords.find(lower_identifier) != keywords.end()) {
                    token << "KEYWORD|" << identifier << "|" << line_num << "|" << (start + 1);
                } else {
                    token << "IDENTIFIER|" << identifier << "|" << line_num << "|" << (start + 1);
                }
                tokens.push_back(token.str());
                
                --i; // 回退，因为循环会++i
                continue;
            }
            
            // 处理数字
            if (std::isdigit(c)) {
                size_t start = i;
                bool is_real = false;
                
                while (i < line.length() && (std::isdigit(line[i]) || line[i] == '.')) {
                    if (line[i] == '.') {
                        is_real = true;
                    }
                    ++i;
                }
                
                std::string number = line.substr(start, i - start);
                std::stringstream token;
                if (is_real) {
                    token << "REAL|" << number << "|" << line_num << "|" << (start + 1);
                } else {
                    token << "INTEGER|" << number << "|" << line_num << "|" << (start + 1);
                }
                tokens.push_back(token.str());
                
                --i; // 回退，因为循环会++i
                continue;
            }
            
            // 处理特殊符号
            if (i + 1 < line.length()) {
                std::string op = line.substr(i, 2);
                if (op == ":=" || op == "<>" || op == "<=" || op == ">=" || op == "..") {
                    std::string token_type;
                    if (op == ":=") token_type = "ASSIGN";
                    else if (op == "<>") token_type = "NE";
                    else if (op == "<=") token_type = "LE";
                    else if (op == ">=") token_type = "GE";
                    else if (op == "..") token_type = "RANGE";
                    
                    std::stringstream token;
                    token << token_type << "|" << op << "|" << line_num << "|" << (i + 1);
                    tokens.push_back(token.str());
                    ++i; // 跳过第二个字符
                    continue;
                }
            }
            
            // 处理单字符符号
            if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ||
                c == '<' || c == '>' || c == '(' || c == ')' || c == '[' ||
                c == ']' || c == ',' || c == '.' || c == ';' || c == ':') {
                std::stringstream token;
                token << "SYMBOL|" << c << "|" << line_num << "|" << (i + 1);
                tokens.push_back(token.str());
            }
        }
        
        ++line_num;
    }
    
    // 写入表格头
    token_file << "| 行号 | 列号 | Token类型 | 值 |\n";
    token_file << "|------|------|-----------|----|\n";
    
    // 写入token信息
    for (const auto& token_str : tokens) {
        std::stringstream ss(token_str);
        std::string type, value, line, col;
        
        std::getline(ss, type, '|');
        std::getline(ss, value, '|');
        std::getline(ss, line, '|');
        std::getline(ss, col, '|');
        
        token_file << "| " << line << " | " << col << " | " << type << " | " << value << " |\n";
    }
    
    // 更新文件开头添加token总数
    long file_pos = token_file.tellp();
    token_file.seekp(0, std::ios::beg);
    token_file << "# PASCC词法分析器 - Token流\n\n";
    token_file << "源文件: " << G_SETTINGS.input_file << "\n";
    token_file << "Token总数: " << tokens.size() << "\n\n";
    token_file.seekp(file_pos);
    
    token_file.close();
    LOG_INFO("Token文件已生成: %s (总共 %d 个tokens)", token_path.c_str(), (int)tokens.size());
    return true;
}

void init_env()
{
    if (G_SETTINGS.output_file.empty())
    {
        size_t pos = G_SETTINGS.input_file.find_last_of('.');
        if (pos == std::string::npos)
        {
            G_SETTINGS.output_file = G_SETTINGS.input_file + ".c";
        }
        else
        {
            G_SETTINGS.output_file = G_SETTINGS.input_file.substr(0, pos) + ".c";
        }
        pos = G_SETTINGS.input_file.find_last_of("/\\");
        std::string filename;
        if (pos != std::string::npos)
            filename = G_SETTINGS.input_file.substr(pos + 1);
        else
            filename = G_SETTINGS.input_file;

        

        LOG_DEBUG("Output file: %s", G_SETTINGS.output_file.c_str());
        // 检测文件是否存在
        // std::ifstream file(G_SETTINGS.output_file);
        // if (file.is_open())
        // {
        //     if (file.peek() != std::ifstream::traits_type::eof())
        //     {
        //         // 文件已经存在且不为空
        //         LOG_WARN("Non-empty output file already exists: %s", G_SETTINGS.output_file.c_str());
        //         LOG_WARN("Program exited.");
        //         exit(0);
        //     }
        // }
    }
#ifndef ONLINE_JUDGE
    switch (G_SETTINGS.log_level)
    {
        case 0:
            common::g_log = new common::Log(common::FATAL);
            break;
        case 1:
            common::g_log = new common::Log(common::ERROR);
            break;
        case 2:
            common::g_log = new common::Log(common::WARN);
            break;
        case 3:
            common::g_log = new common::Log(common::INFO);
            break;
        case 4:
            common::g_log = new common::Log(common::DEBUG);
            break;
        default:
            common::g_log = new common::Log(common::WARN);
            break;
    }
#endif
    common::g_thpool = new common::ThreadPool(G_SETTINGS.thread_num);
    
}

int main(int argc, char *argv[])
{
    // 解析命令行参数
    G_SETTINGS.parse_args(argc, argv);
    // 初始化环境
    init_env();
    // 管理线程池
    std::unique_ptr<common::ThreadPool> thpool(common::g_thpool);
    // 管理日志
    std::unique_ptr<common::Log> log(common::g_log);
    // 从输入文件中读取代码
    std::ifstream input_file(G_SETTINGS.input_file);
    if (!input_file.is_open())
    {
        LOG_FATAL("Can't open input file: %s", G_SETTINGS.input_file.c_str());
    }
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string code = buffer.str();
    input_file.close();
    
    // 生成Token流文件
    if (!G_SETTINGS.input_file.empty()) {
        LOG_INFO("Generating token file...");
        generateTokenFile(code, G_SETTINGS.output_file);
    }
    
    // 第一步：词法分析 and 语法分析
    LOG_DEBUG("Start parsing code...");
    ProgramStmt* program_stmt;
    int ret = code_parse(code.c_str(), &program_stmt);
    if (ret != 0)
    {
        LOG_ERROR("Parsing code failed.");
        return ret;
    }
    LOG_DEBUG("Parsing code done.");
    if(!program_stmt) {
        LOG_FATAL("Program exit");
        return -1;
    }
    // 打印语法树到 tree.txt
    LOG_DEBUG("Attempting to print syntax tree...");
    printSyntaxTree(program_stmt);
    LOG_DEBUG("Syntax tree printing attempted.");
    // 第二步: 语义分析 & 生成中间代码
    LOG_DEBUG("Start generating intermediate code...");
    std::unique_ptr<ir::IRGenerator> visitor = std::make_unique<ir::IRGenerator>();
    visitor->visit(*program_stmt);
    std::unique_ptr<ProgramStmt> program_stmt_ptr(program_stmt);
    //visitor->show_result();
    ir::Module ir = visitor->get_ir();
    LOG_DEBUG("Generating intermediate code done.");
    // 第三步: 优化
    if (G_SETTINGS.opt_level)
    {
        LOG_DEBUG("Start optimizing intermediate code...");
        // TODO
        std::vector<std::unique_ptr<opt::Optimize>> opts;
        opts.emplace_back(std::make_unique<opt::ConstExprOpt>());
        for (auto &opt : opts)
        {
            LOG_DEBUG("Optimizing with %s...", opt->get_name().c_str());
            opt->optimize(ir);
            LOG_DEBUG("Optimizing with %s done.", opt->get_name().c_str());
        }
        LOG_DEBUG("Optimizing intermediate code done.");
    }
    // 第四步: 生成目标代码
    std::ofstream output_file(G_SETTINGS.output_file);
    LOG_DEBUG("Start generating target code...");
    
    LOG_INFO("Generating C code...");
    std::unique_ptr<builder::c::CBuilder> builder = std::make_unique<builder::c::CBuilder>();
    builder->build(ir);
    builder->output(output_file);
    output_file.close();
    
    LOG_DEBUG("Generating target code done.");
    
    return 0;
}
