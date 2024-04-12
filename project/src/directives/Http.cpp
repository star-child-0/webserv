/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcavanna <gcavanna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 16:47:13 by kichkiro          #+#    #+#             */
/*   Updated: 2024/04/09 16:08:07 by gcavanna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Directive.hpp"

Http::Http(string context) {
	vector<Directive *> value;

	if (context != "main")
		throw WrongContextExc("Http", "main", context);
	this->_type = "http";
	this->_is_context = true;
	this->_value_block = value;
}

Http::Http(ifstream &raw_value, string context) {
	if (context != "main")
		throw WrongContextExc("Http", "main", context);
	this->_type = "http";
	this->_is_context = true;
	this->_parsing_block(raw_value);
}

Http::~Http(void) {
	for (size_t i = 0; i < this->_sockets.size(); ++i)
		delete this->_sockets[i];
}

vector<uint16_t> Http::_extract_listen_ports(void) {
	vector<uint16_t> ports;
	size_t num_servers = this->_value_block.size();
	size_t num_directives;
	size_t num_ports;
	uint16_t port;

	for (size_t i = 0; i < num_servers; i++) {
		num_directives = this->_value_block[i]->get_block_size();
		for (size_t j = 0; j < num_directives; j++) {
			if (this->_value_block[i]->get_value_block()[j]->get_type()
				== "listen") {
				num_ports = this->_value_block[i]->get_value_block()[j]
					->get_inline_size();
				for (size_t k = 0; k < num_ports; k++) {
					string	tmpPort = this->_value_block[i]
						->get_value_block()[j]->get_value_inline()[k];
					if (tmpPort != "default_server") {
						if (tmpPort.find(':') != string::npos) {
							tmpPort = tmpPort.substr(
								tmpPort.find(':') + 1, tmpPort.length() - 1);
						}
						port = static_cast<uint16_t>(atoi(tmpPort.c_str()));
						if (!uint16_t_in_vec(ports, port))
							ports.push_back(port);
					}
				}
			}
		}
	}
	return ports;
}

// DA SISTEMARE
string Http::_read_requests(Socket *client_socket) {
	char buf[4096]; // Buffer più grande per gestire la maggior parte delle richieste in un solo ciclo
	string request = "";
	ssize_t bytes_read;
	const string request_end = "\r\n\r\n";
	size_t found_end;

	while (true) {
		bytes_read = recv(client_socket->get_socket(), buf, sizeof(buf) - 1, 0); //La funzione recv può essere utilizzata anche per ricevere dati in modo non bloccante, utilizzando la flag MSG_DONTWAIT. Inoltre, la funzione recv può essere utilizzata per ricevere dati da più socket contemporaneamente, utilizzando la funzione select.

		if (bytes_read > 0) {
			buf[bytes_read] = '\0'; // Assicurati che la stringa sia terminata correttamente
			request.append(buf);

			// Controlla se hai ricevuto l'intero header della richiesta
			found_end = request.find(request_end);
			if (found_end != string::npos) {
				// Se esiste un "Content-Length", devi leggere anche il corpo della richiesta
				size_t content_length_pos = request.find("Content-Length: ");
				if (content_length_pos != string::npos) {
					content_length_pos += strlen("Content-Length: ");
					size_t content_length_end = request.find("\r\n", content_length_pos);
					if (content_length_end != string::npos) {
						Log::error("Richiesta malformata: impossibile trovare la fine dell'header Content-Lenght");
						return "";
					}
					int content_length = atoi(request.substr(content_length_pos, content_length_end - content_length_pos).c_str());
					size_t headers_length = found_end + 4; // +4 per la sequenza \r\n\r\n

					// Leggi il corpo della richiesta se non è stato già completamente ricevuto
					while (request.size() < headers_length + content_length) {
						bytes_read = recv(client_socket->get_socket(), buf, sizeof(buf) - 1, 0);
						if (bytes_read <= 0) break; // Gestisci errori o chiusura connessione
						buf[bytes_read] = '\0';
						request.append(buf);
					}
				}
				break; // Hai trovato la fine dell'header e letto il corpo se necessario
			}
		}
		else if (bytes_read == 0) {
			// La connessione è stata chiusa dal client
			Log::info("Connection closed by remote host");
			return "";
		}
		else {
			// Gestisci errori di lettura
			Log::error("Error reading from socket");
			return "";
		}
	}
	if (found_end == string::npos) {
		Log::error("Richiesta malformata: impossibile trovare la fine dell'header Content-Lenght");
		return "";
	}
	// cout << request << endl;
	return request;
}

void Http::_parse_request(const string &request) {
	istringstream requestStream(request);
	string line;
	getline(requestStream, line);
	istringstream firstLineStream(line);

	// Extract the request method, URI and HTTP version
	firstLineStream >> this->_request["method"] >> this->_request["uri"] >> this->_request["httpVersion"];
	this->_requestMethod = _methodToEnum(this->_request["method"]);

	// Extract the request headers
	while (getline(requestStream, line) && !line.empty()) {
		string::size_type colonPos = line.find(": ");
		if (colonPos != string::npos) {
			string headerName = line.substr(0, colonPos);
			string headerValue = line.substr(colonPos + 2);
			this->_requestHeaders[headerName] = headerValue;
			// cout << headerName << " -> " << headerValue << endl;
		}
	}

	// Extract the request body specified by the Content-Length header
	map<string, string>::iterator it = this->_requestHeaders.find("Content-Length");
	if (it != this->_requestHeaders.end()) {
		int contentLength = atoi(it->second.c_str());
		this->_requestBody.reserve(contentLength);
		while (contentLength > 0 && getline(requestStream, line)) {
			this->_requestBody.append(line + "\n");
			contentLength -= line.length() + 1; // +1 per il carattere di nuova linea che getline consuma
		}
	}
}

vector<Directive*>	Http::_matchingServersServerName(const vector<Directive*>& servers, const string requestIP)
{
	vector<Directive *>	matchingServers;

	for (vector<Directive *>::const_iterator itServer = servers.begin(); itServer != servers.end(); ++itServer)
	{
		vector<Directive *>	serverBlock = (*itServer)->get_value_block();

		for (size_t i = 0; i < serverBlock.size(); ++i)
		{
			if (serverBlock[i]->get_type() == "server_name")
			{
				// cout << "server_name at block " << i << " size : " << serverBlock[i]->get_inline_size() << endl;
				for (size_t j = 0; j < serverBlock[i]->get_inline_size(); ++j)
				{
					// cout << "server_name : " << serverBlock[i]->get_value_inline()[j] << endl;
					if (serverBlock[i]->get_value_inline()[j] == requestIP)
					{
						// Log::debug("server_name match found");
						matchingServers.push_back(*itServer);
						return matchingServers;
					}
				}
			}
		}
	}
	return matchingServers;
}

vector<Directive*>	Http::_matchingServersIP(const vector<Directive*>& servers, const string requestIP)
{
	vector<Directive *>	matchingServers;
	bool	isMatch = false;

	for (vector<Directive *>::const_iterator itServer = servers.begin(); itServer != servers.end(); ++itServer)
	{
		vector<Directive *>	listenValueBlock = (*itServer)->get_value_block();

		for (size_t l = 0; listenValueBlock[l]->get_type() == "listen"; ++l)
		{
			for (size_t p = 0; p < listenValueBlock[l]->get_inline_size(); ++p)
			{
				string	serverHost = listenValueBlock[l]->get_value_inline()[p];
				string	serverIP = "";

				if (serverHost.find(":") != string::npos)
					serverIP = serverHost.substr(0, serverHost.find(":"));

				// Log::debug(serverHost);
				// Log::debug(serverIP);

				if (serverIP == requestIP) {
					// cout << "server_name " << i << " matched : " << serverIP << endl;
					isMatch = true;
				}
			}
		}
		if (isMatch)
		{
			matchingServers.push_back(*itServer);
			isMatch = false;
		}
	}
	return matchingServers;
}

vector<Directive*>	Http::_matchingServersPort(const vector<Directive*>& servers, uint16_t requestPort)
{
	vector<Directive *>	matchingServers;
	bool				isMatch = false;

	for (vector<Directive *>::const_iterator itServer = servers.begin(); itServer != servers.end(); ++itServer) {
		vector<Directive *>	listenValueBlock = (*itServer)->get_value_block();

		for (size_t l = 0; listenValueBlock[l]->get_type() == "listen"; ++l)
		{
			for (size_t p = 0; p < listenValueBlock[l]->get_inline_size(); ++p)
			{
				string		tmpPort = listenValueBlock[l]->get_value_inline()[p];
				uint16_t	serverPort;

				if (tmpPort.find(':') != string::npos)
					tmpPort = tmpPort.substr(tmpPort.find(":") + 1).c_str();
				serverPort = static_cast<uint16_t>(atoi(tmpPort.c_str()));

				// Log::debug(tmpPort);

				if (requestPort == serverPort)
				{
					// cout << "listen " << l << " : port " << p << " matched : " << serverPort << endl;
					// Log::debug(stream.str());
					isMatch = true;
				}
			}
		}
		if (isMatch)
			matchingServers.push_back(*itServer);
		isMatch = false;
	}
	return matchingServers;
}

vector<Directive*> Http::_find_virtual_server(void) {
	string				requestHost = this->_requestHeaders["Host"];
	string				requestIP = requestHost.substr(0, requestHost.find(":")).c_str();
	uint16_t			requestPort = static_cast<uint16_t>(atoi(requestHost.substr(requestHost.find(":") + 1).c_str()));
	vector<Directive *>	serverValueBlock = this->get_value_block();
	vector<Directive *>	matchingServers, ipServers, snServers;
	stringstream		stream;

	matchingServers = this->_matchingServersPort(serverValueBlock, requestPort);
	if (matchingServers.size() == 0)
	{
		Log::error("Port match not found");
	}
	else if (matchingServers.size() == 1)
	{
		// Log::debug("Port match found");
	}
	else //if (matchingServers.size() > 1)
	{
		matchingServers = this->_matchingServersIP(matchingServers, requestIP);
		if (matchingServers.size() == 0)
		{
			Log::error("IP match not found");
		}
		else if (matchingServers.size() == 1)
		{
			// Log::debug("IP match found");
		}
		//matchingServers = this->_matchingServersServerName(matchingServers, requestIP);
	}

	return matchingServers;
}

/*!
 * @brief
	- Trova il blocco server a cui e' indirizzata la richiesta,
	- Instrada la richiesta al blocco server corretto.
	- Il blocco server elabora la richiesta e ritorna la risposta.
 */
