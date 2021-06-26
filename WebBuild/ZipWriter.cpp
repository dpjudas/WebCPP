
#include "Precomp.h"
#include "ZipWriter.h"
#include "File.h"
#include "miniz.h"
#include <stdexcept>

class ZipWriterImpl : public ZipWriter
{
public:
	ZipWriterImpl(std::string filename)
	{
		file = File::create_always(std::move(filename));

		zip.m_pIO_opaque = this;
		zip.m_pWrite = &ZipWriterImpl::write;

		mz_bool result = mz_zip_writer_init_v2(&zip, 0, 0);
		if (result == MZ_FALSE)
			throw std::runtime_error("mz_zip_writer_init_v2 failed");
	}

	~ZipWriterImpl()
	{
		mz_zip_writer_end(&zip);
	}

	void add_file(const std::string& filename, const void* data, size_t size) override
	{
		mz_bool result = mz_zip_writer_add_mem(&zip, FilePath::force_slash(filename).c_str(), data, size, MZ_DEFAULT_COMPRESSION);
		if (result == MZ_FALSE)
			throw std::runtime_error("mz_zip_writer_add_mem failed");
	}

	void finalize_archive() override
	{
		mz_zip_writer_finalize_archive(&zip);
	}

	static size_t write(void* pOpaque, mz_uint64 file_ofs, const void* pBuf, size_t n)
	{
		ZipWriterImpl* impl = static_cast<ZipWriterImpl*>(pOpaque);
		if (file_ofs != impl->offset)
		{
			impl->file->seek(file_ofs);
			impl->offset = file_ofs;
		}
		impl->file->write(pBuf, n);
		impl->offset += n;
		return n;
	}

	std::shared_ptr<File> file;
	mz_uint64 offset = 0;
	mz_zip_archive zip = {};
};

std::unique_ptr<ZipWriter> ZipWriter::create(std::string filename)
{
	return std::make_unique<ZipWriterImpl>(std::move(filename));
}
