#include "SimpleHasher.h"
#include "Crypto/sha1.h"
#include "Crypto/PRNG.h"
#include <random>
#include "Common/Log.h"
#include <numeric>
#include <algorithm>

namespace bOPRF
{

	SimpleHasher::SimpleHasher()
	{
	}

	SimpleHasher::~SimpleHasher()
	{
	}

	void SimpleHasher::print() const
	{

		Log::out << Log::lock;
		Log::out << "Cuckoo Hasher  " << Log::endl;

		for (u64 k = 0; k < 3; k++)
		{
			std::vector<std::vector<u64>> bins = k ? (k == 1 ? mBins1 : mBins2) : mBins0;
			for (u64 i = 0; i < mBins0.size(); ++i)
			{
				Log::out << "Bin #" << k << "-" << i << Log::endl;

				size_t size = bins.size();
				if (size == 0)
				{

					Log::out << " contains 0 elements\n";
				}
				else
				{
					Log::out << " contains " << size << " elements\n";
					for (u64 j = 0; j < size; j++)
					{
						Log::out << "c_idx=" << bins[i][j] << "  hIdx=" << 0 << "\n";
					}
				}
				Log::out << Log::endl;
			}
		}
		Log::out << Log::endl;
		Log::out << Log::unlock;
	}

	void SimpleHasher::init(u64 cuckooSize, u64 simpleSize, u64 statSecParam, u64 numHashFunction)
	{
		if (numHashFunction != 3)
			throw std::runtime_error(LOCATION);

		mSimpleSize = simpleSize;
		mCuckooSize = cuckooSize;
		mBinCount = 2.4 * cuckooSize;
		mMaxBinSize = get_bin_size(mBinCount / 3, simpleSize, statSecParam);
		mBins0.resize(mBinCount / 3, std::vector<u64>());
		mBins1.resize(mBinCount / 3, std::vector<u64>());
		mBins2.resize(mBinCount / 3, std::vector<u64>());
	}

	u64 SimpleHasher::insertItems(std::array<std::vector<block>, 4> hashs)
	{

		u64 cnt = 0;
		u8 xrHash[SHA1::HashSize];

		SHA1 f;

		for (u64 i = 0; i < hashs[0].size(); ++i)
		{
			u64 addr;
			for (u64 k = 0; k < 3; ++k)
			{
				u64 xrHashVal = *(u64 *)&hashs[k][i] % (mBinCount / 3);
				addr = xrHashVal % (mBinCount / 3);
				std::vector<std::vector<u64>> &bins = k ? (k == 1 ? mBins1 : mBins2) : mBins0;
				bins[addr].push_back(i);
				cnt++;
			}
		}
		return cnt;
	}
}
