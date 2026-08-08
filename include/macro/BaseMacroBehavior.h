#pragma once
#include "IMacroBehavior.h"


class BaseMacroBehavior : public IMacroBehavior {
protected:
    uint32_t m_debounceMs = 0;
    uint64_t m_lastTickMs = 0;

public:
    BaseMacroBehavior(uint32_t debounceMs = 0) : m_debounceMs(debounceMs) {};
    ~BaseMacroBehavior() override = default;

    inline bool CanTick(uint64_t currentTimestampMs) const override {
        return (currentTimestampMs - m_lastTickMs) >= m_debounceMs;
    };
    inline void UpdateLastTickTime(uint64_t tickMs) override {
        m_lastTickMs = tickMs;
    };
};