/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include "maidsafe/lifestuff_manager/tcp_connection.h"

#include <array>
#include <algorithm>
#include <functional>

#include "boost/asio/error.hpp"
#include "boost/asio/read.hpp"
#include "boost/asio/write.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/lifestuff_manager/local_tcp_transport.h"
#include "maidsafe/lifestuff_manager/return_codes.h"


namespace asio = boost::asio;
namespace bs = boost::system;
namespace ip = asio::ip;
namespace bptime = boost::posix_time;
namespace args = std::placeholders;

namespace maidsafe {

namespace lifestuff_manager {

TcpConnection::TcpConnection(const std::shared_ptr<LocalTcpTransport>& transport)
    : transport_(transport),
      strand_(transport->asio_service_),
      socket_(transport->asio_service_),
      size_buffer_(sizeof(LocalTcpTransport::DataSize)),
      data_buffer_(),
      data_size_(0),
      data_received_(0) {
  static_assert((sizeof(LocalTcpTransport::DataSize)) == 4, "DataSize must be 4 bytes.");
}

int TcpConnection::Connect(const uint16_t& remote_port) {
  assert(!socket_.is_open());
  boost::system::error_code ec;
  socket_.connect(ip::tcp::endpoint(ip::address_v4::loopback(), remote_port), ec);
  if (ec || !socket_.is_open()) {
    LOG(kError) << "Failed to connect: " << ec.message();
    socket_.close(ec);
    return kConnectFailure;
  }
  StartReceiving();
  return kSuccess;
}

void TcpConnection::Close() {
  strand_.dispatch(std::bind(&TcpConnection::DoClose, shared_from_this()));
}

void TcpConnection::DoClose() {
  bs::error_code ignored_ec;
  socket_.close(ignored_ec);
  if (std::shared_ptr<LocalTcpTransport> transport = transport_.lock())
    transport->RemoveConnection(shared_from_this());
}

void TcpConnection::StartReceiving() {
  strand_.dispatch(std::bind(&TcpConnection::DoStartReceiving, shared_from_this()));
}

void TcpConnection::DoStartReceiving() {
  StartReadSize();
}

void TcpConnection::StartSending(const std::string& data) {
  EncodeData(data);
  strand_.dispatch(std::bind(&TcpConnection::DoStartSending, shared_from_this()));
}

void TcpConnection::DoStartSending() {
  StartWrite();
}

void TcpConnection::StartReadSize() {
  if (!socket_.is_open())
    return;
  asio::async_read(socket_, asio::buffer(size_buffer_),
                   strand_.wrap(std::bind(&TcpConnection::HandleReadSize,
                                          shared_from_this(), args::_1)));
}

void TcpConnection::HandleReadSize(const bs::error_code& ec) {
  if (!socket_.is_open())
    return;

  if (ec) {
    if (ec == asio::error::eof) {
      /*Sleep(std::chrono::milliseconds(10));*/
      strand_.post(std::bind(&TcpConnection::StartReadSize, shared_from_this()));
    } else {
      if (ec != asio::error::connection_reset)
        LOG(kError) << ec.message();
      Close();
    }
    return;
  }

  LocalTcpTransport::DataSize size = (((((size_buffer_.at(0) << 8) | size_buffer_.at(1)) << 8) |
                                       size_buffer_.at(2)) << 8) | size_buffer_.at(3);

  data_size_ = size;
  data_received_ = 0;

  StartReadData();
}

void TcpConnection::StartReadData() {
  if (!socket_.is_open())
    return;

  size_t buffer_size = data_received_;
  buffer_size += std::min(static_cast<size_t>(kMaxTransportChunkSize()),
                          data_size_ - data_received_);
  data_buffer_.resize(buffer_size);

  asio::mutable_buffer data_buffer = asio::buffer(data_buffer_) + data_received_;
  asio::async_read(socket_, asio::buffer(data_buffer),
                   strand_.wrap(std::bind(&TcpConnection::HandleReadData,
                                          shared_from_this(),
                                          args::_1, args::_2)));
}

void TcpConnection::HandleReadData(const bs::error_code& ec, size_t length) {
  if (!socket_.is_open())
    return;

  if (ec) {
    LOG(kError) << "HandleReadData - Failed: " << ec.message();
    strand_.post(std::bind(&TcpConnection::StartReadSize, shared_from_this()));
    return;
  }

  data_received_ += length;

  if (data_received_ == data_size_) {
    // Dispatch the message outside the strand.
    strand_.get_io_service().post(std::bind(&TcpConnection::DispatchMessage, shared_from_this()));
  } else {
    // Need more data to complete the message.
    StartReadData();
  }
}

void TcpConnection::DispatchMessage() {
  if (std::shared_ptr<LocalTcpTransport> transport = transport_.lock()) {
    boost::system::error_code error_code;
    transport->on_message_received_(std::string(data_buffer_.begin(), data_buffer_.end()),
                                    socket_.remote_endpoint(error_code).port());
    strand_.dispatch(std::bind(&TcpConnection::StartReadSize, shared_from_this()));
  }
}

void TcpConnection::EncodeData(const std::string& data) {
  // Serialize message to internal buffer
  LocalTcpTransport::DataSize msg_size = static_cast<LocalTcpTransport::DataSize>(data.size());
  for (int i = 0; i != 4; ++i)
    size_buffer_.at(i) = static_cast<char>(msg_size >> (8 * (3 - i)));
  data_buffer_.assign(data.begin(), data.end());
}

void TcpConnection::StartWrite() {
  if (!socket_.is_open())
    return;

  std::array<boost::asio::const_buffer, 2> asio_buffer;
  asio_buffer[0] = boost::asio::buffer(size_buffer_);
  asio_buffer[1] = boost::asio::buffer(data_buffer_);
  asio::async_write(socket_, asio_buffer,
                    strand_.wrap(std::bind(&TcpConnection::HandleWrite,
                                           shared_from_this(), args::_1)));
}

void TcpConnection::HandleWrite(const bs::error_code& ec) {
  if (ec) {
    LOG(kError) << ec.message();
    if (std::shared_ptr<LocalTcpTransport> transport = transport_.lock())
      transport->on_error_(kSendFailure);
  }
}

}  // namespace lifestuff_manager

}  // namespace maidsafe
