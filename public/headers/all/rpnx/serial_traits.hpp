/*
Copyright (c) 2016, 2017, 2018, 2020 Ryan P. Nicholl <rnicholl@protonmail.com> http://rpnx.net/
RPNX Serial Traits 2
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef RPNX_SERIAL_TRAITS_2_HPP
#define RPNX_SERIAL_TRAITS_2_HPP

#include <cinttypes>
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <deque>


namespace rpnx
{

	// These are specialization placeholders
	// They are used to represent integers of any
	// size in a machine independent format.
	struct intany;
	struct uintany;

	template <typename I>
	struct big_endian;
	
	template <typename I>
	using little_endian = I;



	template <typename T>
	struct serial_traits;

	template <typename T, typename ItFunctor>
	struct synchronous_functor_serialize_traits;

	template <typename T, typename It>
	struct synchronous_iterator_serial_traits;


	template <typename T, typename It>
	auto quick_serialize(T const& val, It iterator)->It;

	// The following 4 specializations are traits for unsigned integers
	// has_fixed_serial_size should return true, and returns the number of bytes in
	// an integer
	template <>
	struct serial_traits<std::uint8_t>
	{
		static inline constexpr bool has_fixed_serial_size() noexcept { return true; }
		static inline constexpr std::size_t fixed_serial_size() noexcept { return 1; }
		static inline constexpr std::size_t serial_size(const std::uint8_t&) noexcept { return 1; }
	};

	
	template <>
	struct serial_traits<std::uint16_t>
	{
		static inline constexpr bool has_fixed_serial_size() noexcept { return true; }
		static inline constexpr std::size_t fixed_serial_size() noexcept { return 2; }
		static inline constexpr std::size_t serial_size(const std::uint16_t&) noexcept  { return 2; }
	};

	template <>
	struct serial_traits<std::uint32_t>
	{
		static inline constexpr bool has_fixed_serial_size() noexcept { return true; }
		static inline constexpr std::size_t fixed_serial_size() noexcept  { return 4; }
		static inline constexpr std::size_t serial_size(const std::uint32_t&) noexcept { return 4; }
	};

	template <>
	struct serial_traits<std::uint64_t>
	{
		static inline constexpr bool has_fixed_serial_size() noexcept { return true; }
		static inline constexpr std::size_t fixed_serial_size() noexcept { return 8; }
		static inline constexpr std::size_t serial_size(const std::uint64_t&) noexcept { return 8; }
	};


	// The following 4 specializations are for serial synchronous functor interfaces for the 
	// unsigned integers

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::uint8_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::uint8_t val)
		{
			auto it = out_functor(1);
			*it++ = val;
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::uint8_t& val)
		{
			auto it = in_functor(1);
			val = *it++;
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::uint16_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::uint16_t val)
		{
			auto it = out_functor(2);
			*it++ = val & 0xFF;
			*it = (val >> 8) & 0xFF;
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::uint16_t& val)
		{
			auto it = in_functor(2);
			val = *it++;
			val |= (*it << 8);
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::uint32_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::uint32_t val)
		{
			auto it = out_functor(4);
			*it++ = val & 0xFF;
			*it++ = (val >> 8) & 0xFF;
			*it++ = (val >> 16) & 0xFF;
			*it = (val >> 24) & 0xFF;
			// TODO: Update this to use iterator trait specializations
			//synchronous_iterator_serialize_traits<std::uint32_t, std::remove_reference_t<decltype(*it)>>(it, val);
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::uint32_t& val)
		{
			auto it = in_functor(4);
			val = *it++;
			val |= (*it++ << 8);
			val |= (*it++ << 16);
			val |= (*it++ << 24);
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::uint64_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::uint64_t val)
		{
			auto it = out_functor(8);
			*it++ = val & 0xFF;
			*it++ = (val >> 8) & 0xFF;
			*it++ = (val >> 16) & 0xFF;
			*it++ = (val >> 24) & 0xFF;
			*it++ = (val >> 32) & 0xFF;
			*it++ = (val >> 40) & 0xFF;
			*it++ = (val >> 48) & 0xFF;
			*it = (val >> 56) & 0xFF;
			// TODO: Update this to use iterator trait specializations
			//synchronous_iterator_serialize_traits<std::uint32_t, std::remove_reference_t<decltype(*it)>>(it, val);
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::uint64_t& val)
		{
			auto it = in_functor(8);
			val = *it++;
			val |= (*it++ << 8);
			val |= (*it++ << 16);
			val |= (*it++ << 24);
			val |= (*it++ << 32);
			val |= (*it++ << 40);
			val |= (*it++ << 48);
			val |= (*it << 56);
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::int8_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::int8_t val)
		{
			auto it = out_functor(1);
			*it++ = val;
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::int8_t& val)
		{
			auto it = in_functor(1);
			val = *it++;
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::int16_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::int16_t val)
		{
			auto it = out_functor(2);
			*it++ = val & 0xFF;
			*it = (val >> 8) & 0xFF;
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::int16_t& val)
		{
			auto it = in_functor(2);
			val = *it++;
			val |= (*it << 8);
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::int32_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::int32_t val)
		{
			auto it = out_functor(4);
			*it++ = val & 0xFF;
			*it++ = (val >> 8) & 0xFF;
			*it++ = (val >> 16) & 0xFF;
			*it = (val >> 24) & 0xFF;
			// TODO: Update this to use iterator trait specializations
			//synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::int32_t& val)
		{
			auto it = in_functor(4);
			val = *it++;
			val |= (*it++ << 8);
			val |= (*it++ << 16);
			val |= (*it++ << 24);
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::int64_t, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::int64_t val)
		{
			auto it = out_functor(8);
			*it++ = val & 0xFF;
			*it++ = (val >> 8) & 0xFF;
			*it++ = (val >> 16) & 0xFF;
			*it++ = (val >> 24) & 0xFF;
			*it++ = (val >> 32) & 0xFF;
			*it++ = (val >> 40) & 0xFF;
			*it++ = (val >> 48) & 0xFF;
			*it = (val >> 56) & 0xFF;
			// TODO: Update this to use iterator trait specializations
			//synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::int64_t& val)
		{
			auto it = in_functor(8);
			val = *it++;
			val |= (*it++ << 8);
			val |= (*it++ << 16);
			val |= (*it++ << 24);
			val |= (*it++ << 32);
			val |= (*it++ << 40);
			val |= (*it++ << 48);
			val |= (*it << 56);
		}
	};




	template <>
	struct serial_traits<uintany>
	{
		static inline constexpr bool has_fixed_serial_size() { return false; }
		static inline constexpr std::size_t serial_size(std::uint64_t value) 
		{
			if (value < 128ull) return 1;
			else if (value < 16512ull) return 2;
			else if (value < 2113664ull) return 3;
			else if (value < 270549120ull) return 4;
			else if (value < 34630287488ull) return 5;
			else if (value < 4432676798592ull) return 6;
			else if (value < 567382630219904ull) return 7;
			else if (value < 72624976668147840ull) return 8;
			else if (value < 9295997013522923648ull) return 9;
			else return 10; // 10 units can represent all 64-bit integers.
		}
	};

	template <typename It>
	struct synchronous_iterator_serial_traits<uintany, It>
	{
		static inline It quick_serialize(uintmax_t in, It out)
		{
			// This isn't the best function, it might give wrong results for really large values.
			// It's good enough for now.
			// Also, optimize this, it's not very fast

			uintmax_t base = in;

			uintmax_t bytecount = 1;
			uintmax_t max = 0;

			max = (1ull << (7)) - 1;
			while (base > max)
			{
				bytecount++;
				base -= max + 1;
				max = (1ull << ((7) * bytecount)) - 1;
			}

			for (uintmax_t i = 0; i < bytecount; i++)
			{
				uint8_t val = base & ((uintmax_t(1) << (7)) - 1);
				if (i != bytecount - 1)
				{
					val |= 0b10000000;
				}
				*out++ = val;
				base >>= 7;
			}
			return out;
		}

		static inline It quick_deserialize(uintmax_t& n, It in)
		{
			// This function is slow, optimize it
			n = 0;
			uintmax_t n2 = 0;
			while (true)
			{
				uint8_t a = *in++;
				uint8_t read_value = a & 0b1111111;
				n += (uintmax_t(read_value) << (n2 * 7));
				if (!(a & 0b10000000)) break;
				n2++;
			}
			for (uintmax_t i = 1; i <= n2; i++)
			{
				n += (uintmax_t(1) << (i * 7));
			}
			return in;
		}
	};

	template <typename T, typename A>
	struct serial_traits<std::vector<T, A>>
	{
		static inline constexpr bool has_fixed_serial_size() { return false; }
		static inline constexpr std::size_t serial_size(std::vector<T, A> const& value)
		{
			if constexpr (serial_traits<typename std::vector<T, A>::value_type>::has_fixed_serial_size())
			{
				return serial_traits<uintany>::serial_size(value.size()) + serial_traits<std::vector<T, A>::value_type>::fixed_serial_size() * value.size();
			}
			else
			{
				std::size_t result = 0;
				result += serial_traits<uintany>::serial_size(value.size());
				for (auto const& x : value)
				{
					result += serial_triats<typename std::vector<T, A>::value_type>::serial_size(x);
				}
				return result;				
			}
		}
	};

	template <>
	struct serial_traits<std::string>
	{
		static inline constexpr bool has_fixed_serial_size() { return false; }
		static inline std::size_t serial_size(std::string const& value)
		{
			return serial_traits<uintany>::serial_size(value.size()) +  value.size();
		}
	};

	template <typename ItFunctor>
	struct synchronous_functor_serialize_traits<std::string, ItFunctor>
	{
		static inline constexpr auto sync_serialize(ItFunctor out_functor, std::string const & val)
		{
			auto it = out_functor(serial_traits<std::string>::serial_size(val));
			it = synchronous_iterator_serial_traits<uintany, decltype(it)>::quick_serialize(val.size(), it);
			std::copy(val.begin(), val.end(), it);
			
			// TODO: Update this to use iterator trait specializations
			//synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
		}

		static inline constexpr auto sync_deserialize(ItFunctor in_functor, std::int64_t& val)
		{
			auto it = in_functor(8);
			val = *it++;
			val |= (*it++ << 8);
			val |= (*it++ << 16);
			val |= (*it++ << 24);
			val |= (*it++ << 32);
			val |= (*it++ << 40);
			val |= (*it++ << 48);
			val |= (*it << 56);
		}
	};


	template <typename I>
	struct serial_traits<big_endian<I>>
		: serial_traits<I>
	{
		static_assert(std::is_integral<I>::value);
	};
	
	template <typename T, typename ItF>
	inline void quick_functor_serialize(T const& t, ItF f)
	{
		synchronous_functor_serialize_traits<T, ItF>::sync_serialize(f, t);
	}

}

#endif