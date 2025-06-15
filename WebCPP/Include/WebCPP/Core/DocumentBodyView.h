#pragma once

#include "View.h"

namespace web
{
	class ModalLayer;

	class DocumentBodyView : public View
	{
	public:
		DocumentBodyView();

		void addView(View* view, bool grow = false, bool shrink = false);

		static DocumentBodyView* get();
		template<typename T>
		static T* get() { return static_cast<T*>(get()); }

		template<typename T>
		static void create() { new T(); }

		ModalLayer* beginModal();
		ModalLayer* beginUnshadedModal();
		void endModal();

		virtual void onNavigate() {}


	private:
		std::vector<ModalLayer*> modalLayers;

		static DocumentBodyView* instance;
	};

	class ModalLayer : public View
	{
	public:
		ModalLayer() : View("modallayer-view")
		{
		}

		JSValue oldActiveElement = JSValue::undefined();
	};

	class ShadedModalLayer : public ModalLayer
	{
	public:
		ShadedModalLayer()
		{
			addClass("shaded");
		}
	};
}
