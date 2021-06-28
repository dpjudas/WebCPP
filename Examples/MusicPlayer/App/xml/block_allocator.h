
#pragma once

#include <memory>

class BlockAllocator_Impl;

class BlockAllocator
{
public:
	BlockAllocator();

	void *allocate(size_t size);
	void free();

private:
	std::shared_ptr<BlockAllocator_Impl> impl;
};

class BlockAllocated
{
public:
	void *operator new(size_t size, BlockAllocator *allocator);
	void operator delete(void *data, size_t size);
	void operator delete(void *data, BlockAllocator *allocator);
};
