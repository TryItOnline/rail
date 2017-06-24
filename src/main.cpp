// main.cpp

#include "lib.h"
#include "Thread.h"
#include "Board.h"
#include "interactive.h"

using namespace std;

void parseAllArguments(int argc, char * argv[]);
void parseArgument(string const & arg);
bool hasPrefix(string const & subject, string const & prefix);
void parseInput(map<string, Board> & globals);
void parseFile(ifstream & file, map<string, Board> & globals);
void parseField(ifstream & file, list<string> & field, string & line);
string parseHeader(string const & line);

int main(int argc, char * argv[])
{
  int result = 0;
  try
  {
    parseAllArguments(argc, argv);
    map<string, Board> globals;
    parseInput(globals);

    Thread program;
    map<string, Board>::iterator mainPos = globals.find("main");
    if (mainPos != globals.end())
    {
      program.reset(mainPos->second, &globals);
    }
    else
    {
      throw InternalException("Crash: No 'main' function found");
    }
    if (args::isBatch)
    {
      program.go();
    }
    else
    {
      runInteractive(program);
    }
  }
  catch(ArgumentException & error)
  {
    cerr << "Argument Error: " << error.what() << endl;
    cerr << "Usage: " << argv[0] << " [--batch] [--interactive] " << endl
         << "        [--input=<file>] [--output=<file>] <sourcefile> " << endl
         << "        [<sourcefile> [...]]" << endl << endl;

    cerr << "--batch           Runs in batch mode. The interpreted program will run" << endl
         << "                  silently to completion." << endl;

    cerr << "--interactive     Runs in interactive mode. A step by step readout of the" << endl
         << "                  interpreted program is displayed with debugging controls." << endl
         << "                  If the run is interactive, then input and output must both"
         << "                  be bound to files." << endl;

    cerr << "--input=<file>    Sets standard input of the interpreted program to <file>." << endl
         << "                  Default is the outer program's standard input." << endl;

    cerr << "--ouput=<file>    Sets standard output of the interpreted program to <file>" << endl
         << "                  Default is the outer program's standard output." << endl;

    result = 2;
  }
  catch(std::exception & error)
  {
    cerr << "Internal Error: " << error.what() << endl;
    result = 1;
  }
  return result;
}

void parseAllArguments(int argc, char * argv[])
{
  for (int i = 1; i < argc; ++i)
  {
    parseArgument(argv[i]);
  }
  if (!(args::isBatch) && (args::output == NULL || args::input == NULL))
  {
    throw ArgumentException("If the run is interactive, then input and output must both be bound to files.");
  }
  else if (args::sourceFiles.empty())
  {
    throw ArgumentException("No source files to interpret.");
  }

  if (args::output == NULL)
  {
    args::output = &cout;
  }
  if (args::input == NULL)
  {
    args::input = &cin;
  }
}

void parseArgument(string const & arg)
{
  static const string batchFlag = "--batch";
  static const string interactiveFlag = "--interactive";
  static const string inputPrefix = "--input=";
  static const string outputPrefix = "--output=";

  static ifstream input;
  static ofstream output;
  if (arg == batchFlag)
  {
    args::isBatch = true;
  }
  else if (arg == interactiveFlag)
  {
    args::isBatch = false;
  }
  else if (hasPrefix(arg, inputPrefix))
  {
    if (args::input == NULL)
    {
      input.open(arg.substr(inputPrefix.size()).c_str(), ios::in);
      if (!input)
      {
        throw ArgumentException("Failed to open '--input' file: "
                                + arg.substr(inputPrefix.size()));
      }
      args::input = &input;
    }
    else
    {
      throw ArgumentException("Only one '--input' file is allowed: " + arg);
    }
  }
  else if (hasPrefix(arg, outputPrefix))
  {
    if (args::output == NULL)
    {
      output.open(arg.substr(outputPrefix.size()).c_str(),
                  ios::out | ios::trunc);
      if (!output)
      {
        throw ArgumentException("Failed to open '--output' file: "
                                + arg.substr(outputPrefix.size()));
      }
      args::output = &output;
    }
    else
    {
      throw ArgumentException("Only one '--output' file is allowed: " + arg);
    }
  }
  else
  {
    // This must be a source filename.
    args::sourceFiles.push_back(arg);
  }
}

bool hasPrefix(string const & subject, string const & prefix)
{
  return subject.compare(0, prefix.size(), prefix) == 0;
}

void parseInput(map<string, Board> & globals)
{
  list<string>::iterator pos = args::sourceFiles.begin();
  list<string>::iterator limit = args::sourceFiles.end();
  for (; pos != limit; ++pos)
  {
    ifstream source(pos->c_str(), ios::in);
    if (!source)
    {
      throw ArgumentException("Failed to open source file: " + *pos);
    }
    parseFile(source, globals);
  }
}

void parseFile(ifstream & file, map<string, Board> & globals)
{
  list<string> field;
  string line;
  string name;
  parseField(file, field, line);
  while (file)
  {
    field.clear();
    field.push_back(line);
    name = parseHeader(line);
    parseField(file, field, line);
    if (name != "")
    {
      globals[name].reset(name, field);
    }
  }
}

void parseField(ifstream & file, list<string> & field, string & line)
{
  getline(file, line);
  while (file && line[0] != '$')
  {
    field.push_back(line);
    getline(file, line);
  }
}

string parseHeader(string const & line)
{
  string result;
  size_t first = line.find('\'');
  if (first != string::npos)
  {
    size_t second = line.find('\'', first + 1);
    if (second != string::npos)
    {
      result = line.substr(first + 1, second - (first + 1));
    }
  }
  return result;
}
