
#include "block_allocator.h"
#include <vector>
#include <algorithm>

#undef max

class BlockDataBuffer
{
public:
	BlockDataBuffer(size_t size) : ptr(new uint64_t[(size + 7) / 8 * 8]), s(size) { }
	~BlockDataBuffer() { delete[] ptr; }

	uint8_t* data() { return reinterpret_cast<uint8_t*>(ptr); }
	const uint8_t* data() const { return reinterpret_cast<const uint8_t*>(ptr); }
	size_t size() const { return s; }

	static std::shared_ptr<BlockDataBuffer> create(size_t size)
	{
		return std::make_shared<BlockDataBuffer>(size);
	}

private:
	uint64_t* ptr;
	size_t s;
};

class BlockAllocator_Impl
{
public:
	std::vector<std::shared_ptr<BlockDataBuffer>> blocks;
	size_t block_pos = 0;
};

BlockAllocator::BlockAllocator()
	: impl(std::make_shared<BlockAllocator_Impl>())
{
}

void *BlockAllocator::allocate(size_t size)
{
	if (impl->blocks.empty())
		impl->blocks.push_back(BlockDataBuffer::create(size * 10));
	std::shared_ptr<BlockDataBuffer> &cur = impl->blocks.back();
	if (impl->block_pos + size <= cur->size())
	{
		void *data = cur->data() + impl->block_pos;
		impl->block_pos += size;
		return data;
	}
	impl->blocks.push_back(BlockDataBuffer::create(std::max(cur->size() * 2, size)));
	impl->block_pos = size;
	return impl->blocks.back()->data();
}

void BlockAllocator::free()
{
	impl->blocks.clear();
	impl->block_pos = 0;
}

void *BlockAllocated::operator new(size_t size, BlockAllocator *allocator)
{
	return allocator->allocate(size);
}

void BlockAllocated::operator delete(void *data, size_t size)
{
}

void BlockAllocated::operator delete(void *data, BlockAllocator *allocator)
{
}
