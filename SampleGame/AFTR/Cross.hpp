#include "ComponentBase.hpp"

namespace aftr
{
	class Cross : public ComponentBase
	{
	public:
		Cross();
		Directions GetOrientation() override;
		void Update() override;
	};
}