#include <cstdint>
#include <vector>
#include <Susi4.h>
#include <string>
#include <cstdio>

#include "util.h"

namespace SUSI {
    namespace GPIO {
        using detail::PinAccess;

        static inline SusiId_t PinToSingleId(Pin pin) noexcept {
            const auto abs = static_cast<uint8_t>(PinAccess::abs(pin));
            return SUSI_ID_GPIO(abs);
        }

        static inline SusiId_t BankToSingleId(Bank bank) noexcept {
            return SUSI_ID_GPIO_BANK(static_cast<uint32_t>(bank.index));
        }

        static inline bool PassesFilter(uint32_t supIn, uint32_t supOut, int bit, PinFilter filter) noexcept {
            const bool in = (supIn & (1u << bit)) != 0;
            const bool out = (supOut & (1u << bit)) != 0;
            switch (filter) {
            case PinFilter::Any:            return in || out;
            case PinFilter::InputOnly:      return in && !out;
            case PinFilter::OutputOnly:     return out && !in;
            case PinFilter::InputAndOutput: return in && out;
            }
            return false;
        }

        static inline uint32_t MaskForFilter(uint32_t supIn, uint32_t supOut, PinFilter f) noexcept {
            switch (f) {
            case PinFilter::Any:            return supIn | supOut;
            case PinFilter::InputOnly:      return supIn & ~supOut;
            case PinFilter::OutputOnly:     return supOut & ~supIn;
            case PinFilter::InputAndOutput: return supIn & supOut;
            }
            return 0;
        }

        std::vector<Pin> FindAvailablePins(PinFilter filter) {
            std::vector<Pin> pins;
            pins.reserve(64);

            for (uint8_t b = 0; b < MAX_BANKS; ++b) {
                Bank bank{ b };
                const uint32_t idBank = BankToSingleId(bank);
                uint32_t supIn = 0, supOut = 0;

                const auto st1 = SusiGPIOGetCaps(idBank, SUSI_ID_GPIO_INPUT_SUPPORT, &supIn);
                const auto st2 = SusiGPIOGetCaps(idBank, SUSI_ID_GPIO_OUTPUT_SUPPORT, &supOut);
                if (st1 != SUSI_STATUS_SUCCESS || st2 != SUSI_STATUS_SUCCESS) continue;

                const uint32_t maskAny = (supIn | supOut);
                if (maskAny == 0) continue;

                for (int bit = 0; bit < static_cast<int>(PINS_PER_BANK); ++bit) {
                    if ((maskAny & (1u << bit)) && PassesFilter(supIn, supOut, bit, filter)) {
                        pins.emplace_back(bank, static_cast<std::uint32_t>(bit));
                    }
                }
            }
            return pins;
        }

        std::vector<Bank> FindAvailableBanks(PinFilter filter) {
            std::vector<Bank> banks;
            banks.reserve(MAX_BANKS);

            for (uint8_t b = 0; b < MAX_BANKS; ++b) {
                Bank bank{ b };
                const uint32_t idBank = BankToSingleId(bank);
                uint32_t supIn = 0, supOut = 0;

                const auto st1 = SusiGPIOGetCaps(idBank, SUSI_ID_GPIO_INPUT_SUPPORT, &supIn);
                const auto st2 = SusiGPIOGetCaps(idBank, SUSI_ID_GPIO_OUTPUT_SUPPORT, &supOut);
                if (st1 != SUSI_STATUS_SUCCESS || st2 != SUSI_STATUS_SUCCESS)
                    continue;

                const uint32_t mask = MaskForFilter(supIn, supOut, filter);
                if (mask != 0)
                    banks.push_back(bank);
            }
            return banks;
        }

        SusiStatus_t SetOut(Pin pin) noexcept {
            const SusiId_t id = PinToSingleId(pin);
            return SusiGPIOSetDirection(id, /*mask*/1u, /*Direction(0=out,1=in)*/0u);
        }

        SusiStatus_t SetIn(Pin pin) noexcept {
            const SusiId_t id = PinToSingleId(pin);
            return SusiGPIOSetDirection(id, /*mask*/1u, /*Direction*/1u);
        }

        SusiStatus_t GetDir(Pin pin, Dir& dir) noexcept {
            const SusiId_t id = PinToSingleId(pin);
            uint32_t dir_raw = 0;
            const auto st = SusiGPIOGetDirection(id, /*mask*/1u, &dir_raw);
            if (st == SUSI_STATUS_SUCCESS) {
                const bool isInput = (dir_raw & 1u) != 0;
                dir = isInput ? Dir::Input : Dir::Output;
            }
            else
                dir = Dir::Unknown;
            return st;
        }

        SusiStatus_t WriteLow(Pin pin) noexcept {
            const SusiId_t id = PinToSingleId(pin);
            return SusiGPIOSetLevel(id, /*mask*/1u, /*Level(0=low,1=high)*/0u);
        }

        SusiStatus_t WriteHigh(Pin pin) noexcept {
            const SusiId_t id = PinToSingleId(pin);
            return SusiGPIOSetLevel(id, /*mask*/1u, /*Level*/1u);
        }

        SusiStatus_t GetLevel(Pin pin, Level& level) noexcept {
            const SusiId_t id = PinToSingleId(pin);
            uint32_t level_raw = 0;
            const auto st = SusiGPIOGetLevel(id, /*mask*/1u, &level_raw);
            if (st == SUSI_STATUS_SUCCESS) {
                const bool isHigh = (level_raw & 1u) != 0;
                level = isHigh ? Level::High : Level::Low;
            }
            else
                level = Level::Unknown;
            return st;
        }

    }
}
