/***************************************************************************************
*    Title: Base64 encoding
*    Author: Martin Vorbrodt
*    Date: retrieved 12/06/2020
*    Code version: commit 920f9265ff763e8a89c6ee6385162dd6aec3dbad
*    Availability: https://github.com/mvorbrodt/blog/blob/master/src/base64.hpp
*    Code samples from https://vorbrodt.blog/
***************************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace base64
{
	inline static const char kEncodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	inline static const char kPadCharacter = '=';

	using byte = std::uint8_t;

	inline std::string encode(const std::vector<byte>& input)
	{
		std::string encoded;
		encoded.reserve(((input.size() / 3) + (input.size() % 3 > 0)) * 4);

		std::uint32_t temp{};
		auto it = input.begin();

		for(std::size_t i = 0; i < input.size() / 3; ++i)
		{
			temp  = (*it++) << 16;
			temp += (*it++) << 8;
			temp += (*it++);
			encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
			encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
			encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6 ]);
			encoded.append(1, kEncodeLookup[(temp & 0x0000003F)      ]);
		}

		switch(input.size() % 3)
		{
		case 1:
			temp = (*it++) << 16;
			encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
			encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
			encoded.append(2, kPadCharacter);
			break;
		case 2:
			temp  = (*it++) << 16;
			temp += (*it++) << 8;
			encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
			encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
			encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6 ]);
			encoded.append(1, kPadCharacter);
			break;
		}

		return encoded;
	}

	std::vector<byte> decode(const std::string& input)
	{
		if(input.length() % 4)
			throw std::runtime_error("Invalid base64 length!");

		std::size_t padding{};

		if(input.length())
		{
			if(input[input.length() - 1] == kPadCharacter) padding++;
			if(input[input.length() - 2] == kPadCharacter) padding++;
		}

		std::vector<byte> decoded;
		decoded.reserve(((input.length() / 4) * 3) - padding);

		std::uint32_t temp{};
		auto it = input.begin();

		while(it < input.end())
		{
			for(std::size_t i = 0; i < 4; ++i)
			{
				temp <<= 6;
				if (*it >= 0x41 && *it <= 0x5A) temp |= *it - 0x41;
				else if (*it >= 0x61 && *it <= 0x7A) temp |= *it - 0x47;
				else if (*it >= 0x30 && *it <= 0x39) temp |= *it + 0x04;
				else if (*it == 0x2B)                temp |= 0x3E;
				else if (*it == 0x2F)                temp |= 0x3F;
				else if (*it == kPadCharacter)
				{
					switch (input.end() - it)
					{
					case 1:
						decoded.push_back((temp >> 16) & 0x000000FF);
						decoded.push_back((temp >> 8) & 0x000000FF);
						return decoded;
					case 2:
						decoded.push_back((temp >> 10) & 0x000000FF);
						return decoded;
					default:
						throw std::runtime_error("Invalid padding in base64!");
					}
				}
				else throw std::runtime_error("Invalid character in base64!");
				++it;
			}

			decoded.push_back((temp >> 16) & 0x000000FF);
			decoded.push_back((temp >> 8 ) & 0x000000FF);
			decoded.push_back((temp      ) & 0x000000FF);
		}

		return decoded;
	}

	// Inspired by https://github.com/krypted/jwttools/blob/beee363ed91085f18b918098ca889d0f13fa4fc9/Jwt/Jwt/jwt/base64.hpp#L301 
	std::string uri_decode(const std::string& data)
	{
		const size_t sz = data.size();
		const size_t padding = 4 * static_cast<size_t>(sz % 4 != 0) - (sz % 4);
		std::string uri_dec(sz + padding, '=');
		for (size_t i = 0; i < sz; ++i)
		{
			switch (data[i]) {
			case '-':
				uri_dec[i] = '+';
				break;
			case '_':
				uri_dec[i] = '/';
				break;
			default:
				uri_dec[i] = data[i];
			};
		}
		auto decoded = decode(uri_dec);
		std::string decoded_str(begin(decoded), end(decoded));
		return decoded_str;
	}
}