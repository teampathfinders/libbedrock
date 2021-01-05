//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//

#ifndef BEDROCK_FORMAT_BITSTREAM_HPP
#define BEDROCK_FORMAT_BITSTREAM_HPP

#include <vector>
#include <string>

namespace Pathfinders::Bedrock {
	class BitStream {
		public:
			explicit BitStream(size_t preallocated = 0) noexcept;
			explicit BitStream(const char* data, size_t length) noexcept;
			explicit BitStream(const std::string& data) noexcept;
			~BitStream() noexcept = default;

			// -- Writing --
			void WriteByte(char value) noexcept;
			void WriteLittleInt(int value) noexcept;

			// -- Reading --
			[[nodiscard]] char ReadByte() noexcept;
			[[nodiscard]] int ReadLittleInt() noexcept;

			[[nodiscard]] const std::vector<char>& GetBuffer() const noexcept;

			void Advance(size_t amount) noexcept;

		private:
			std::vector<char> m_buffer;
			size_t m_position;
	};
}

#endif // BEDROCK_FORMAT_BITSTREAM_HPP