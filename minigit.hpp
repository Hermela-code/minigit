#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <filesystem>
#include <ctime>
#include <unordered_map>
#include <iomanip>

namespace fs = std::filesystem;

class MiniGit {
public:
    MiniGit();
    ~MiniGit();

    void init();
    void add(const std::string& filename);
    void commit(const std::string& message);
    void log();
    void status();  // New method
    void branch(const std::string& branchName);
    void checkout(const std::string& target);
    void merge(const std::string& branchName);
    void loadState();

private:
    struct Commit {
        std::string hash;
        std::string message;
        std::string timestamp;
        std::vector<std::string> parentHashes;
        std::map<std::string, std::string> fileMap;
    };

    std::string currentBranch;
    std::map<std::string, std::string> branches;
    std::map<std::string, std::string> stagingArea;
    bool inMergeState = false;
    std::string mergeTargetBranch;

    std::string gitDir;
    std::string objectsDir;
    std::string commitsDir;
    std::string refsDir;
    std::string headsDir;

    std::string computeHash(const std::string& content);
    void writeBlob(const std::string& hash, const std::string& content);
    std::string readBlob(const std::string& hash);
    void writeCommit(const Commit* commit);
    Commit* readCommit(const std::string& hash);
    std::string getCurrentCommitHash();
    void updateHead();
    void updateBranch(const std::string& branchName);
    void restoreCommit(const std::string& commitHash);
    std::string findLCA(const std::string& commit1, const std::string& commit2);
    void threeWayMerge(
        const std::string& currentHash,
        const std::string& targetHash,
        const std::string& baseHash
    );
    void markConflict(const std::string& filename, 
                     const std::string& currentContent,
                     const std::string& incomingContent);
    void persistStagingArea();
    void loadStagingArea();
    std::string getFileStatus(const std::string& filename);  // Helper for status
};

#endif