
#include "ZipReader.h"
#include "File.h"
#include "miniz.h"
#include <stdexcept>

class ZipReaderImpl : public ZipReader
{
public:
	ZipReaderImpl(std::string filename)
	{
		file = File::open_existing(std::move(filename));

		zip.m_pIO_opaque = this;
		zip.m_pRead = &ZipReaderImpl::read;

		mz_bool result = mz_zip_reader_init(&zip, file->size(), 0);
		if (result == MZ_FALSE)
			throw std::runtime_error("mz_zip_reader_init failed");
	}

	~ZipReaderImpl()
	{
		mz_zip_reader_end(&zip);
	}

	std::vector<uint8_t> read_all_bytes(const std::string& filename) override
	{
		int file_index = mz_zip_reader_locate_file(&zip, filename.c_str(), nullptr, 0);
		if (file_index == -1)
			throw std::runtime_error("File " + filename + " not found in archive");

		mz_zip_archive_file_stat stat;
		mz_bool result = mz_zip_reader_file_stat(&zip, file_index, &stat);
		if (result == MZ_FALSE)
			throw std::runtime_error("mz_zip_reader_file_stat failed");

		std::vector<uint8_t> buffer(stat.m_uncomp_size);
		if (!buffer.empty())
		{
			mz_bool result = mz_zip_reader_extract_to_mem(&zip, file_index, buffer.data(), buffer.size(), 0);
			if (result == MZ_FALSE)
				throw std::runtime_error("mz_zip_reader_extract_file_to_mem failed");
		}
		return buffer;
	}

	std::string read_all_text(const std::string& filename) override
	{
		int file_index = mz_zip_reader_locate_file(&zip, filename.c_str(), nullptr, 0);
		if (file_index == -1)
			throw std::runtime_error("File " + filename + " not found in archive");

		mz_zip_archive_file_stat stat;
		mz_bool result = mz_zip_reader_file_stat(&zip, file_index, &stat);
		if (result == MZ_FALSE)
			throw std::runtime_error("mz_zip_reader_file_stat failed");

		std::string buffer;
		buffer.resize(stat.m_uncomp_size);
		if (!buffer.empty())
		{
			mz_bool result = mz_zip_reader_extract_to_mem(&zip, file_index, &buffer[0], buffer.size(), 0);
			if (result == MZ_FALSE)
				throw std::runtime_error("mz_zip_reader_extract_file_to_mem failed");
		}
		return buffer;
	}

	static size_t read(void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n)
	{
		ZipReaderImpl* impl = static_cast<ZipReaderImpl*>(pOpaque);
		if (file_ofs != impl->offset)
		{
			impl->file->seek(file_ofs);
			impl->offset = file_ofs;
		}
		impl->file->read(pBuf, n);
		impl->offset += n;
		return n;
	}

	std::shared_ptr<File> file;
	mz_uint64 offset = 0;
	mz_zip_archive zip = {};
};

std::unique_ptr<ZipReader> ZipReader::open(std::string filename)
{
	return std::make_unique<ZipReaderImpl>(std::move(filename));
}
