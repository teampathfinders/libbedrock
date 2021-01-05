//
// Created by RadiatedMonkey on 04/01/2021.
// Copyright (c) 2021 Pathfinders. All rights reserved
//


#include "bitstream.hpp"

namespace Pathfinders::Bedrock {
	BitStream::BitStream(size_t preallocated) noexcept : m_position(0) {
		m_buffer.reserve(preallocated);
	}

	BitStream::BitStream(const char *data, size_t length) noexcept : m_position(0) {
		m_buffer = std::vector<char>(data, data + length);
	}

	BitStream::BitStream(const std::string& data) noexcept : m_position(0) {
		m_buffer = std::vector<char>(data.begin(), data.end());
	}

	const std::vector<char>& BitStream::GetBuffer() const noexcept {
		return m_buffer;
	}

	// -- Writing --
	void BitStream::WriteByte(char value) noexcept {
		m_buffer.push_back(value);
	}

	void BitStream::WriteLittleInt(int value) noexcept {
		for(char i = 0; i < 4; i++) {
			m_buffer.push_back(value >> (i * 8));
		}
	}

	// -- Reading --
	char BitStream::ReadByte() noexcept {
		return m_buffer[m_position++];
	}

	int BitStream::ReadLittleInt() noexcept {
		int returnValue = 0;
		for(char i = 0; i < 4; i++){
			returnValue += m_buffer[m_position + i] << (i * 8);
		}
		m_position += 4;
		return returnValue;
	}

	void BitStream::Advance(size_t amount) noexcept {
		m_position += amount;
	}
}