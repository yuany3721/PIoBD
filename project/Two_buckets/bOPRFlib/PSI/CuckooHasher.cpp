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
		for (u64 i = 0; i < mBins0.size(); ++i)
		{
			Log::out << "Bin #" << i << Log::endl;

			if (mBins0[i].isEmpty())
			{

				Log::out << " contains 0 elements\n";
			}
			else
			{
				Log::out << " contains 0 elements\n"
						 << "    c_idx=" << mBins0[i].mIdx << "  hIdx=" << mBins0[i].mHashIdx << "\n";
			}
			Log::out << Log::endl;
		}
		Log::out << Log::endl;
		Log::out << Log::unlock;
	}

	void CuckooHasher::init(u64 cuckooSize, u64 simpleSize)
	{
		mCuckooSize = cuckooSize;
		mSimpleSize = simpleSize;
		// mBinCount = 1.2 * cuckooSize;
		mBinCount = 0.6 * cuckooSize;
		mMaxStashSize = get_stash_size(cuckooSize);
		mSendersMaxBinSize = get_bin_size(mBinCount, simpleSize * 2, 40);
		// mBins.resize(mBinCount);
		mBins0.resize(mBinCount);
		mBins1.resize(mBinCount);
		lastEvicted.resize(mBinCount, 0);
	}

	void CuckooHasher::insertItem(u64 IdxItem, std::array<std::vector<block>, 4> &hashs, u64 hashIdx, u64 numTries)
	{
		++mTotalTries;

		u64 &xrHashVal = *(u64 *)&hashs[hashIdx][IdxItem];

		auto addr = (xrHashVal) % mBinCount;

		u64 i = lastEvicted[addr];
		lastEvicted[addr] = (i + 1) % 2;

		std::vector<Bin> &mBins = i ? mBins1 : mBins0;

		if (mBins[addr].isEmpty())
		{
			// empty, place it here.
			mBins[addr].mIdx = IdxItem;
			mBins[addr].mHashIdx = hashIdx;
		}
		else if (numTries < mCuckooSize)
		{
			// mN times => evict
			u64 evictIdx = mBins[addr].mIdx;
			u64 idxHash = mBins[addr].mHashIdx;

			mBins[addr].mIdx = IdxItem;
			mBins[addr].mHashIdx = hashIdx;

			// increments tries, %3 we use only 3 hash functions!
			// insertItem(evictIdx, hashs, (idxHash + 1) % 3, numTries + 1);

			// increments tries, %2 we use only 2 hash functions!
			insertItem(evictIdx, hashs, (idxHash + 1) % 2, numTries + 1);
		}
		else
		{
			// put in stash
			// hashIdx = 2; //assign the hash Idx be 2
			mStash.emplace_back(IdxItem, hashIdx);
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
