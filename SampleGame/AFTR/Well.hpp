#include "ComponentBase.hpp"

namespace aftr
{
	class Well : public ComponentBase
	{
	public:
		Well();
		bool GetState() override;
		void Update() override;
		Directions GetOrientation() override;
	};
}