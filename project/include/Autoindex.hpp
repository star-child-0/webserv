/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Autoindex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 16:41:28 by kichkiro          #+#    #+#             */
/*   Updated: 2024/04/22 10:36:05 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

// Class ---------------------------------------------------------------------->

/*!
 * @ref
    Docs:       https://nginx.org/en/docs/http/ngx_http_autoindex_module.html#autoindex
    Syntax:	    autoindex on | off;
    Default:    autoindex off;
    Context:	http, server, location
 */
class Autoindex : public ADirective
{
	private:
		bool	_isEnabled;

		Autoindex();

	public:
		Autoindex(uint16_t context, bool isEnabled);
		Autoindex(const Autoindex& copy);
		~Autoindex();
		Autoindex&	operator=(const Autoindex& other);

		ADirective*	clone() const;

		bool	getMode() const;
};
