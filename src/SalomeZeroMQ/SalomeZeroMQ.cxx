// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#pragma once
#include <iostream>

#include "SalomeZeroMQ.hxx"
#include "Basics_Utils.hxx"

DataZMQ::DataZMQ()
{
  this->cont = zmq::context_t(1);
  this->addr = "tcp://" + Kernel_Utils::GetIpByHostname() + ":*";
  this->sock = zmq::socket_t(this->cont, zmq::socket_type::pair);
}

void DataZMQ::connectDataZMQ()
{
  char port[1024];
  size_t size = sizeof(port);
  try{
    this->sock.bind(this->addr);
    sock.getsockopt( ZMQ_LAST_ENDPOINT, &port, &size );
    std::cout << "socket is bound at port " << port << std::endl;
    this->addr = std::string(port);
  }
  catch (zmq::error_t&e ){
    std::cerr << "couldn't bind to socket: " << e.what();
  }
}

void DataZMQ::sendDataZMQ(const char* data, int len)
{
  zmq::message_t message(len);
  std::memcpy (message.data(), data, len);
  this->sock.send(message);
}

std::string DataZMQ::receiveDataZMQ()
{
  std::string s;
  zmq::message_t reply{};
  this->sock.recv(reply, zmq::recv_flags::none);
  s = reply.to_string();

  return s;
}