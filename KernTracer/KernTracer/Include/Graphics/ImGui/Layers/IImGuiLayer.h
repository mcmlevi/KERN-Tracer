#pragma once
namespace RT
{
	class IImGuiLayer
	{
	public:
		IImGuiLayer() = default;
		virtual ~IImGuiLayer() = default;
		virtual void Update(float dt) = 0;
		
	};
}
