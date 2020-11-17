import 'dart:convert';
import 'dart:io';

import 'package:glob/glob.dart';
import 'package:path/path.dart' as p;

/// Runs the tests.

final _expectedOutputPattern = RegExp(r"// expect: ?(.*)");
final _expectedErrorPattern = RegExp(r"// (Error.*)");
final _errorLinePattern = RegExp(r"// \[line (\d+)\] (Error.*)");
final _expectedRuntimeErrorPattern = RegExp(r"// expect runtime error: (.+)");
final _syntaxErrorPattern = RegExp(r"\[.*line (\d+)\] (Error.+)");
final _stackTracePattern = RegExp(r"\[line (\d+)\]");

var _passed = 0;
var _failed = 0;
var _skipped = 0;
var _expectations = 0;

Interpreter _interpreter;

class Interpreter {
  final String executable;  
  final Map<String, String> tests;

  Interpreter(this.executable, this.tests);
}

void main(List<String> arguments) {    
  if (arguments.length > 1) {
    print("Usage: test.dart [testFilePath]");
    exit(1);
  } else if (arguments.length == 0) {
    _setUpInterpreter();
    _runTests();
  } else {    
    var tests = {arguments[0]: "pass"};
    _interpreter = Interpreter("cplox", tests);
    _runTest(arguments[0]);
  }      
}

void _setUpInterpreter() {    
  var tests = {
    "test": "pass",
    "test/scanning": "skip", // These were only run when building the scanner.
    "test/string/literals.cplox": "skip", // TODO: fix non-ASCII printing issue.
    "test/string/multiline.cplox": "skip" // TODO: fix conversion from stdout.
  };

  _interpreter = Interpreter("cplox", tests);  
}

bool _runTests() {    
  for (var file in Glob("test/**.cplox").listSync()) {
    _runTest(file.path);
  }

  print("");

  if (_failed == 0) {
    print("All $_passed tests passed "
        "($_expectations expectations).");
  } else {
    var passTestString = _passed == 1 ? "test" : "tests";
    var failTestString = _failed == 1 ? "test" : "tests";
    print("$_passed $passTestString passed. "
        "$_failed $failTestString failed.");
  }

  return _failed == 0;
}

void _runTest(String path) {      
  path = p.windows.normalize(path);            
  print("Running $path");
  var test = Test(path);
  if (!test.parse()) return;
  var failures = test.run();
  
  if (failures.isEmpty) {
    _passed++;
  } else {
    _failed++;
    stdout.write("\nFAIL $path");
    print("");
    for (var failure in failures) {
      print("     $failure");
    }
    print("");
  }
  
  stdout.write("Passed: $_passed | "
      "Failed: $_failed | "
      "Skipped: $_skipped\n\n");
}

class Test {
  final String _path;
    
  final _expectedOutput = <List<Object>>[];

  /// The set of expected compile error messages.
  final _expectedCompileErrors = <String>{};

  /// The expected runtime error message or `null` if there should not be one.
  String _expectedRuntimeError;

  /// If there is an expected runtime error, the line it should occur on.
  int _runtimeErrorLine = 0;

  int _expectedExitCode = 0;

  /// The list of failure message lines.
  final _failures = <String>[];

  Test(this._path);

  bool parse() {    
    var parts = _path.split("\\"); // Use "/" instead for POSIX paths
    var subpath = "";
    String state;
    
    for (var part in parts) {      
      if (subpath.isNotEmpty) subpath += "/";
      subpath += part;            
      if (_interpreter.tests.containsKey(subpath)) {
        state = _interpreter.tests[subpath];
      }
    }

    if (state == null) {
      throw "Unknown test state for '$_path'.";
    } else if (state == "skip") {
      _skipped++;
      return false;
    }

    var lines = File(_path).readAsLinesSync();
    for (var lineNum = 1; lineNum <= lines.length; lineNum++) {
      var line = lines[lineNum - 1];      

      var match = _expectedOutputPattern.firstMatch(line);
      if (match != null) {
        _expectedOutput.add([match[1], lineNum]);
        _expectations++;
        continue;
      }

      match = _expectedErrorPattern.firstMatch(line);
      if (match != null) {
        _expectedCompileErrors.add("[$lineNum] ${match[1]}");

        // If we expect a compile error, it should exit with EX_DATAERR.
        _expectedExitCode = 65;
        _expectations++;
        continue;
      }

      match = _errorLinePattern.firstMatch(line);
      if (match != null) {        
        _expectedCompileErrors.add("[${match[1]}] ${match[2]}");

        // If we expect a compile error, it should exit with EX_DATAERR.
        _expectedExitCode = 65;
        _expectations++;
        continue;
      }

      match = _expectedRuntimeErrorPattern.firstMatch(line);
      if (match != null) {
        _runtimeErrorLine = lineNum;
        _expectedRuntimeError = match[1];
        
        // If we expect a runtime error, it should exit with EX_SOFTWARE.
        _expectedExitCode = 70;
        _expectations++;
      }
    }

    if (_expectedCompileErrors.isNotEmpty && _expectedRuntimeError != null) {
      print("'TEST ERROR' $_path");
      print("     Cannot expect both compile and runtime errors.");
      print("");
      return false;
    }
    
    return true;
  }

