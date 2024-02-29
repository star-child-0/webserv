#include "Error.h"

/*!
* @brief Check if an error occurred
* @details This function checks if an error occurred, and prints the error message if it did.
* @param ret The result of the function that was called
* @param exit The error code to return if an error occurred
* @return The error code if an error occurred, or NO_ERROR if no error occurred
*/
ErrorCodes Error::checkError(int ret, ErrorCodes exit)
{
	std::string error;
	if (ret < 0)
	{
		if (exit == FAILED_ACCEPT)
			error = "Failed to accept connection";
		else
			error = strerror(ret);
		Log::error(error);
		return (exit);
	}
	return (NO_ERROR);
}

/*!
* @brief Exit the program
* @details This function closes the server and returns the exit status of the weberver.
* @param exit The exit status of the weberver
* @param server The server to close
* @return The exit status of the weberver
*/
int Error::exit(ErrorCodes exit, Server &server)
{
	server.close();
	return (exit);
}
