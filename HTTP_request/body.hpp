/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   body.hpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/01 17:54:32 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/02 09:29:58 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "request.hpp"

class Body{
	private:
		std::string _bodyText;
	public:
		Body();
		~Body();
};
