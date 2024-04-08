/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcavanna <gcavanna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 16:25:08 by kichkiro          #+#    #+#             */
/*   Updated: 2024/04/08 18:43:42 by gcavanna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

// Class ---------------------------------------------------------------------->

/*!
 * @ref
    Docs:       https://nginx.org/en/docs/http/ngx_http_core_module.html#server
    Syntax:	    server { ... }
    Default:	—
    Context:	http
 */
class Server : public Directive
{
    private:
        map<string, string> _process_get(const ifstream &file);
        map<string, string> _process_post(map<string, string> request);
        map<string, string> _process_delete(map<string, string> request);
        map<string, string> _process_unknown(void);


    public:
        Server(string context);
        Server(ifstream &raw_value, string context);
        ~Server();

        map<string, string> process_request(map<string, string> request, const vector<Directive*>& servers);

        vector<Listen *>            get_listen(void);
        vector<ServerName *>        get_server_name(void);
        vector<Root *>              get_root(void);
        vector<Alias *>             get_alias(void);
        vector<Autoindex *>         get_autoindex(void);
        vector<ErrorPage *>         get_error_page(void);
        vector<Rewrite *>           get_rewrite(void);
        vector<ClientMaxBodySize *> get_client_max_body_size(void);
        vector<Index *>             get_index(void);
        // get_location();
        // get_limit_except();
};
