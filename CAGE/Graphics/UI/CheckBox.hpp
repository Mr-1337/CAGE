#pragma once
#include "Button.hpp"
#include "Text.hpp"
#include "../../IO/Assets/TextureManager.hpp"
#include "FlowLayout.hpp"
#include "LayoutGroup.hpp"

namespace cage
{
	namespace ui
	{
		class CheckBox : public LayoutGroup
		{
			class CheckBoxButton : public Button
			{
			public:
				CheckBoxButton(TextureManager& texManager);

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
			CheckBox(TextureManager& texManager, const std::string& label, const Font& font);
			bool Checked();

			bool HandleEvent(Event& e) override;

		private:
			std::shared_ptr<CheckBoxButton> m_button;
			std::string m_label;
		};
	}
}