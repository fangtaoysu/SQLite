#pragma once
#include <string>


namespace orm {
/**
 * @class Database
 * @brief 操作数据库
 */
class Database {
  private:
    std::string _filename;
    void WriteSqlToFile(const std::string& sql);

  public:
    enum class Error {
        OK,
        EMPTY_SQL,
        MISSING_SEMICOLON,
        FILE_ERROR
    };
    // 防止编译器进行隐式转化，要去构造函数必须显示调用
    explicit Database(const std::string& filename);
    // 执行sql的万能语句
    Error execute(const std::string& sql);
    void PrintError(Error err, const std::string& sql);
};

}