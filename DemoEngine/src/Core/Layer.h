#pragma once

#include "Core/Core.h" 
#include "Core/Timestep.h"
#include "Events/Event.h"

namespace DemoEngine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer"); 
		virtual ~Layer();

		virtual void OnAttach() {};
		virtual void OnDetach() {};
		virtual void OnUpdate(Timestep ts) {};
		virtual void OnImGuiRender() {};
		virtual void OnEvent(Event& event) {};

		inline const std::string& GetName() const { return m_DebugName; }
	
	private:
		std::string m_DebugName;
	};
}