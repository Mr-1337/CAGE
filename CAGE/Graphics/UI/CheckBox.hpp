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
				bool onRelease() override;

				friend class CheckBox;
			};

		public:
			CheckBox(const std::string& label, TTF_Font* font);
			bool Checked();

			bool HandleEvent(Event& e) override;

		private:
			std::shared_ptr<CheckBoxButton> m_button;
			std::string m_label;
			TTF_Font* m_font;
		};
	}
}