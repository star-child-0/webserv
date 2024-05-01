/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 04:12:07 by kichkiro          #+#    #+#             */
/*   Updated: 2024/05/01 12:24:28 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(std::map<std::string, std::string> &req, std::map<std::string,
    std::string> req_head, std::string path_info) {
    split_path(path_info, this->_params["DOCUMENT_ROOT"],
               this->_params["SCRIPT_NAME"]);
    this->_request_body = "{key:val}";
    this->_params["SCRIPT_FILENAME"] = path_info;
    this->_params["REQUEST_METHOD"] = req["method"];
    this->_params["CONTENT_TYPE"] = req_head["Content-Type"];
    this->_params["CONTENT_LENGTH"] = req_head["Content-Length"];
    this->_params["REQUEST_URI"] = req["uri"];
    this->_params["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_params["REQUEST_SCHEME"] = "http";
    this->_params["HTTPS"] = "off";
    this->_params["GATEWAY_INTERF"] = "CGI/1.1";
    this->_params["SERVER_SOFTWARE"] = "webserv/1.0";

    // corpo della richiesta:

    std::cout << "DEBUG CGI:" << path_info << std::endl;
    std::cout << "DEBUG CGI:" << this->_params["REQUEST_URI"] << std::endl;
    std::cout << "DEBUG CGI:" << this->_params["REQUEST_METHOD"] << std::endl;
    std::cout << "DEBUG CGI:" << this->_params["DOCUMENT_ROOT"] << std::endl;
    std::cout << "DEBUG CGI:" << this->_params["SCRIPT_NAME"] << std::endl;
    this->exec();
}

Cgi::~Cgi(void) {}

std::string Cgi::exec(void) {
    char *const argv[] = {NULL};
    char *envp[this->_params.size() + 1];
    int i = 0;
    char buffer[4096];
    std::string cgi_out;
    ssize_t bytesRead;
    int status;

    for (std::map<std::string, std::string>::iterator iter = _params.begin();
         iter != _params.end(); ++iter) {
        std::string env_str = iter->first + "=" + iter->second;
        envp[i] = strdup(env_str.c_str());
        i++;
    }
    envp[i] = NULL;

    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        Log::error("CGI: Error creating pipe");
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        Log::error("CGI: Error forking");
        return "";
    }
    else if (!pid) {
        close(pipe_fds[0]);
        // if (this->_params["REQUEST_METHOD"] == "POST") {
        //     if (write(pipe_fds[1], this->_request_body.c_str(), 
        //         this->_request_body.length()) == -1) {
        //         Log::error("CGI: Error writing request body to pipe");
        //         return "";
        //     }
        // }
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1 || dup2(pipe_fds[1],
                                                           STDERR_FILENO) == -1) {
            Log::error("CGI: Error duplicating pipe file descriptor");
            return "";
        }
        close(pipe_fds[1]);
        if (execve(this->_params["SCRIPT_FILENAME"].c_str(), argv, envp) == -1)
            exit(EXIT_FAILURE);
    }
    else {
        close(pipe_fds[1]);
        while ((bytesRead = read(pipe_fds[0], buffer, sizeof(buffer))) > 0) {
            cgi_out.append(buffer, bytesRead);
        }
        if (bytesRead == -1) {
            Log::error("CGI: Error reading from pipe");
            return "";
        }
        close(pipe_fds[0]);
        waitpid(pid, &status, 0);
        std::cout << "---BEGIN RESULT---" << std::endl << cgi_out << "---END RESULT---" << std::endl;
        return cgi_out;
    }
    return "";
}
