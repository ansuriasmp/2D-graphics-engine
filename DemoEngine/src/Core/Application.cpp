#include "DemoEngine_PCH.h" 
#include "Application.h"

#include <GLFW/glfw3.h>

#include "Input.h"
#include "Renderer/2D/Renderer2D.h"

namespace DemoEngine
{

	// Macro to bind an event function to this instance of Application
#define BIND_EVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)

	// Static instance pointer for global access
	Application* Application::s_Instance = nullptr;

	// Constructor initializes the application with a window and required subsystems
	Application::Application(const std::string& name)
	{
		s_Instance = this;

		// Create the main window with a given name
		m_Window = Window::Create(WindowProps(name));
		// Set callback to handle events through the OnEvent function
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		// Initialize input system
		Input::Init();
		Input::SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		// Initialize 2D renderer
		Renderer2D::Init();

		// Create and add ImGui overlay
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	// Destructor
	Application::~Application()
	{

	}

	// Marks the application as not running (to exit the main loop)
	void Application::Close()
	{
		m_Running = false;
	}

	// Handles all events passed by the window system
	void Application::OnEvent(Event& e)
	{
		// Dispatch window close and resize events
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OnWindowResize));

		// Send the event to all layers in reverse order (top to bottom)
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled) {
				break; // Stop if the event has already been handled
			}
			(*it)->OnEvent(e); // Let the layer handle the event
		}
	}

	// Main application loop
	void Application::Run()
	{
		while (m_Running)
		{
			// Get current time and calculate the time step between frames
			float time = (float)glfwGetTime(); // Platform::GetTime() 
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// Only update if the window is not minimized
			if (!m_Minimized)
			{
				// Update all layers
				{
					for (Layer* layer : m_LayerStack) {
						layer->OnUpdate(timestep);
					}
				}

				// Start ImGui frame
				m_ImGuiLayer->Begin();
				{
					// Render ImGui for each layer
					for (Layer* layer : m_LayerStack)
					{
						layer->OnImGuiRender();
					}
				}
				// End ImGui frame
				m_ImGuiLayer->End();

				// Update input state (key presses, mouse, etc.)
				Input::Update();
			}

			// Update the window (swap buffers, poll events, etc.)
			m_Window->OnUpdate();
		}
	}

	// Adds a new layer to the layer stack
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach(); // Call setup logic
	}

	// Adds a new overlay (rendered on top of everything else)
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach(); // Call setup logic
	}

	// Event handler for window close
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false; // Stop application loop
		return true; // Event handled
	}

	// Event handler for window resize
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true; // Pause updates while minimized
			return false; // Not fully handled
		}
		m_Minimized = false; // Resume updates
		return false; // Event not fully handled, let others respond
	}
}