  /// Invoke the interpreter and run the test.
  List<String> run() {
    var args = [_path];
    var result = Process.runSync(_interpreter.executable, args);

    // Normalize Windows line endings.
    var outputLines = const LineSplitter().convert(result.stdout as String);
    var errorLines = const LineSplitter().convert(result.stderr as String);
    
    if (_expectedRuntimeError != null) {
      _validateRuntimeError(errorLines);
    } else {
      _validateCompileErrors(errorLines);
    }

    _validateExitCode(result.exitCode, errorLines);
    _validateOutput(outputLines);
    return _failures;
  }

  void _validateRuntimeError(List<String> errorLines) {
    if (errorLines.length < 2) {
      fail("Expected runtime error '$_expectedRuntimeError' and got none.");
      return;
    }

    if (errorLines[0] != _expectedRuntimeError) {
      fail("Expected runtime error '$_expectedRuntimeError' and got:");
      fail(errorLines[0]);
    }

    // Make sure the stack trace has the right line.
    RegExpMatch match;
    var stackLines = errorLines.sublist(1);
    for (var line in stackLines) {
      match = _stackTracePattern.firstMatch(line);
      if (match != null) break;
    }

    if (match == null) {
      fail("Expected stack trace and got:", stackLines);
    } else {
      var stackLine = int.parse(match[1]);
      if (stackLine != _runtimeErrorLine) {
        fail("Expected runtime error on line $_runtimeErrorLine "
            "but was on line $stackLine.");
      }
    }
  }

  void _validateCompileErrors(List<String> error_lines) {    
    var foundErrors = <String>{};
    var unexpectedCount = 0;
    for (var line in error_lines) {
      var match = _syntaxErrorPattern.firstMatch(line);
      if (match != null) {
        var error = "[${match[1]}] ${match[2]}";
        if (_expectedCompileErrors.contains(error)) {
          foundErrors.add(error);
        } else {
          if (unexpectedCount < 10) {
            fail("Unexpected error:");
            fail(line);
          }
          unexpectedCount++;
        }
      } else if (line != "") {
        if (unexpectedCount < 10) {
          fail("Unexpected output on stderr:");
          fail(line);
        }
        unexpectedCount++;
      }
    }

    if (unexpectedCount > 10) {
      fail("(truncated ${unexpectedCount - 10} more...)");
    }
    
    for (var error in _expectedCompileErrors.difference(foundErrors)) {
      fail("Missing expected error: $error");
    }
  }

  void _validateExitCode(int exitCode, List<String> errorLines) {
    if (exitCode == _expectedExitCode) return;

    if (errorLines.length > 10) {
      errorLines = errorLines.sublist(0, 10);
      errorLines.add("(truncated...)");
    }

    fail("Expected return code $_expectedExitCode and got $exitCode. Stderr:",
        errorLines);
  }

  void _validateOutput(List<String> outputLines) {    
    if (outputLines.isNotEmpty && outputLines.last == "") {
      outputLines.removeLast();
    }

    var index = 0;
    for (; index < outputLines.length; index++) {
      var line = outputLines[index];
      var output = _expectedOutput[index][0] as String;
      if (index >= _expectedOutput.length) {
        fail("Got output '$line' when none was expected.");
      } else if (output != line) {
        var lineNum = _expectedOutput[index][1] as int;
        fail("Expected output '$output' on line $lineNum and got '$line'.");
      }
    }

    while (index < _expectedOutput.length) {
      fail("Missing expected output '${_expectedOutput[index][0]}' on line "
          "${_expectedOutput[index][1]}.");
      index++;
    }
  }

  void fail(String message, [List<String> lines]) {
    _failures.add(message);
    if (lines != null) _failures.addAll(lines);
  }
}
