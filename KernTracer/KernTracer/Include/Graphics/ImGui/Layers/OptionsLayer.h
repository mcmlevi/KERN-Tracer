#pragma once
#include <Graphics/ImGui/Layers/IImGuiLayer.h>
#include <signals/signals/Delegate.h>

enum eOptions
{
	NONE = 0,
	NORMALS,
	DEPTH,
};
namespace RT
{
	class OptionsLayer : public IImGuiLayer
	{
	public:
		void Update(float dt) override;
		SIG::Delegate<int32_t> m_setOptionsDelegate{};
	private:
		bool m_showNormals = false;
		int radioButton = 0;
	};
	
}


