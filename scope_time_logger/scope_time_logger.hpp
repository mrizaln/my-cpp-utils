#ifndef SCOPE_TIME_LOGGER_HPP_TORJQ8M2
#define SCOPE_TIME_LOGGER_HPP_TORJQ8M2

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

namespace util
{
    template <typename Dur = std::chrono::milliseconds>
    class ScopeTimeLogger
    {
    public:
        friend std::unique_ptr<ScopeTimeLogger> std::make_unique<ScopeTimeLogger>();

        using Data_type    = std::map<std::string, Dur>;    // iterator won't be invalidated even when resized
        using Element_type = Data_type::value_type;

        enum FunctionStatus
        {
            ACTIVE,
            INACTIVE,
            ACTIVE_AND_INACTIVE,
        };

    private:
        class TimeInserter
        {
            friend ScopeTimeLogger;

            using Clock_type  = std::chrono::steady_clock;
            using Second_type = Dur;

            std::chrono::time_point<Clock_type> m_beginning{ Clock_type::now() };
            const Data_type::key_type           m_key;

            TimeInserter(Data_type::key_type key)
                : m_key{ std::move(key) }
            {
            }

        public:
            ~TimeInserter()
            {
                ScopeTimeLogger::insert({
                    m_key,
                    std::chrono::duration_cast<Second_type>(Clock_type::now() - m_beginning),
                });
            }
        };

    private:
        inline static std::unique_ptr<ScopeTimeLogger> s_instance{ nullptr };

    private:
        Data_type                   m_runTimes;
        std::map<std::string, bool> m_funcActivities;
        std::mutex                  m_mutex;

    public:
        static ScopeTimeLogger& start()
        {
            if (s_instance.get() == nullptr) {
                s_instance = std::make_unique<ScopeTimeLogger>();
            }
            return *s_instance;
        }

        static ScopeTimeLogger* getInstance() { return s_instance.get(); }

        [[nodiscard("object will be destroyed immediately thus timer won't be working correctly if discarded")]]
        static TimeInserter add(const std::string& name)
        {
            return TimeInserter(name);
        }

        static void print(bool clearAfter, bool printInline = false)
        {
            if (s_instance.get() == nullptr) {
                return;
            }

            if (printInline) {
                std::cout << "\033[s"
                          << "\033[0J";
            }

            for (const auto& e : s_instance->m_runTimes) {
                std::cout << e.first << " " << e.second << '\n';
            }
            std::cout << "\n";

            if (printInline) {
                std::cout << "\033[u";
            }

            std::lock_guard lock{ s_instance->m_mutex };
            if (clearAfter) {
                s_instance->m_runTimes.clear();
            }
        }

        static std::optional<Data_type> read(FunctionStatus status)
        {
            std::lock_guard lock{ s_instance->m_mutex };
            if (s_instance.get() == nullptr) {
                return {};
            }

            Data_type temp;

            switch (status) {
            case ACTIVE:
                for (const auto& e : s_instance->m_runTimes) {
                    if (s_instance->m_funcActivities[e.first]) {
                        temp.insert(e);
                    }
                }
                break;
            case INACTIVE:
                for (const auto& e : s_instance->m_runTimes) {
                    if (!s_instance->m_funcActivities[e.first]) {
                        temp.insert(e);
                    }
                }
                break;
            case ACTIVE_AND_INACTIVE:
                for (const auto& e : s_instance->m_runTimes) {
                    temp.insert(e);
                }
                break;
            }

            // reset function activities
            for (auto& e : s_instance->m_funcActivities) {
                e.second = false;
            }

            return temp;
        }

    private:
        ScopeTimeLogger() = default;

        static void insert(Data_type::value_type&& value)
        {
            if (s_instance.get() == nullptr) {
                return;
            }

            std::lock_guard lock{ s_instance->m_mutex };

            if (s_instance->m_runTimes.count(value.first) == 0) {
                s_instance->m_runTimes.insert(value);
                s_instance->m_funcActivities.insert({ value.first, true });
            } else {
                s_instance->m_runTimes.find(value.first)->second       = value.second;
                s_instance->m_funcActivities.find(value.first)->second = true;
            }
        }
    };
}

#endif /* end of include guard: SCOPE_TIME_LOGGER_HPP_TORJQ8M2 */
