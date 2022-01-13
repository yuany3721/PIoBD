#pragma once
#include "Common/Defines.h"
#include "Common/BitVector.h"
#include "Common/ArrayView.h"
#include "Common/Matrix.h"

namespace bOPRF
{

	class SimpleHasher
	{
	public:
		SimpleHasher();
		~SimpleHasher();

		u64 mBinCount, mMaxBinSize, mSimpleSize, mCuckooSize;
		std::vector<std::vector<u64> > mBins0;
		std::vector<std::vector<u64> > mBins1;
		std::vector<std::vector<u64> > mBins2;
		block mHashSeed;
		void print() const;

		void init(u64 cuckooSize, u64 simpleSize, u64 statSecParam = 40, u64 numHashFunction = 3);
		u64 insertItems(std::array<std::vector<block>, 4> hashs);
	};

}
