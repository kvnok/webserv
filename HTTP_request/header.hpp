/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   header.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:55:24 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/02 09:32:09 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "request.hpp"

class Header {
	private:
		std::string _key;
		std::string _value;
	public:
		Header();
		~Header();
};
