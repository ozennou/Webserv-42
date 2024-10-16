/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logs.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 16:02:34 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/16 16:18:12 by mozennou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <header.hpp>

void    displayTimestamp(const char *color)
{
    time_t  now;
    char    time_buffer[30];

    now = time(NULL);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S ", localtime(&now));
    cerr << color << time_buffer;
}

void    logging(const string& msg, int level)
{
    if (level == INFO)
    {
        displayTimestamp(GREEN);
        cerr << "[INFO] :" << msg << RESET << endl; 
    }
    else if (level == WARNING)
    {
        displayTimestamp(YELLOW);
        cerr << "[WARNING] :" << msg << RESET << endl; 
    }
    else if (level == ERROR)
    {
        displayTimestamp(RED);
        cerr << "[ERROR] :" << msg << RESET << endl; 
    }
}