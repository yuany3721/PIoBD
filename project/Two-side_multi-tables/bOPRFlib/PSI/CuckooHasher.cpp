#include "CuckooHasher.h"
#include "Crypto/sha1.h"
#include "Crypto/PRNG.h"
#include <random>
#include "Common/Log.h"
#include <numeric>

namespace bOPRF
{

	CuckooHasher::CuckooHasher()
		: mTotalTries(0)
	{
	}

	CuckooHasher::~CuckooHasher()
	{
	}

	void CuckooHasher::print() const
	{
		Log::out << Log::lock;
		Log::out << "Cuckoo Hasher  " << Log::endl;

		for (u64 k = 0; k < 3; k++)
		{
			std::vector<u64> bins = k ? (k == 1 ? mBins1 : mBins2) : mBins0;
			for (u64 i = 0; i < mBins0.size(); ++i)
			{
				Log::out << "Bin #" << k << "-" << i << Log::endl;

				if (mBins0[i] == -1)
				{

					Log::out << " contains 0 elements\n";
				}
				else
				{
					Log::out << " contains 1 elements\n"
							 << "    c_idx=" << mBins0[i] << "  hIdx=" << 0 << "\n";
				}
				Log::out << Log::endl;
			}

			Log::out << Log::endl;
			Log::out << Log::unlock;
		}
	}

	void CuckooHasher::init(u64 cuckooSize, u64 simpleSize)
	{
		mCuckooSize = cuckooSize;
		mSimpleSize = simpleSize;
		mBinCount = 2.4 * cuckooSize;
		mMaxStashSize = get_stash_size(cuckooSize);
		mSendersMaxBinSize = get_bin_size(mBinCount / 3, simpleSize, 40);
		mBins0.resize(mBinCount / 3, -1);
		mBins1.resize(mBinCount / 3, -1);
		mBins2.resize(mBinCount / 3, -1);
		mStash.resize(mMaxStashSize, -1);
	}

	void CuckooHasher::insertItem(u64 IdxItem, std::array<std::vector<block>, 4> &hashs, u64 hashIdx, u64 numTries)
	{
		++mTotalTries;

		u64 &xrHashVal = *(u64 *)&hashs[hashIdx][IdxItem];

		auto addr = (xrHashVal) % (mBinCount / 3);

		std::vector<u64> &bins = hashIdx ? (hashIdx == 1 ? mBins1 : mBins2) : mBins0;

		if (bins[addr] == -1)
		{
			// empty, place it here.
			bins[addr] = IdxItem;
		}
		else if (numTries < mCuckooSize)
		{
			// mN times => evict
			u64 evictIdx = bins[addr];
			bins[addr] = IdxItem;

			// increments tries, %3 we use only 3 hash functions!
			insertItem(evictIdx, hashs, (hashIdx + 1) % 3, numTries + 1);
		}
		else
		{
			// put in stash
			//hashIdx = 2; //assign the hash Idx be 2
			mStash.emplace_back(IdxItem);
		}
	}
	void CuckooHasher::insertItems(std::array<std::vector<block>, 4> &hashs)
	{
		for (u64 i = 0; i < hashs[0].size(); ++i)
		{
			insertItem(i, hashs);
		}

		//		Log::out << "#Stash size " << mStash.size() << Log::endl;

		mStash.resize(mMaxStashSize);
	}
}
