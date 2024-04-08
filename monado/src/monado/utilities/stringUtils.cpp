#include "monado/utilities/stringUtils.h"

namespace Monado::Utils {

    /**
     * @brief 获取文件名
     *
     * @param filepath 文件的完整路径
     * @return std::string 文件名，没有文件名返回空字符串
     */
    std::string GetFilename(const std::string &filepath) {
        std::vector<std::string> parts = SplitString(filepath, "/\\");

        if (parts.size() > 0)
            return parts[parts.size() - 1];

        return "";
    }

    /**
     * @brief 获取一个文件名的拓展名
     *
     * @param filename 文件名
     * @return std::string 拓展名，没有拓展名返回空字符串
     */
    std::string GetExtension(const std::string &filename) {
        std::vector<std::string> parts = SplitString(filename, '.');

        if (parts.size() > 1)
            return parts[parts.size() - 1];

        return "";
    }

    std::string RemoveExtension(const std::string &filename) { return filename.substr(0, filename.find_last_of('.')); }

    /**
     * @brief 判断一个模式串是否是以匹配串作为头部
     *
     * @param string 模式串
     * @param start 匹配串
     * @return true 匹配串是模式串的头部
     * @return false 匹配串不是模式串的头部
     */
    bool StartsWith(const std::string &string, const std::string &start) { return string.find(start) == 0; }

    /**
     * @brief 根据分隔符分隔字符串
     *
     * @param string 待分隔的字符串
     * @param delimiters 分隔符
     * @return std::vector<std::string> 分隔后得到的结果
     */
    std::vector<std::string> SplitString(const std::string &string, const std::string &delimiters) {
        size_t start = 0;
        size_t end = string.find_first_of(delimiters);

        std::vector<std::string> result;

        while (end <= std::string::npos) {
            std::string token = string.substr(start, end - start);
            if (!token.empty())
                result.push_back(token);

            if (end == std::string::npos)
                break;

            start = end + 1;
            end = string.find_first_of(delimiters, start);
        }

        return result;
    }

    /**
     * @brief 根据分隔符分隔字符串
     *
     * @param string 待分隔的字符串
     * @param delimiter 分隔符
     * @return std::vector<std::string> 分隔后得到的结果
     */
    std::vector<std::string> SplitString(const std::string &string, const char delimiter) {
        return SplitString(string, std::string(1, delimiter));
    }

} // namespace Monado::Utils
