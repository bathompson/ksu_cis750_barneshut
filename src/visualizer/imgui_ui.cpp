#include "imgui_ui.h"

#define IMGUI_IMPLEMENTATION
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imfilebrowser.h>

ImguiUI::ImguiUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    d_fileDialog = std::make_shared<ImGui::FileBrowser>();
    d_fileDialog->SetTitle("Select a File");
    d_fileDialog->SetTypeFilters({".nbody"});
}

ImguiUI::~ImguiUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImguiUI::layout() {
    ImGui::Begin("Simulation Controls", nullptr, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                d_fileDialog->Open();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::SliderInt("Time", &selectedFrame, 0, maxFrame);
    ImGui::End();

    d_fileDialog->Display();
}

void ImguiUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    layout();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImguiUI::hasNewFile() const {
    return d_fileDialog->HasSelected();
}

std::string ImguiUI::popFile() {
    std::string selected = d_fileDialog->GetSelected().string();
    d_fileDialog->ClearSelected();
    return selected;
}

void ImguiUI::setMaxFrame(int frame) {
    maxFrame = frame;
    selectedFrame = 0;
}
