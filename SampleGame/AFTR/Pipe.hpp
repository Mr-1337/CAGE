#include "ComponentBase.hpp"

namespace aftr
{
	class Pipe : public ComponentBase
	{
		Directions m_orientation;
	public:
		Pipe(Directions orientation);
		void Update() override;

		Directions GetOrientation() override;
	};
}