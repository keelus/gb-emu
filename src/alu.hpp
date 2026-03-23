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
		res.flag_h = (((a & 0xF) + (b & 0xF)) > 0xF);
		res.flag_c = ((result & 0x100) != 0);

		return res;
	}

	static Result16 add16(uint16_t a, uint16_t b) {
		uint32_t result = static_cast<uint32_t>(a) + static_cast<uint32_t>(b);
		Result16 res;

		res.value = static_cast<uint16_t>(result);
		res.flag_z = 0;
		res.flag_n = 0;
		res.flag_h = (((a & 0xFFF) + (b & 0xFFF)) > 0xFFF);
		res.flag_c = ((result & 0x10000) != 0);

		return res;
	}

	static Result8 add8WithCarry(uint8_t a, uint8_t b, bool carry) {
		uint16_t result = static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + (carry ? 1 : 0);
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;
		res.flag_n = 0;
		res.flag_h = (((a ^ b ^ res.value) & (1 << 4)) == (1 << 4));
		res.flag_c = result > 0xFF;

		return res;
	}

	static Result8 sub8(uint8_t a, uint8_t b) {
		uint16_t result = static_cast<uint16_t>(a) - static_cast<uint16_t>(b);
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;
		res.flag_n = 1;
		res.flag_h = (((static_cast<int16_t>(a) & 0xF) - (static_cast<int16_t>(b) & 0xF)) < 0);
		res.flag_c = (a < b);

		return res;
	}

	static Result16 sub16(uint16_t a, uint16_t b) {
		uint32_t result = static_cast<uint32_t>(a) - static_cast<uint32_t>(b);
		Result16 res;

		res.value = static_cast<uint16_t>(result);

		return res;
	}

	static Result8 sub8WithCarry(uint8_t a, uint8_t b, bool carry) {
		int16_t result = static_cast<uint16_t>(a) - static_cast<uint16_t>(b) - (carry ? 1 : 0);
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;
		res.flag_n = 0;
		res.flag_h = ((a & 0xF) - (b & 0xF) - carry) < 0;
		res.flag_c = result < 0;

		return res;
	}

	static Result8 gateAnd(uint8_t a, uint8_t b) {
		uint8_t result = a & b;
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;

		return res;
	}

	static Result8 gateXor(uint8_t a, uint8_t b) {
		uint8_t result = a ^ b;
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;

		return res;
	}

	static Result8 gateOr(uint8_t a, uint8_t b) {
		uint8_t result = a | b;
		Result8 res;

		res.value = static_cast<uint8_t>(result);
		res.flag_z = res.value == 0;

		return res;
	}

	static Result8 inc8(uint8_t a) {
		Result8 res = add8(a, 1);
		return res;
	}

	static Result16 inc16(uint16_t a) {
		Result16 res = add16(a, 1);
		return res;
	}

	static Result8 dec8(uint8_t a) {
		Result8 res = sub8(a, 1);
		return res;
	}

	static Result16 dec16(uint16_t a) {
		Result16 res = sub16(a, 1);
		return res;
	}

	static Result8 rlc(uint8_t a) { return rl(a, a >> 7); }
	static Result8 rl(uint8_t a, bool flag_c) {
		Result8 res;

		res.flag_c = a >> 7;
		res.value = (a << 1) | static_cast<uint8_t>(flag_c);

		return res;
	}

	static Result8 rrc(uint8_t a) { return rr(a, a & 1); }
	static Result8 rr(uint8_t a, bool flag_c) {
		Result8 res;

		res.flag_c = a & 1;
		res.value = (a >> 1) | (static_cast<uint8_t>(flag_c) << 7);

		return res;
	}

	static Result8 sla(uint8_t a) {
		Result8 res;

		res.flag_c = a >> 7;
		res.value = (a << 1);

		return res;
	}

	static Result8 sra(uint8_t a) {
		Result8 res;

		res.flag_c = a & 1;
		res.value = (a & 0x80) | (a >> 1);

		return res;
	}

	static Result8 srl(uint8_t a) {
		Result8 res;

		res.flag_c = a & 1;
		res.value = (a >> 1);

		return res;
	}
};
