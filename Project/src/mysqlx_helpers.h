#ifndef _MYSQLX_HELPERS_H
#define _MYSQLX_HELPERS_H

#include <mysqlx/xdevapi.h>
#include <vector>
#include <cstddef>

// Declare the function as inline to avoid duplicate symbols
inline std::vector<uint64_t> mysqlx_raw_as_u64_vector(const mysqlx::Value& in_value) {
    std::vector<uint64_t> out;

    const auto bytes = in_value.getRawBytes();
    auto ptr = reinterpret_cast<const std::byte*>(bytes.first);
    auto end = reinterpret_cast<const std::byte*>(bytes.first) + bytes.second;

    while (ptr != end) {
        static constexpr std::byte carry_flag{0b1000'0000};
        static constexpr std::byte value_mask{0b0111'1111};

        uint64_t v = 0;
        uint64_t shift = 0;
        bool is_carry;
        do {
            auto byte = *ptr;
            is_carry = (byte & carry_flag) == carry_flag;
            v |= std::to_integer<uint64_t>(byte & value_mask) << shift;

            ++ptr;
            shift += 7;
        } while (is_carry && ptr != end && shift <= 63);

        out.push_back(v);
    }

    return out;
}

#endif