map<string, string> Http::_process_requests() {
	vector<Directive *> server = this->_find_virtual_server();
	Server *ser = dynamic_cast<Server *> (server[0]);

	return ser->process_request(this->_request);
}

void Http::_send_response(Socket *client_socket, map<string, string> response) {
	HTTP_STATUS status = _statusToEnum(response["status"]);

	Log::response(this->_requestMethod, this->_request["httpVersion"],
				  this->_request["uri"], status);

	string s_response = (this->_request["httpVersion"] + " " +
					   _statusToMessage(status) +
					   "\r\nContent-Type: " + response["Content-Type"] +
					   "\r\n\r\n" + response["body"]);

	ssize_t bytes_sent = send(client_socket->get_socket(), s_response.c_str(),
							  strlen(s_response.c_str()), 0);

	// TODO:
	// Se non si riesce ad inviare tutti i dati, impostare POLLOUT ed inviare
	// i dati rimanenti appena e' possibile scrivere.

	if (bytes_sent == -1) {
		Log::error("Sending the response failed");
		client_socket->close_socket();
	}
}

string Http::_statusToMessage(HTTP_STATUS status) {
	switch (status) {
		case OK:
			return "200 OK";
		case BAD_REQUEST:
			return "400 Bad Request";
		case NOT_FOUND:
			return "404 Not Found";
		default:
			return "500 Internal Server Error";
	}
}

