/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcavanna <gcavanna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 10:42:17 by kichkiro          #+#    #+#             */
/*   Updated: 2024/05/03 11:59:49 by gcavanna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

std::vector<HTTP_METHOD> allHttpMethods;

std::vector<HTTP_STATUS> allHttpStatus;

void initializeHttpVectors()
{
	if (!(allHttpMethods.empty()))
		return;
	allHttpMethods.push_back(GET);
	allHttpMethods.push_back(POST);
	allHttpMethods.push_back(DELETE);
	allHttpMethods.push_back(UNKNOWN);

	if (!(allHttpStatus.empty()))
		return;
	allHttpStatus.push_back(OK);
	allHttpStatus.push_back(BAD_REQUEST);
	allHttpStatus.push_back(NOT_FOUND);
	allHttpStatus.push_back(FORBIDDEN);
	allHttpStatus.push_back(METHOD_NOT_ALLOWED);
	allHttpStatus.push_back(REQUEST_TIMEOUT);
	allHttpStatus.push_back(PAYLOAD_TOO_LARGE);
	allHttpStatus.push_back(REQUEST_HEADER_FIELDS_TOO_LARGE);
	allHttpStatus.push_back(INTERNAL_SERVER_ERROR);
	allHttpStatus.push_back(BAD_GATEWAY);
}

Http::Http() : ADirective("http", GLOBAL_CONTEXT)
{ initializeHttpVectors(); }

Http::Http(const Http &copy) : ADirective(copy)
{ *this = copy; }

Http::~Http() {}

Http &Http::operator=(const Http &other)
{
	if (this != &other) {
		ADirective::operator=(other);
	}

	return *this;
}

ADirective *Http::clone() const
{ return new Http(*this); }

enum HTTP_METHOD	Http::_methodToEnum(const std::string &method)
{
	std::cout << "'" << method << "'" << std::endl;
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	else
		return UNKNOWN;
}

enum HTTP_STATUS	Http::_statusToEnum(const std::string &status)
{
	if (status == "200")
		return OK;
	else if (status == "302")
		return FOUND;
	else if (status == "400")
		return BAD_REQUEST;
	else if (status == "403")
		return FORBIDDEN;
	else if (status == "404")
		return NOT_FOUND;
	else if (status == "405")
		return METHOD_NOT_ALLOWED;
	else if (status == "408")
		return REQUEST_TIMEOUT;
	else if (status == "413")
		return PAYLOAD_TOO_LARGE;
	else if (status == "431")
		return REQUEST_HEADER_FIELDS_TOO_LARGE;
	else if (status == "502")
		return BAD_GATEWAY;
	else
		return INTERNAL_SERVER_ERROR;
}

std::string	Http::_statusToMessage(enum HTTP_STATUS status)
{
	switch (status)
	{
		case OK:
			return "200 OK";
		case FOUND:
			return "302 Found";
		case BAD_REQUEST:
			return "400 Bad Request";
		case FORBIDDEN:
			return "403 Forbidden";
		case NOT_FOUND:
			return "404 Not Found";
		case METHOD_NOT_ALLOWED:
			return "405 Method Not Allowed";
		case REQUEST_TIMEOUT:
			return "408 Request Timeout";
		case PAYLOAD_TOO_LARGE:
			return "413 Payload Too Large";
		case REQUEST_HEADER_FIELDS_TOO_LARGE:
			return "431 Request Header Fields Too Large";
		default:
			return "500 Internal Server Error";
	}
}

std::string	Http::_statusToString(enum HTTP_STATUS status)
{
	switch (status)
	{
		case OK:
			return "200";
		case FOUND:
			return "302";
		case BAD_REQUEST:
			return "400";
		case FORBIDDEN:
			return "403";
		case NOT_FOUND:
			return "404";
		case METHOD_NOT_ALLOWED:
			return "405";
		case REQUEST_TIMEOUT:
			return "408";
		case PAYLOAD_TOO_LARGE:
			return "413";
		case REQUEST_HEADER_FIELDS_TOO_LARGE:
			return "431";
		default:
			return "500";
	}
}
