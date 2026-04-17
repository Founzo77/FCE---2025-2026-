#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "../PlatformWindows.hpp"

namespace fge
{
    enum class LoggerLevel
    {
        // Ultra-détaillé, utilisé pour suivre l’exécution pas à pas
        TRACE,
        // informations utiles aux développeurs pour le débogage 
        // (valeurs de variables, étapes internes…)
        DEBUG,
        // messages informatifs sur le fonctionnement normal de l’application 
        // (démarrage d’un service, action réussie, etc.)
        INFO,
        // quelque chose d’anormal ou inattendu, 
        // mais qui n’empêche pas l’application de continuer
        WARNING,
        // une opération a échoué, nécessite souvent une intervention
        ERRORS,
        // Empêche le système ou une partie du programme de continuer à fonctionner
        CRITICAL,
        // Aucun affichage (pour une meilleur optimisation il faut compiler
        // en mode release quand on ne veut pas d'affichage)
        NONE
    };

    constexpr const char* toString(LoggerLevel level) noexcept
    {
        switch (level)
        {
            case LoggerLevel::TRACE: return "TRACE";
            case LoggerLevel::DEBUG: return "DEBUG";
            case LoggerLevel::INFO: return "INFO";
            case LoggerLevel::WARNING: return "WARNING";
            case LoggerLevel::ERRORS: return "ERROR";
            case LoggerLevel::CRITICAL: return "CRITICAL";
            default: return "NONE";
        }
    }

    namespace detail {
        // ANSI escape codes
        constexpr const char* RESET   = "\033[0m";
        constexpr const char* GRAY    = "\033[90m";
        constexpr const char* RED     = "\033[31m";
        constexpr const char* GREEN   = "\033[32m";
        constexpr const char* YELLOW  = "\033[33m";
        constexpr const char* BLUE    = "\033[34m";
        constexpr const char* MAGENTA = "\033[35m";
        constexpr const char* CYAN    = "\033[36m";

        inline const char* levelColor(LoggerLevel level)
        {
            switch (level)
            {
                case LoggerLevel::TRACE:    return CYAN;
                case LoggerLevel::DEBUG:    return BLUE;
                case LoggerLevel::INFO:     return GREEN;
                case LoggerLevel::WARNING:  return YELLOW;
                case LoggerLevel::ERRORS:   return RED;
                case LoggerLevel::CRITICAL: return MAGENTA;
                default: return RESET;
            }
        }
    }

    #ifdef _WIN32
        inline void enableAnsiColors()
        {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE) return;

            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode)) return;

            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    #endif

    class LoggerStream
    {
    private:
        LoggerLevel m_level;
        std::ostream& m_outputStream;
        bool m_isPrePrint = false;

    public:
        explicit LoggerStream(LoggerLevel level, std::ostream& ostream) : 
            m_level(level), m_outputStream(ostream) {}
        LoggerStream(const LoggerStream& other) : 
            m_level(other.m_level), m_outputStream(other.m_outputStream) {}

        ~LoggerStream()
        {
            #pragma omp critical(post_print)
            {
                postPrint();
                m_outputStream.flush();
            }
        }

        template <typename T>
        inline LoggerStream& operator<<(const T& toPrint)
        {
           #if !defined(NDEBUG)
                if(m_level != LoggerLevel::NONE)
                {
                    #pragma omp critical(pre_print)
                    {
                        prePrint();
                        m_outputStream << toPrint;
                    }
                }
            #endif

            return *this;
        }

    private:
        std::string currentTime() const
        {
            using namespace std::chrono;

            auto now     = system_clock::now();
            auto nowTime = system_clock::to_time_t(now);
            auto ms      = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

            std::tm localTime{};
            #ifdef _WIN32
                localtime_s(&localTime, &nowTime);
            #else
                localtime_r(&nowTime, &localTime);
            #endif

            std::ostringstream oss;
            oss << std::put_time(&localTime, "%H:%M:%S")
                << "." << std::setw(3) << std::setfill('0') << ms.count();
            return oss.str();
        }

        inline void prePrint()
        {
            if(m_isPrePrint == false)
            {
                m_outputStream 
                    << detail::GRAY << "[" << currentTime() << "]" << detail::RESET << " "
                    << detail::levelColor(m_level) << "[" << toString(m_level) << "]" << 
                    detail::RESET << " ";

                m_isPrePrint = true;
            }
        }

        inline void postPrint()
        {
            if(m_isPrePrint)
            {
                m_outputStream << std::endl;
            }
        }
    };

    class Logger
    {
    private:
        LoggerLevel m_level;
        std::ostream* m_outputStream;

    public:
        explicit Logger(LoggerLevel level, std::ostream& outputStream = std::cout) : 
            m_level(level), m_outputStream(&outputStream) {}
        Logger(const Logger& other) : 
            m_level(other.m_level), m_outputStream(other.m_outputStream) {}

        inline void setLevel(LoggerLevel level) noexcept { m_level = level; }
        inline void setOutputStream(std::ostream& outputStream) noexcept { 
            m_outputStream = &outputStream; }

        inline LoggerStream trace() const {
            #if !defined(NDEBUG)
                if (m_level <= LoggerLevel::TRACE)
                    return LoggerStream(LoggerLevel::TRACE, *m_outputStream);
                else
                    return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #else
                return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #endif
        }
        inline LoggerStream debug() const {
           #if !defined(NDEBUG)
                if (m_level <= LoggerLevel::DEBUG) 
                    return LoggerStream(LoggerLevel::DEBUG, *m_outputStream);
                else
                    return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #else
                return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #endif
        }
        inline LoggerStream info() const {
           #if !defined(NDEBUG)
                if (m_level <= LoggerLevel::INFO) 
                    return LoggerStream(LoggerLevel::INFO, *m_outputStream);
                else
                    return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #else
                return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #endif
        }
        inline LoggerStream warning() const {
           #if !defined(NDEBUG)
                if (m_level <= LoggerLevel::WARNING) 
                    return LoggerStream(LoggerLevel::WARNING, *m_outputStream);
                else
                    return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #else
                return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #endif
        }
        inline LoggerStream error() const {
           #if !defined(NDEBUG)
                if (m_level <= LoggerLevel::ERRORS) 
                    return LoggerStream(LoggerLevel::ERRORS, *m_outputStream);
                else
                    return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #else
                return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #endif
        }
        inline LoggerStream critical() const {
           #if !defined(NDEBUG)
                if (m_level <= LoggerLevel::CRITICAL) 
                    return LoggerStream(LoggerLevel::CRITICAL, *m_outputStream);
                else
                    return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #else
                return LoggerStream(LoggerLevel::NONE, *m_outputStream);
            #endif
        }

        constexpr inline bool isLogOn() const
        {
           #if !defined(NDEBUG)
                return true;
            #else
                return false;
            #endif
        }
    };
}