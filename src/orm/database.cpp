#include "../../include/orm/database.h"
#include <iostream>
#include <fstream>

// TODO: cout 转为日志信息

orm::Database::Database(const std::string& filename) {
    _filename = "/data/fangtao/SQLite/sources/" + filename;
}

// 执行sql语句
orm::Database::Error orm::Database::execute(const std::string& sql) {
    if (sql.empty()) {
        return Error::EMPTY_SQL; // SQL语句不能为空
    }
    if (sql.back() != ';') {
        return Error::MISSING_SEMICOLON; // SQL语句必须以分号结尾
    }
    try {
        WriteSqlToFile(sql);
        return Error::OK;
     } catch (const std::exception& e) {
        std::cerr << "[系统错误] " << e.what() << std::endl;
        return Error::FILE_ERROR;
     }
}

void orm::Database::WriteSqlToFile(const std::string& sql) {
    // 使用二进制避免文本转化
    std::ofstream db_file(_filename, std::ios::app | std::ios::binary);
    
    if (!db_file) {
        db_file.open(_filename, std::ios::out); // 尝试创建新文件
        if (!db_file) {
            throw std::runtime_error("无法打开数据库文件");
        }

    }
    db_file << sql << ";\n";
    db_file.flush(); // 缓冲区写入磁盘
}

void orm::Database::PrintError(Error err, const std::string& sql) {
    const std::string info("数据库文件: " + _filename + ",执行sql语句:" + sql + ", ");
    switch (err)
    {
        case Error::EMPTY_SQL:
            std::cerr << "[Error] " << info << "SQL语句为空" << std::endl;
            break;
        case Error::MISSING_SEMICOLON:
            std::cerr << "[Warning] " << info << "结尾缺少分号" << std::endl;
            break;
        case Error::FILE_ERROR:
            std::cerr << "[Warning] 数据库文件: " << _filename << "文件操作失败" << std::endl;
            break;
        default:
            std::cout << "[OK] " << info << "成功" << std::endl;
            break;
    }
}