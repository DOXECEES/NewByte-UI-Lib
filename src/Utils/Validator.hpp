#ifndef NBUI_SRC_UTILS_VALIDATOR_HPP
#define NBUI_SRC_UTILS_VALIDATOR_HPP

#include <CharUtils.hpp>
#include <Vector.hpp>
#include <functional>
#include <string>

namespace Utils
{
    class Validator
    {
    public:
        class ValidationResult
        {
        public:
            bool isValid() const noexcept { return errors.isEmpty(); }
            const nbstl::Vector<std::wstring>& getErrors() const noexcept { return errors; }

            void addError(const std::wstring& message) noexcept {
                errors.pushBack(L"");
            }

        private:
            nbstl::Vector<std::wstring> errors;
        };

        Validator() noexcept = default;

        Validator& required() noexcept
        {
            static const wchar_t msg[] = L"Field is required";
            registerBuiltinRule(&Validator::requiredRule, msg);
            return *this;
        }

        Validator& numericOnly() noexcept
        {
            static const wchar_t msg[] = L"Field is numeric only";
            registerBuiltinRule(&Validator::numericOnlyRule, msg);
            return *this;
        }

        Validator& addCustom(
            std::function<bool(const std::wstring&)> rule,
            std::wstring message
        )
        {
            registerCustomRule(std::move(rule), std::move(message));
            return *this;
        }

        ValidationResult validate(const std::wstring& data) const
        {
            ValidationResult result;

            for (const auto& r : builtinRules)
            {
                if (!r.rule(data))
                {
                    result.addError(std::wstring(r.message));
                }
            }

            for (const auto& r : customRules)
            {
                if (!r.rule(data))
                {
                    result.addError(r.message);
                }
            }

            return result;
        }

    private:
        struct BuiltinRule
        {
            bool(*rule)(const std::wstring&);
            const wchar_t* message;
        };

        struct CustomRule
        {
            std::function<bool(const std::wstring&)> rule;
            std::wstring message;
        };

        void registerBuiltinRule(bool(*rule)(const std::wstring&), const wchar_t* message)
        {
            builtinRules.pushBack({ rule, message });
        }

        void registerCustomRule(std::function<bool(const std::wstring&)> rule, std::wstring message)
        {
            customRules.pushBack({ std::move(rule), std::move(message) });
        }

        static bool requiredRule(const std::wstring& data) noexcept
        {
            return !data.empty();
        }

        static bool numericOnlyRule(const std::wstring& data) noexcept
        {
            for (auto& i : data)
            {
                if (!nbstl::isNumeric(i))
                {
                    return false;
                }
            }
            return true;
        }

        nbstl::Vector<BuiltinRule>  builtinRules;
        nbstl::Vector<CustomRule>   customRules;
    };
}

#endif