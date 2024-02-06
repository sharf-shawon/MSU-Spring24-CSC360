// CSC360 - Project #2: (guish) "Gnu" Shell
// Author   : Mohammed Sharfuddin Shawon
// Date     : 05/02/2024

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cerrno>
#include <limits.h>
#include <regex>

using namespace std;

void showLogo(bool showHelpText = true)
{
    cout << endl;
    cout << "                 _                                    " << endl;
    cout << "                | |                                   " << endl;
    cout << " _ __ ___   ___ | |__    __ _ __      __  ___   _ __  " << endl;
    cout << "| '_ ` _ \\ / __|| '_ \\  / _` |\\ \\ /\\ / / / _ \\ | '_ \\ " << endl;
    cout << "| | | | | |\\__ \\| | | || (_| | \\ V  V / | (_) || | | |" << endl;
    cout << "|_| |_| |_||___/|_| |_| \\__,_|  \\_/\\_/   \\___/ |_| |_|" << endl;
    cout << "                         By Mohammed Sharfuddin Shawon" << endl;
    cout << endl;
    if (showHelpText)
        cout << "Enter \"help\" to learn more." << endl;
}

void showHelp()
{
    cout << "Command-Line Shell - User Guide\n\n";
    cout << "Welcome to the command-line shell (mshawon) - a simple interactive interface. Below is a guide on how to use the available features:\n\n";
    cout << "1. Basic Commands:\n";
    cout << "   - Enter commands at the prompt to execute them.\n";
    cout << "   - Example: `ls -l`, `echo Hello`, etc.\n\n";
    cout << "2. View Command History:\n";
    cout << "   - Type `hist` and press Enter to view the command history.\n";
    cout << "   - The history includes the last 10 entered commands.\n\n";
    cout << "3. Run Command from History:\n";
    cout << "   - Use the syntax `r n` to run the nth command from history.\n";
    cout << "   - Example: `r 3` will execute the third command from history.\n\n";
    cout << "4. Error Handling:\n";
    cout << "   - If a program terminates with a non-zero exit code, it will be displayed.\n";
    cout << "   - Example:\n";
    cout << "     ```\n";
    cout << "     [ Program returned exit code 1 ]\n";
    cout << "     ```\n\n";
    cout << "5. Built-in Commands:\n";
    cout << "   - `exit`: Terminate the shell.\n";
    cout << "   - `help`: Display this user guide.\n\n";
}

void parse_args(string line, vector<string> &cmds)
{
    stringstream liness(line);

    string token;
    while (getline(liness, token, ' '))
    {
        cmds.push_back(token);
    }
}
string formatCWD()
{
    string home = getenv("HOME");
    char full_path[PATH_MAX];
    if (getcwd(full_path, sizeof(full_path)) == NULL)
    {
        perror("getcwd() error");
        return "getcwd() error";
    }
    return regex_replace(full_path, std::regex(home), "~");
}
int main(void)
{
    const int historySize = 10;
    deque<string> commandHistory;
    showLogo();
    while (1)
    {
        // prompt
        cout << "msh:" << formatCWD() << "$ ";

        string cmd;
        getline(cin, cmd);

        // ignore empty input
        if (cmd == "" || cmd.size() == 0)
            continue;
        else if (cmd == "help")
        {
            showLogo(false);
            showHelp();
            continue;
        }
        else if (cmd == "exit")
        {
            exit(0);
        }
        else if (cmd == "hist")
        {
            cout << "Command history:\n";
            int i = commandHistory.size();
            for (const string &historyCmd : commandHistory)
            {
                cout << i-- << ".\t" << historyCmd << endl;
            }
            continue; // Skip the rest of the loop and prompt again
        }
        // Check for the "r n" command
        else if (cmd.length() > 1 && cmd[0] == 'r' && isdigit(cmd[2]))
        {
            int n = stoi(cmd.substr(1));
            if (n > 0 && n <= commandHistory.size())
            {
                // Get the nth command from history
                //
                string lastNthCommand = commandHistory[commandHistory.size() - n];
                cout << "Running Command from History: " << lastNthCommand << endl;
                vector<string> cmd_args;
                parse_args(lastNthCommand, cmd_args);

                // fork child and execute the nth command
                int pid = fork();
                int status;

                if (pid == 0)
                {
                    // Convert vector<string> to char*
                    vector<char *> argv(cmd_args.size() + 1);
                    for (size_t i = 0; i < cmd_args.size(); ++i)
                    {
                        argv[i] = const_cast<char *>(cmd_args[i].c_str());
                    }
                    argv[cmd_args.size()] = NULL;

                    // Execute the nth command from history
                    execvp(argv[0], argv.data());

                    // If execvp fails, print an error message
                    cerr << "Error executing command: " << strerror(errno) << endl;
                    exit(EXIT_FAILURE);
                }
                else
                {
                    // wait for program to finish and print exit status
                    waitpid(pid, &status, 0);

                    // Print exit code if non-zero
                    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
                    {
                        cout << "The program existed with error: " << WEXITSTATUS(status) << endl;
                        exit(0); // Exiting out of the child process. This solves "multiple exit command needed to exit the shell" issue
                    }
                }
                continue; // Skip the rest of the loop and prompt again
            }
            else
            {
                cerr << "Invalid history command: " << cmd << endl;
                continue; // Skip the rest of the loop and prompt again
            }
        }

        // Since its not a custom command, add it to history
        commandHistory.push_back(cmd);

        // If the history exceeds the limit, remove the oldest command
        if (commandHistory.size() > historySize)
        {
            commandHistory.pop_front();
        }

        vector<string> cmd_args;
        parse_args(cmd, cmd_args);

        // fork child and execute program
        int pid = fork();
        int status;

        if (pid == 0)
        {
            // Convert vector<string> to char*
            vector<char *> argv(cmd_args.size() + 1);
            for (size_t i = 0; i < cmd_args.size(); ++i)
            {
                argv[i] = const_cast<char *>(cmd_args[i].c_str());
            }
            argv[cmd_args.size()] = NULL;

            // Execute the command entered by the user
            execvp(argv[0], argv.data());

            // If execvp fails, print an error message
            cerr << "The program seems missing. Error code is: " << errno << endl;
            exit(0); // Exiting out of the child process. This solves "multiple exit command needed to exit the shell" issue
        }
        else
        {
            // wait for program to finish and print exit status
            waitpid(pid, &status, 0);

            // Print exit code if non-zero
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                cout << "The program existed with error: " << WEXITSTATUS(status) << endl;
            }
        }
    }
}