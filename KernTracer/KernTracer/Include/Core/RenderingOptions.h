#pragma once
namespace RT
{
	namespace RenderingOptions
	{
		constexpr int NOEFFECT = 0;
		constexpr int NORMAL = 1 << 1;
		constexpr int DEPTH = 1 << 2;
		constexpr int VIEWBVH = 1 << 3;
		constexpr int PIXELCOST = 1 << 4;

		struct OptionsData
		{
			int Mode = NOEFFECT;
			float PixelCostSensitivity = 1.f;
		};
	}
}
