/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   httpStatus.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/07 16:46:21 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/06/13 23:26:14 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

using namespace std;

const string	httpStatusMessage(int statusCode);
const string	getHtmlPath(int statusCode);
const string	fourZeroFourBody();
