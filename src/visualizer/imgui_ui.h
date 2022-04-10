#pragma once

#include <string>
#include <memory>

class GLFWwindow;
namespace ImGui {
    class FileBrowser;
}

class ImguiUI {
public:
    int selectedFrame = 0;

public:
    ImguiUI(GLFWwindow* window);
    ~ImguiUI();

    void render();
    bool hasNewFile() const;
    std::string popFile();

    void setMaxFrame(int frame);

private:
    void layout();

    std::shared_ptr<ImGui::FileBrowser> d_fileDialog;
    int maxFrame = 10;
};
