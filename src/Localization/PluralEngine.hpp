#ifndef NB_SRC_LOCALIZATION_PLURALENGINE_HPP
#define NB_SRC_LOCALIZATION_PLURALENGINE_HPP

#include <NbCore.hpp>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <variant>
#include <optional>
#include <charconv>

namespace Localization
{
    enum class PluralCategory { Zero, One, Two, Few, Many, Other };

    inline std::string_view to_string(PluralCategory cat) {
        static constexpr std::string_view names[] = { "zero", "one", "two", "few", "many", "other" };
        return names[static_cast<int>(cat)];
    }

    using FormatterArgs = std::unordered_map<std::string, std::variant<double, std::string>>;


    struct PluralOperands {
        double n;      // Absolute value
        int64_t i;     // Integer part
        int v;         // Fraction digit count
        int64_t f;     // Fraction part as integer

        static PluralOperands fromDouble(double val) {
            val = std::abs(val);
            const auto i = static_cast<int64_t>(val);

            char buf[64];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), val, std::chars_format::fixed);
            std::string_view s(buf, ptr - buf);

            if (const auto dot = s.find('.'); dot != std::string_view::npos) {
                const auto frac = s.substr(dot + 1);
                return { val, i, static_cast<int>(frac.size()), std::stoll(std::string(frac)) };
            }
            return { val, i, 0, 0 };
        }
    };


    class LocalePlural;

    class Node {
    public:
        virtual ~Node() = default;
        virtual std::string evaluate(const FormatterArgs& args, const LocalePlural& loc) const = 0;
    };

    using NodePtr = std::unique_ptr<Node>;
    using NodeList = std::vector<NodePtr>;

    class TextNode : public Node {
        std::string text;
    public:
        explicit TextNode(std::string_view t) : text(t) {}
        std::string evaluate(const FormatterArgs&, const LocalePlural&) const override { return text; }
    };

    class VariableNode : public Node {
        std::string name;
    public:
        explicit VariableNode(std::string_view n) : name(n) {}
        std::string evaluate(const FormatterArgs& args, const LocalePlural&) const override {
            if (auto it = args.find(name); it != args.end()) {
                if (auto d = std::get_if<double>(&it->second)) return std::to_string(static_cast<int64_t>(*d));
                if (auto s = std::get_if<std::string>(&it->second)) return *s;
            }
            return "{" + name + "}";
        }
    };

    class BranchNode : public Node {
    protected:
        std::string varName;
        std::unordered_map<std::string, NodeList> branches;

        std::string evaluateBranch(std::string_view key, const FormatterArgs& args, const LocalePlural& loc, std::optional<double> hashValue = std::nullopt) const {
            auto it = branches.find(std::string(key));
            if (it == branches.end()) it = branches.find("other");
            if (it == branches.end()) return "";

            std::string result;
            for (const auto& node : it->second) {
                std::string part = node->evaluate(args, loc);
                if (hashValue) {
                    size_t pos = 0;
                    std::string sub = std::to_string(static_cast<int64_t>(*hashValue));
                    while ((pos = part.find('#', pos)) != std::string::npos) {
                        part.replace(pos, 1, sub);
                        pos += sub.size();
                    }
                }
                result += part;
            }
            return result;
        }

    public:
        explicit BranchNode(std::string_view var) : varName(var) {}
        void addBranch(std::string_view key, NodeList nodes) { branches[std::string(key)] = std::move(nodes); }
    };

    class PluralNode : public BranchNode {
    public:
        using BranchNode::BranchNode;
        std::string evaluate(const FormatterArgs& args, const LocalePlural& loc) const override;
    };

    class SelectNode : public BranchNode {
    public:
        using BranchNode::BranchNode;
        std::string evaluate(const FormatterArgs& args, const LocalePlural& loc) const override {
            std::string key = "other";
            if (auto it = args.find(varName); it != args.end()) {
                if (auto s = std::get_if<std::string>(&it->second)) key = *s;
            }
            return evaluateBranch(key, args, loc);
        }
    };


    using PluralRuleFunc = std::function<PluralCategory(const PluralOperands&)>;

    class LocalePlural 
    {
        std::string id;
        PluralRuleFunc rule;
    public:
        LocalePlural() = default;

        LocalePlural(
            std::string_view lang,
            PluralRuleFunc f
        ) 
            : id(lang)
            , rule(std::move(f))
        {}

        NB_COPYMOVABLE(LocalePlural);

        PluralCategory getCategory(double n) const { return rule(PluralOperands::fromDouble(n)); }
        std::string_view langId() const { return id; }
    };

    inline std::string PluralNode::evaluate(const FormatterArgs& args, const LocalePlural& loc) const {
        double val = 0;
        if (auto it = args.find(varName); it != args.end()) {
            if (auto d = std::get_if<double>(&it->second)) val = *d;
        }
        return evaluateBranch(to_string(loc.getCategory(val)), args, loc, val);
    }


    class MessageCompiler {
        static std::string_view trim(std::string_view s) {
            s.remove_prefix((std::min)(s.find_first_not_of(" \t\n\r"), s.size()));
            const auto e = s.find_last_not_of(" \t\n\r");
            return (e == std::string_view::npos) ? "" : s.substr(0, e + 1);
        }

        static size_t findClosingBrace(std::string_view s, size_t start) {
            int depth = 0;
            for (size_t i = start; i < s.size(); ++i) {
                if (s[i] == '{') depth++;
                else if (s[i] == '}') if (--depth == 0) return i;
            }
            return std::string_view::npos;
        }

    public:
        static NodeList compile(std::string_view pattern) {
            NodeList nodes;
            size_t last = 0;

            for (size_t i = 0; i < pattern.size(); ++i) {
                if (pattern[i] == '{') {
                    if (i > last) nodes.push_back(std::make_unique<TextNode>(pattern.substr(last, i - last)));

                    const size_t end = findClosingBrace(pattern, i);
                    if (end == std::string_view::npos) break;

                    nodes.push_back(parseBlock(pattern.substr(i + 1, end - i - 1)));
                    i = end;
                    last = i + 1;
                }
            }
            if (last < pattern.size()) nodes.push_back(std::make_unique<TextNode>(pattern.substr(last)));
            return nodes;
        }

    private:
        static NodePtr parseBlock(std::string_view content) {
            std::vector<std::string_view> parts;
            size_t last = 0, depth = 0;
            for (size_t i = 0; i < content.size(); ++i) {
                if (content[i] == '{') depth++;
                else if (content[i] == '}') depth--;
                else if (content[i] == ',' && depth == 0) {
                    parts.emplace_back(trim(content.substr(last, i - last)));
                    last = i + 1;
                }
            }
            parts.emplace_back(trim(content.substr(last)));

            if (parts.size() == 1) return std::make_unique<VariableNode>(parts[0]);

            const auto type = parts[1];
            if (type == "plural" || type == "select") {
                std::unique_ptr<BranchNode> node;
                if (type == "plural") node = std::make_unique<PluralNode>(parts[0]);
                else node = std::make_unique<SelectNode>(parts[0]);

                const auto branchesRaw = parts[2];
                for (size_t i = 0; i < branchesRaw.size(); ) {
                    const auto k_start = branchesRaw.find_first_not_of(" \t\n\r", i);
                    if (k_start == std::string_view::npos) break;
                    const auto k_end = branchesRaw.find('{', k_start);
                    const auto v_end = findClosingBrace(branchesRaw, k_end);

                    node->addBranch(trim(branchesRaw.substr(k_start, k_end - k_start)),
                        compile(branchesRaw.substr(k_end + 1, v_end - k_end - 1)));
                    i = v_end + 1;
                }
                return node;
            }
            return std::make_unique<TextNode>("[Parser Error]");
        }
    };


    class PluralEngine
    {
        std::unordered_map<std::string, LocalePlural> locales;
        std::unordered_map<std::string, NodeList> astCache;

    public:
        void registerLocale(std::string_view id, PluralRuleFunc rule)
        {
            locales.try_emplace(std::string(id), id, std::move(rule));
        }

        std::string format(std::string_view lang, std::string_view pattern, const FormatterArgs& args)
        {
            auto itLoc = locales.find(std::string(lang));
            if (itLoc == locales.end()) return "Locale Not Found";

            const std::string key(pattern);
            if (astCache.find(key) == astCache.end()) {
                astCache[key] = MessageCompiler::compile(pattern);
            }

            std::string result;
            for (const auto& node : astCache[key]) {
                result += node->evaluate(args, itLoc->second);
            }
            return result;
        }
    };


    namespace rules
    {
        inline PluralCategory Russian(const PluralOperands& o) {
            if (o.v > 0) return PluralCategory::Other;
            if (o.i % 10 == 1 && o.i % 100 != 11) return PluralCategory::One;
            if (o.i % 10 >= 2 && o.i % 10 <= 4 && (o.i % 100 < 12 || o.i % 100 > 14)) return PluralCategory::Few;
            if (o.i % 10 == 0 || (o.i % 10 >= 5 && o.i % 10 <= 9) || (o.i % 100 >= 11 && o.i % 100 <= 14)) return PluralCategory::Many;
            return PluralCategory::Other;
        }

        inline PluralCategory English(const PluralOperands& o) {
            return (o.i == 1 && o.v == 0) ? PluralCategory::One : PluralCategory::Other;
        }

        inline PluralCategory Arabic(const PluralOperands& o) {
            if (o.v > 0) return PluralCategory::Few;
            if (o.i == 0) return PluralCategory::Zero;
            if (o.i == 1) return PluralCategory::One;
            if (o.i == 2) return PluralCategory::Two;
            const auto m100 = o.i % 100;
            if (m100 >= 3 && m100 <= 10) return PluralCategory::Few;
            if (m100 >= 11 && m100 <= 99) return PluralCategory::Many;
            return PluralCategory::Other;
        }
    }

};

#endif 
