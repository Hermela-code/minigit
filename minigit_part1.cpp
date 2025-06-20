#include "minigit.hpp"
#include <iomanip>
#include <algorithm>

using namespace std;

// FNV-1a 64-bit hash
string MiniGit::computeHash(const string& content) {
    const uint64_t fnv_prime = 1099511628211ULL;
    uint64_t hash = 14695981039346656037ULL;
    
    for (char c : content) {
        hash ^= static_cast<uint64_t>(c);
        hash *= fnv_prime;
    }
    
    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}

MiniGit::MiniGit() : gitDir(".minigit") {
    objectsDir = gitDir + "/objects";
    commitsDir = gitDir + "/commits";
    refsDir = gitDir + "/refs";
    headsDir = refsDir + "/heads";
}

MiniGit::~MiniGit() {}

void MiniGit::init() {
    if (fs::exists(gitDir)) {
        cerr << "MiniGit already initialized" << endl;
        return;
    }

    fs::create_directory(gitDir);
    fs::create_directory(objectsDir);
    fs::create_directory(commitsDir);
    fs::create_directory(refsDir);
    fs::create_directory(headsDir);

    ofstream(gitDir + "/index").close();

    Commit* initialCommit = new Commit();
    initialCommit->message = "Initial commit";
    time_t now = time(nullptr);
    initialCommit->timestamp = ctime(&now);
    initialCommit->timestamp.pop_back();
    
    stringstream commitData;
    commitData << initialCommit->message << initialCommit->timestamp;
    initialCommit->hash = computeHash(commitData.str());
    
    writeCommit(initialCommit);
    
    currentBranch = "main";
    branches[currentBranch] = initialCommit->hash;
    updateBranch(currentBranch);
    
    ofstream headFile(gitDir + "/HEAD");
    headFile << "ref: refs/heads/main" << endl;
    headFile.close();
    
    cout << "Initialized MiniGit repository with 'main' branch" << endl;
    delete initialCommit;
}

void MiniGit::add(const string& filename) {
    if (!fs::exists(filename)) {
        cerr << "File not found: " << filename << endl;
        return;
    }
    
    ifstream file(filename);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    string blobHash = computeHash(content);
    stagingArea[filename] = blobHash;
    persistStagingArea();
    
    writeBlob(blobHash, content);
    
    cout << "Added " << filename << " to staging area" << endl;
}

void MiniGit::commit(const string& message) {
    loadStagingArea();
    
    if (stagingArea.empty()) {
        cerr << "No changes staged for commit" << endl;
        return;
    }
    
    Commit* commit = new Commit();
    commit->message = message;
    time_t now = time(nullptr);
    commit->timestamp = ctime(&now);
    commit->timestamp.pop_back();
    
    string currentCommitHash = getCurrentCommitHash();
    if (!currentCommitHash.empty()) {
        commit->parentHashes.push_back(currentCommitHash);
        Commit* parent = readCommit(currentCommitHash);
        commit->fileMap = parent->fileMap;
        delete parent;
    }
    
    for (const auto& entry : stagingArea) {
        commit->fileMap[entry.first] = entry.second;
    }
    
    stringstream commitData;
    commitData << message << commit->timestamp;
    for (const auto& parent : commit->parentHashes) commitData << parent;
    for (const auto& entry : commit->fileMap) commitData << entry.first << entry.second;
    commit->hash = computeHash(commitData.str());
    
    writeCommit(commit);
    branches[currentBranch] = commit->hash;
    updateBranch(currentBranch);
    
    stagingArea.clear();
    persistStagingArea();
    
    cout << "[" << currentBranch << " " << commit->hash.substr(0, 7) << "] " << message << endl;
    delete commit;
}

void MiniGit::log() {
    string current = getCurrentCommitHash();
    if (current.empty()) {
        cout << "No commits yet" << endl;
        return;
    }
    
    while (!current.empty()) {
        Commit* commit = readCommit(current);
        if (!commit) break;
        
        cout << "commit " << commit->hash << endl;
        cout << "Author: MiniGit User <user@example.com>" << endl;
        cout << "Date:   " << commit->timestamp << endl;
        cout << "\n    " << commit->message << "\n" << endl;
        
        if (!commit->parentHashes.empty()) {
            current = commit->parentHashes[0];
        } else {
            current = "";
        }
        delete commit;
    }
}

void MiniGit::status() {
    cout << "On branch " << (currentBranch.empty() ? "DETACHED HEAD" : currentBranch) << endl;
    
    string currentCommitHash = getCurrentCommitHash();
    if (currentCommitHash.empty()) {
        cout << "No commits yet" << endl;
        return;
    }
    
    Commit* currentCommit = readCommit(currentCommitHash);
    if (!currentCommit) return;
    
    cout << "\nStaged changes:" << endl;
    if (stagingArea.empty()) {
        cout << "  (no files staged)" << endl;
    } else {
        for (const auto& entry : stagingArea) {
            cout << "  " << getFileStatus(entry.first) << " " << entry.first << endl;
        }
    }
    
    cout << "\nBranches:" << endl;
    for (const auto& branch : branches) {
        cout << (branch.first == currentBranch ? "* " : "  ") << branch.first << endl;
    }
    
    delete currentCommit;
}

string MiniGit::getFileStatus(const string& filename) {
    string currentCommitHash = getCurrentCommitHash();
    if (currentCommitHash.empty()) return "A";
    
    Commit* currentCommit = readCommit(currentCommitHash);
    if (!currentCommit) return "?";
    
    string status = "?";
    if (currentCommit->fileMap.count(filename)) {
        if (stagingArea.count(filename)) {
            status = (currentCommit->fileMap.at(filename) != stagingArea.at(filename)) ? "M" : " ";
        } else {
            status = "D"; // Deleted from staging but still in commit
        }
    } else if (stagingArea.count(filename)) {
        status = "A"; // Added
    }
    
    delete currentCommit;
    return status;
}
