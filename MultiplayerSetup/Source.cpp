#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

void copyFiles(const std::vector<std::string>& filesToCopy, const fs::path& targetDir) {
    for (const auto& fileName : filesToCopy) {
        fs::copy(fileName, targetDir / fileName, fs::copy_options::overwrite_existing);
    }
}

void modifyFile(const fs::path& filePath) {
    std::fstream file(filePath, std::ios::in | std::ios::out);
    if (file.is_open()) {
        std::string line;
        std::vector<std::string> lines;
        
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        if (!lines.empty() && lines[0].find("[ Layers :") != std::string::npos) {
            size_t pos = lines[0].find(":") + 1;
            int currentNumber = std::stoi(lines[0].substr(pos));
            lines[0] = "[ Layers : " + std::to_string(currentNumber + 1) + " ]";
        }

        file.clear();
        file.seekp(0, std::ios::beg);

        for (const auto& l : lines) {
            file << l << "\n";
        }
        
        file << "\n\"FAKE_HOBBITS\"";
    }
    file.close();
}

int main() {
    const std::string levelsFolder = "../Levels";
    const std::vector<std::string> filesToCopy = {"FAKE_HOBBITS.EXPORT", "BILBOFAKE.NPCGEOM", "BILBC[D].XBMP"};
    const std::string bilboAnimFile = "BILBORMANIMS.CHARANIM";

    for (const auto& entry : fs::directory_iterator(levelsFolder)) {
        if (fs::is_directory(entry)) {
            const auto& currentDir = entry.path();
            copyFiles(filesToCopy, currentDir);

            modifyFile(currentDir / "ALLUSEDLAYERS.TXT");
            modifyFile(currentDir / "INITIALOBJECTLAYERS.TXT");
        }
    }

    const fs::path commonCharactersDir = fs::path("..") / "Common" / "CHARACTERS";
    fs::copy(bilboAnimFile, commonCharactersDir / fs::path(bilboAnimFile).filename(), fs::copy_options::overwrite_existing);

    std::cout << "Program has finished execution. Press Enter to exit.";
    std::cin.get();

    return 0;
}