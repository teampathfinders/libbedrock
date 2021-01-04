#ifndef BEDROCK_FORMAT_FORMAT_HPP
#define BEDROCK_FORMAT_FORMAT_HPP

namespace leveldb {
	class DB;
}

class BedrockWorld {
	public:
		BedrockWorld(const char* path);
		~BedrockWorld();

	private:
		leveldb::DB* db;
};

#endif //BEDROCK_FORMAT_FORMAT_HPP
