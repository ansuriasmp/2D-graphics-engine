#pragma once

#include "Core.h"
#include "Window.h"
#include "Core/Timestep.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"


int main(int argc, char** argv);

namespace DemoEngine
{
	class Application
	{
	public:
		Application(const std::string& name = "DemoEngine"); 
		~Application();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		
		inline Window& GetWindow() { return *m_Window; } 
		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer;}

		static Application& Get() { return *s_Instance; }
	
	//private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window; 
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
		friend int main(int argc, char** argv);
	};

	//to be defined in client
	Application* CreateApplication();
};
