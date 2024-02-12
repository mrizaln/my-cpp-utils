#ifndef UNINDENT_HPP_OHGAE2K6
#define UNINDENT_HPP_OHGAE2K6

#include <algorithm>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

/*
 * Used to unindent a raw string literal (or any string actually).
 * Example:
 * ```
 * std::string_view s = R"(
 *     Hello
 *     World
 * )";
 * std::cout << unindent(s, true, true);
 * ````
 *
 * Inspired by: https://stackoverflow.com/a/24900770
 */
inline std::string unindent(
    std::string_view string,
    bool             removeLeadingNewline  = false,
    bool             removeTrailingNewline = false
)
{
    if (string.empty()) {
        return {};
    }

    struct Line
    {
        const char* p;
        std::size_t len;        // length of the line, the newline character
        bool        isBlank;    // blank line means it contains only whitespace
    };

    const char*       p{ string.data() };
    std::vector<Line> lines;

    while (*p != '\0') {
        const auto* lineStart{ p };
        bool        isBlankLine{ true };
        while (*p != '\0' && *p != '\n') {
            if (!static_cast<bool>(std::isspace(*p))) {
                isBlankLine = false;
            }
            ++p;
        }
        lines.emplace_back(lineStart, static_cast<std::size_t>(p - lineStart), isBlankLine);
        if (*p == '\0') {
            break;
        }
        ++p;
    }

    if (removeLeadingNewline) {
        auto firstNonBlankLine = std::find_if(lines.begin(), lines.end(), [](const auto& line) {
            return !line.isBlank;
        });
        if (firstNonBlankLine != lines.end()) {
            lines.erase(lines.begin(), firstNonBlankLine);
        }
    }

    if (removeTrailingNewline) {
        auto lastNonBlankLine = std::find_if(lines.rbegin(), lines.rend(), [](const auto& line) {
            return !line.isBlank;
        });
        if (lastNonBlankLine != lines.rend()) {
            lines.erase(lastNonBlankLine.base(), lines.end());
        }
    }

    const std::size_t indentLength = [&] {
        std::size_t minIndentLength{ std::numeric_limits<std::size_t>::max() };
        for (const auto& line : lines) {
            if (line.isBlank) {
                continue;
            }
            std::size_t indentLength{ 0 };
            while (indentLength < line.len && (bool)std::isspace(line.p[indentLength])) {
                ++indentLength;
            }
            minIndentLength = std::min(minIndentLength, indentLength);
        }
        return minIndentLength;
    }();

    std::string result;
    result.reserve(string.size());
    for (std::size_t index{ 0 }; const auto& line : lines) {
        if (!line.isBlank) {
            result.append(line.p + indentLength, line.len - indentLength);
        }
        if (++index != lines.size()) {
            result.push_back('\n');
        }
    }
    return result;
}

#endif /* end of include guard: UNINDENT_HPP_OHGAE2K6 */
