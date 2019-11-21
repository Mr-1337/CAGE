#pragma once
#include "Button.hpp"
#include "Text.hpp"

namespace cage
{
	namespace ui
	{
		class CheckBox : public UIElement
		{
			class CheckBoxButton : public Button
			{
			public:
				CheckBoxButton();

				bool Checked();

			private:

				bool m_selected;

				bool onClick() override;
				void onHover() override;
				void onUnHover() override;

				friend class CheckBox;
			};

		public:
			CheckBox(const std::string& label, TTF_Font* font);
			bool Checked();

		private:
			std::shared_ptr<UIElement> m_button;
			std::string m_label;
			TTF_Font* m_font;
		};
	}
}