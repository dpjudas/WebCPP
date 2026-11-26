
#include "Precomp.h"
#include "ZipReader.h"
#define MINIZ_NO_DEFLATE_APIS
#include "miniz.h"
#include <algorithm>

namespace web
{
	class ZipReaderImpl : public ZipReader
	{
	public:
		ZipReaderImpl(std::shared_ptr<IODevice> input) : input(input)
		{
			mz_zip_zero_struct(&zip);
			zip.m_pRead = &ZipReaderImpl::read_func;
			zip.m_pIO_opaque = this;
			mz_bool result = mz_zip_reader_init(&zip, input->size(), 0);
			if (result == MZ_FALSE)
				throw std::runtime_error("mz_zip_reader_init failed");

			mz_uint count = mz_zip_reader_get_num_files(&zip);
			for (mz_uint i = 0; i < count; i++)
			{
				mz_zip_archive_file_stat fileinfo;
				mz_bool result = mz_zip_reader_file_stat(&zip, i, &fileinfo);
				if (result)
				{
					ZipFileEntry entry;
					entry.fileIndex = fileinfo.m_file_index;
					entry.centralDirOffset = fileinfo.m_central_dir_ofs;
					entry.versionMadeBy = fileinfo.m_version_made_by;
					entry.versionNeeded = fileinfo.m_version_needed;
					entry.bitFlag = fileinfo.m_bit_flag;
					entry.method = fileinfo.m_method;
					entry.time = fileinfo.m_time;
					entry.fileCrc32 = fileinfo.m_crc32;
					entry.compSize = fileinfo.m_comp_size;
					entry.uncompSize = fileinfo.m_uncomp_size;
					entry.internalAttr = fileinfo.m_internal_attr;
					entry.externalAttr = fileinfo.m_external_attr;
					entry.localHeaderOffset = fileinfo.m_local_header_ofs;
					entry.commentSize = fileinfo.m_comment_size;
					entry.isDirectory = fileinfo.m_is_directory;
					entry.isEncrypted = fileinfo.m_is_encrypted;
					entry.isSupported = fileinfo.m_is_supported;
					entry.filename = fileinfo.m_filename;
					entry.comment = fileinfo.m_comment;
					files.push_back(std::move(entry));
				}
			}
		}

		~ZipReaderImpl()
		{
			if (state)
				mz_zip_reader_extract_iter_free(state);
			mz_zip_reader_end(&zip);
		}

		const std::vector<ZipFileEntry>& getFiles() override
		{
			return files;
		}

		std::string readAllText(uint32_t file_index)
		{
			auto data = readAllBytes(file_index);
			return std::string(data->data(), data->size());
		}

		std::string readAllText(const std::string& filename)
		{
			auto data = readAllBytes(filename);
			return std::string(data->data(), data->size());
		}

		static std::string getKey(std::string filename)
		{
			for (char& c : filename)
			{
				if (c == '\\')
					c = '/';
			}
			return filename;
		}

		uint32_t getFileIndex(const std::string& filename)
		{
			std::string key = getKey(filename);
			if (filenameToIndex.empty())
			{
				for (const auto& entry : files)
				{
					filenameToIndex[getKey(entry.filename)] = entry.fileIndex;
				}
			}

			auto it = filenameToIndex.find(filename);
			if (it == filenameToIndex.end())
				throw std::runtime_error("File not found in archive: " + filename);
			return it->second;
		}

		std::shared_ptr<DataBuffer> readAllBytes(const std::string& filename) override
		{
			return readAllBytes(getFileIndex(filename));
		}

		std::shared_ptr<DataBuffer> readAllBytes(uint32_t file_index) override
		{
			if (files[file_index].uncompSize == 0)
				return DataBuffer::create(0);

			auto data = DataBuffer::create(files[file_index].uncompSize);
			mz_bool result = mz_zip_reader_extract_to_mem(&zip, file_index, data->data(), data->size(), 0);
			if (!result)
				throw std::runtime_error("Could not read all zip entry data");
			return data;
		}

		ZipCompressionMethod getCompressionMethod(const std::string& filename) override
		{
			return getCompressionMethod(getFileIndex(filename));
		}

		ZipCompressionMethod getCompressionMethod(uint32_t file_index) override
		{
			switch (files[file_index].method)
			{
			case 0:
				return ZipCompressionMethod::uncompressed;
			case MZ_DEFLATED:
				return ZipCompressionMethod::deflate;
			default:
				return ZipCompressionMethod::unknown;
			}
		}

		std::shared_ptr<DataBuffer> readAllCompressedBytes(const std::string& filename) override
		{
			return readAllCompressedBytes(getFileIndex(filename));
		}

		std::shared_ptr<DataBuffer> readAllCompressedBytes(uint32_t file_index)
		{
			if (files[file_index].compSize == 0)
				return DataBuffer::create(0);

			auto data = DataBuffer::create(files[file_index].compSize);
			mz_bool result = mz_zip_reader_extract_to_mem(&zip, file_index, data->data(), data->size(), MZ_ZIP_FLAG_COMPRESSED_DATA);
			if (!result)
				throw std::runtime_error("Could not read all zip entry data");
			return data;
		}

		static size_t read_func(void* opaque, mz_uint64 file_offset, void* data, size_t size)
		{
			ZipReaderImpl* self = static_cast<ZipReaderImpl*>(opaque);
			if (file_offset != self->pos)
			{
				if (self->input->seek(file_offset) != file_offset)
					throw std::runtime_error("ZipReader seek failed");
				self->pos = file_offset;
			}

			int intsize = (int)std::min(size, (size_t)0x70000000);
			size_t bytes = self->input->try_read(data, intsize);
			self->pos += bytes;
			return bytes;
		}

		uint32_t getCrc32(uint32_t file_index) override
		{
			mz_zip_archive_file_stat stat;
			mz_bool result = mz_zip_reader_file_stat(&zip, file_index, &stat);
			if (result == MZ_FALSE)
				throw std::runtime_error("mz_zip_reader_file_stat failed");

			return stat.m_crc32;
		}

		uint32_t getCrc32(const std::string& filename) override
		{
			return getCrc32(getFileIndex(filename));
		}

		std::shared_ptr<IODevice> input;
		mz_zip_archive zip = {};
		mz_uint64 pos = 0;
		int file_index = -1;
		mz_zip_reader_extract_iter_state* state = nullptr;
		std::vector<ZipFileEntry> files;
		std::map<std::string, uint32_t> filenameToIndex;
	};

	std::unique_ptr<ZipReader> ZipReader::open(std::shared_ptr<IODevice> input)
	{
		return std::make_unique<ZipReaderImpl>(std::move(input));
	}
}
