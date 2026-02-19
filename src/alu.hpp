#include <cstdint>

class ALU {
  public:
	struct Result8 {
		uint8_t value;
		bool flag_z;
		bool flag_n;
		bool flag_h;
		bool flag_c;
	};
	struct Result16 {
		uint16_t value;
		bool flag_z;
		bool flag_n;
		bool flag_h;
		bool flag_c;
	};

	static Result8 add8(uint8_t a, uint8_t b) {
		uint16_t result = static_cast<uint16_t>(a) + static_cast<uint16_t>(b);
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;
		res.flag_n = 0;
		res.flag_h = (((a ^ b ^ res.value) & (1 << 4)) == (1 << 4));
		res.flag_c = (((a ^ b ^ res.value) & (1 << 8)) == (1 << 8));

		return res;
	}

	static Result16 add16(uint16_t a, uint16_t b) {
		uint32_t result = static_cast<uint32_t>(a) + static_cast<uint32_t>(b);
		Result16 res;

		res.value = static_cast<uint16_t>(result);
		res.flag_z = 0;
		res.flag_n = 0;
		res.flag_h = (((a ^ b ^ res.value) & (1 << 4)) == (1 << 12));
		res.flag_c = (((a ^ b ^ res.value) & (1 << 8)) == (1 << 16));

		return res;
	}

	static Result8 add8WithCarry(uint8_t a, uint8_t b, bool carry) {
		uint16_t result = static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + (carry ? 1 : 0);
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;
		res.flag_n = 0;
		res.flag_h = (((a ^ b ^ res.value) & (1 << 4)) == (1 << 4));
		res.flag_c = (((a ^ b ^ res.value) & (1 << 8)) == (1 << 8));

		return res;
	}
};
