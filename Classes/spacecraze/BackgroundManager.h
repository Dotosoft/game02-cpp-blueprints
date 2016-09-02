#ifndef _SPACECRAZE_BACKGROUND_MANAGER_H_
#define _SPACECRAZE_BACKGROUND_MANAGER_H_

#include "spacecraze\GameGlobals.h"
#include "GameResources.h"

namespace spacecraze
{
	class BackgroundManager : public CCNode
	{
		public:
			virtual bool init();
			~BackgroundManager(void);

			CREATE_FUNC(BackgroundManager);

			void CreateStars();
			void update(float dt);

		private:
			CCArray* stars_;
			CCSize star_size_;
	};
}

#endif // _SPACECRAZE_BACKGROUND_MANAGER_H_