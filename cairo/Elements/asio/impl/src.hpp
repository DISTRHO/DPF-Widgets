//
// impl/src.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_SRC_HPP
#define ASIO_IMPL_SRC_HPP

#define ASIO_SOURCE

#include "Elements/asio/detail/config.hpp"

#if defined(ASIO_HEADER_ONLY)
# error Do not compile Asio library source with ASIO_HEADER_ONLY defined
#endif

#include "Elements/asio/impl/error.ipp"
#include "Elements/asio/impl/error_code.ipp"
#include "Elements/asio/impl/execution_context.ipp"
#include "Elements/asio/impl/executor.ipp"
#include "Elements/asio/impl/handler_alloc_hook.ipp"
#include "Elements/asio/impl/io_context.ipp"
#include "Elements/asio/impl/serial_port_base.ipp"
#include "Elements/asio/impl/system_context.ipp"
#include "Elements/asio/impl/thread_pool.ipp"
#include "Elements/asio/detail/impl/buffer_sequence_adapter.ipp"
#include "Elements/asio/detail/impl/descriptor_ops.ipp"
#include "Elements/asio/detail/impl/dev_poll_reactor.ipp"
#include "Elements/asio/detail/impl/epoll_reactor.ipp"
#include "Elements/asio/detail/impl/eventfd_select_interrupter.ipp"
#include "Elements/asio/detail/impl/handler_tracking.ipp"
#include "Elements/asio/detail/impl/kqueue_reactor.ipp"
#include "Elements/asio/detail/impl/null_event.ipp"
#include "Elements/asio/detail/impl/pipe_select_interrupter.ipp"
#include "Elements/asio/detail/impl/posix_event.ipp"
#include "Elements/asio/detail/impl/posix_mutex.ipp"
#include "Elements/asio/detail/impl/posix_thread.ipp"
#include "Elements/asio/detail/impl/posix_tss_ptr.ipp"
#include "Elements/asio/detail/impl/reactive_descriptor_service.ipp"
#include "Elements/asio/detail/impl/reactive_serial_port_service.ipp"
#include "Elements/asio/detail/impl/reactive_socket_service_base.ipp"
#include "Elements/asio/detail/impl/resolver_service_base.ipp"
#include "Elements/asio/detail/impl/scheduler.ipp"
#include "Elements/asio/detail/impl/select_reactor.ipp"
#include "Elements/asio/detail/impl/service_registry.ipp"
#include "Elements/asio/detail/impl/signal_set_service.ipp"
#include "Elements/asio/detail/impl/socket_ops.ipp"
#include "Elements/asio/detail/impl/socket_select_interrupter.ipp"
#include "Elements/asio/detail/impl/strand_executor_service.ipp"
#include "Elements/asio/detail/impl/strand_service.ipp"
#include "Elements/asio/detail/impl/throw_error.ipp"
#include "Elements/asio/detail/impl/timer_queue_ptime.ipp"
#include "Elements/asio/detail/impl/timer_queue_set.ipp"
#include "Elements/asio/detail/impl/win_iocp_handle_service.ipp"
#include "Elements/asio/detail/impl/win_iocp_io_context.ipp"
#include "Elements/asio/detail/impl/win_iocp_serial_port_service.ipp"
#include "Elements/asio/detail/impl/win_iocp_socket_service_base.ipp"
#include "Elements/asio/detail/impl/win_event.ipp"
#include "Elements/asio/detail/impl/win_mutex.ipp"
#include "Elements/asio/detail/impl/win_object_handle_service.ipp"
#include "Elements/asio/detail/impl/win_static_mutex.ipp"
#include "Elements/asio/detail/impl/win_thread.ipp"
#include "Elements/asio/detail/impl/win_tss_ptr.ipp"
#include "Elements/asio/detail/impl/winrt_ssocket_service_base.ipp"
#include "Elements/asio/detail/impl/winrt_timer_scheduler.ipp"
#include "Elements/asio/detail/impl/winsock_init.ipp"
#include "Elements/asio/generic/detail/impl/endpoint.ipp"
#include "Elements/asio/ip/impl/address.ipp"
#include "Elements/asio/ip/impl/address_v4.ipp"
#include "Elements/asio/ip/impl/address_v6.ipp"
#include "Elements/asio/ip/impl/host_name.ipp"
#include "Elements/asio/ip/impl/network_v4.ipp"
#include "Elements/asio/ip/impl/network_v6.ipp"
#include "Elements/asio/ip/detail/impl/endpoint.ipp"
#include "Elements/asio/local/detail/impl/endpoint.ipp"

#endif // ASIO_IMPL_SRC_HPP
