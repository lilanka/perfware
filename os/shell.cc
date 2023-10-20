int main() {
	char *prog = nullptr;
	char **args = nullptr;

	// Read the input line at a time and parse each name into program name and its arguments
	while (ReadAndParseCommandLine(&prog, &args)) {
		// Create a chile process to run the command
		int child_pid = fork();

		if (child_pid == 0) {
			// Run the program with parent's input and output
			exec(prog, args);
		} else {
			// Wait for the child to complete
			wait(child_pid);
			return 0;
		}
	}
}
