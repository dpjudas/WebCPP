
#include "OpenFileDialog.h"
#include "WebCPP/Core/DocumentBodyView.h"
#include "WebCPP/Core/JSCallback.h"

class FileInputView : public View
{
public:
	FileInputView(const std::string& filter, bool multiple) : View("input")
	{
		element->setAttribute("type", "file");
		element->setStyle("display", "none");

		if (multiple)
			element->setAttribute("multiple", {});

		if (!filter.empty())
			element->setAttribute("accept", filter);

		delete instance;
		instance = this;
	}

	~FileInputView()
	{
		instance = nullptr;
	}

	void click()
	{
		element->handle.call<void>("click");
	}

	std::vector<OpenFileEntry> fileList()
	{
		std::vector<OpenFileEntry> files;
		for (const JSValue& file : vecFromJSArray<JSValue>(element->handle["files"]))
		{
			files.push_back(OpenFileEntry(file));
		}
		return files;
	}

	static FileInputView* instance;
};

FileInputView* FileInputView::instance = nullptr;

/////////////////////////////////////////////////////////////////////////////

void OpenFileDialog::show(const std::string& filter, bool multiple, std::function<void(std::vector<OpenFileEntry> files)> selectedCallback)
{
	auto fileDialog = new FileInputView(filter, multiple);

	DocumentBodyView::get()->addView(fileDialog);

	fileDialog->element->addEventListener("change", [=](Event* event) { event->stopPropagation(); selectedCallback(fileDialog->fileList()); });
	fileDialog->click();
}

/////////////////////////////////////////////////////////////////////////////

OpenFileEntry::OpenFileEntry(JSValue file) : handle(std::move(file))
{
}

std::string OpenFileEntry::name() const
{
	return handle["name"].as<std::string>();
}

int64_t OpenFileEntry::size() const
{
	return (int64_t)handle["size"].as<double>();
}

std::string OpenFileEntry::mimeType() const
{
	return handle["type"].as<std::string>();
}

int64_t OpenFileEntry::lastModified() const
{
	return (int64_t)handle["lastModified"].as<double>();
}

void OpenFileEntry::readAllText(std::function<void(std::string text)> onResult)
{
	JSValue promise = handle.call<JSValue>("text");
	auto callback = new JSCallback([=](JSValue args) { onResult(args[0].as<std::string>()); return JSValue::undefined(); }, true);
	promise.call<JSValue>("then", callback->getHandler());
}

void OpenFileEntry::readAllBytes(std::function<void(std::vector<uint8_t> data)> onResult)
{
	JSValue promise = handle.call<JSValue>("arrayBuffer");
	auto callback = new JSCallback([=](JSValue args)
		{
			JSValue uint8Array = JSValue::global("Uint8Array").new_(args[0]);
			onResult(vecFromJSArray<uint8_t>(uint8Array));
			return JSValue::undefined();
		}, true);
	promise.call<JSValue>("then", callback->getHandler());
}
