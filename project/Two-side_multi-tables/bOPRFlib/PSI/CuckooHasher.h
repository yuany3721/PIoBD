#pragma once
#include "Common/Defines.h"
#include "Common/BitVector.h"
#include "Common/ArrayView.h"

namespace bOPRF
{
	class CuckooHasher
	{
	public:
		CuckooHasher();
		~CuckooHasher();

		u64 mBinCount, mCuckooSize, mSimpleSize, mMaxStashSize, mSendersMaxBinSize, mTotalTries;

		std::vector<u64> mBins0;
		std::vector<u64> mBins1;
		std::vector<u64> mBins2;
		std::vector<u64> mStash;

		void print() const;

		void init(u64 cuckooSize, u64 simpleSize);
		void insertItem(u64 IdxItem, std::array<std::vector<block>, 4> &hashs, u64 hashIdx = 0, u64 numTries = 0);
		void insertItems(std::array<std::vector<block>, 4> &hashs);
	};

}