string Http::statusToString(HTTP_STATUS status) {
	switch (status) {
		case OK:
			return "200";
		case BAD_REQUEST:
			return "400";
		case NOT_FOUND:
			return "404";
		default:
			return "500";
	}
}

HTTP_STATUS Http::_statusToEnum(const string &status) {
	if (status == "200")
		return OK;
	else if (status == "400")
		return BAD_REQUEST;
	else if (status == "404")
		return NOT_FOUND;
	else
		return INTERNAL_SERVER_ERROR;
}

HTTP_METHOD Http::_methodToEnum(const string &method) {
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	else
		return UNKNOWN;
}

void Http::start_servers(void) {
	vector<uint16_t> ports = this->_extract_listen_ports();
	// vector<Socket *> sockets;
	int num_ports = ports.size();
	struct pollfd fds[num_ports];
	int num_fds = 0;
	Socket *client_socket;
	string request;
	map<string, string> response;

	// Creates sockets and adds them to the pollfd structure ------------------>
	for (int i = 0; i < num_ports; i++) {
		this->_sockets.push_back(new Socket(ports[i]));
		fds[num_fds].fd = this->_sockets[i]->get_socket();
		fds[num_fds].events = POLLIN | POLLOUT;
		num_fds++;
	}

	// Polling ---------------------------------------------------------------->
	while (true) {
		int poll_status = poll(fds, num_fds, -1);
		if (poll_status == -1) {
			Log::error("Poll failed");
			exit(1);
		}

		// Handle events on sockets ------------------------------------------->
		for (int i = 0; i < num_fds; ++i) {
			if (fds[i].revents & POLLIN) {

				// Accept connection and create new socket for client---------->
				client_socket = this->_sockets[i]->create_client_socket();

				// Read the request ------------------------------------------->
				request = this->_read_requests(client_socket);

				if (!request.empty()) {
					// Parse the request -------------------------------------->
					this->_parse_request(request);

					// Process the requests ----------------------------------->
					response = this->_process_requests();

					// Send the response -------------------------------------->
					this->_send_response(client_socket, response);
				}

				// Close the client socket ------------------------------------>
				client_socket->close_socket();
				delete client_socket;
			}
		}
	}
}
