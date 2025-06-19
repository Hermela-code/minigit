#include <fstream>
#include <iostream>
using namespace std;

void createBranch(const std::string& branchName) {
    ifstream head(".minigit/HEAD");
    string commitHash;
    getline(head, commitHash);

    ofstream branch(".minigit/" + branchName);
    branch << commitHash;
    cout << "Branch '" << branchName << "' created at " << commitHash << "\n";
}

void checkoutCommit(const std::string& nameOrHash) {
    ifstream branch(".minigit/" + nameOrHash);
    if (!branch) {
        cerr << "Invalid branch or commit: " << nameOrHash << "\n";
        return;
    }

    string commitHash;
    getline(branch, commitHash);
    ifstream commit(".minigit/" + commitHash);
    cout << "Checking out " << commitHash << ":\n";
    string line;
    while (getline(commit, line)) {
        cout << line << "\n";
    }

    ofstream head(".minigit/HEAD");
    head << commitHash;
}
