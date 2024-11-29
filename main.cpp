#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Built-in command function declarations
int byte_cd(const vector<string>& args);
int byte_help(const vector<string>& args);
int byte_exit(const vector<string>& args);
int byte_launch(const vector<string>& args);

// List of built-in commands
vector<string> builtin_str = {
    "cd",
    "help",
    "exit"
};

vector<int (*)(const vector<string>&)> builtin_func = {
    byte_cd,
    byte_help,
    byte_exit
};

// Get the number of built-in commands
int byte_num_builtins() {
    return builtin_str.size();
}

// Implementation of built-in commands
int byte_cd(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "byte: expected argument to \"cd\"\n";
    } else {
        if (!SetCurrentDirectoryA(args[1].c_str())) {
            perror("byte");
        }
    }
    return 1;
}

int byte_help(const vector<string>& args) {
    cout << "Simple Shell\n";
    cout << "Type program names and arguments, and hit enter.\n";
    cout << "The following are built-in commands:\n";

    for (const auto& cmd : builtin_str) {
        cout << "  " << cmd << "\n";
    }

    cout << "Use the help command for information on other programs.\n";
    return 1;
}

int byte_exit(const vector<string>& args) {
    return 0;
}

int byte_launch(const vector<string>& args) {
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(STARTUPINFOA);

    // Construct the command string
    string command;
    for (const auto& arg : args) {
        command += arg + " ";
    }

    if (!CreateProcessA(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        cerr << "Failed to create process: " << command << "\n";
        return 1;
    }

    // Wait for the process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 1;
}

int byte_execute(const vector<string>& args) {
    if (args.empty()) {
        return 1;
    }

    for (size_t i = 0; i < builtin_str.size(); i++) {
        if (args[0] == builtin_str[i]) {
            return (*builtin_func[i])(args);
        }
    }

    return byte_launch(args);
}

string byte_read_line() {
    string line;
    getline(cin, line);
    return line;
}

vector<string> byte_split_line(const string& line) {
    vector<string> tokens;
    istringstream stream(line);
    string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

void byte_loop() {
    while (true) {
        cout << "> ";
        string line = byte_read_line();
        vector<string> args = byte_split_line(line);

        int status = byte_execute(args);

        if (status == 0) {
            break;
        }
    }
}

int main() {
    byte_loop();
    return EXIT_SUCCESS;
}
