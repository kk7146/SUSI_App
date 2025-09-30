#ifndef GPIO_H
#define GPIO_H

#include <vector>
#include <cstdint>

#include "Susi4.h"

namespace SUSI {
    namespace GPIO {
        constexpr std::uint32_t MAX_BANKS = 16;
        constexpr std::uint32_t PINS_PER_BANK = 32;

        enum class PinFilter : std::uint8_t {
            Any,
            InputOnly,
            OutputOnly,
            InputAndOutput
        };
        enum class Dir : std::uint8_t {
            Output,
            Input,
            Unknown
        };
        enum class Level : std::uint8_t {
            Low,
            High,
            Unknown
        };

        struct Bank {
            std::uint32_t index{ static_cast<std::uint32_t>(-1) };
            [[nodiscard]]constexpr bool valid() const noexcept { return index < MAX_BANKS; }
        };

        namespace detail { struct PinAccess; }

        struct Pin {
            constexpr Pin(Bank b, std::uint32_t bitIndex) noexcept
                : bank_{ b.index }, bit_{ bitIndex }, abs_{ bank_ * PINS_PER_BANK + bitIndex } {
            }
            [[nodiscard]] constexpr bool valid() const noexcept {
                return bank_ < MAX_BANKS && bit_ < PINS_PER_BANK;
            }
            [[nodiscard]] constexpr Bank bank() const noexcept { return Bank{ bank_ }; }
            [[nodiscard]] constexpr std::uint32_t bit() const noexcept { return bit_; }

        private:
            std::uint32_t bank_{ static_cast<std::uint32_t>(-1) };
            std::uint32_t bit_{ static_cast<std::uint32_t>(-1) };
            std::uint32_t abs_{ static_cast<std::uint32_t>(-1) };
            friend struct detail::PinAccess;
        };

        namespace detail {
            struct PinAccess {
                [[nodiscard]] static constexpr std::uint32_t abs(Pin p) noexcept { return p.abs_; }
                [[nodiscard]] static constexpr Pin from_abs(std::uint32_t a) noexcept {
                    return Pin{ Bank{ a / PINS_PER_BANK }, a % PINS_PER_BANK };
                }
            };
        }

        std::vector<Pin>      FindAvailablePins(PinFilter filter = PinFilter::Any);
        std::vector<Bank>     FindAvailableBanks(PinFilter filter = PinFilter::Any);

        [[nodiscard]] SusiStatus_t SetOut(Pin pin) noexcept;
        [[nodiscard]] SusiStatus_t SetIn(Pin pin) noexcept;
        [[nodiscard]] SusiStatus_t GetDir(Pin pin, Dir& dir) noexcept;
        [[nodiscard]] SusiStatus_t WriteLow(Pin pin) noexcept;
        [[nodiscard]] SusiStatus_t WriteHigh(Pin pin) noexcept;
        [[nodiscard]] SusiStatus_t GetLevel(Pin pin, Level& level) noexcept;
    }
}

#endif
