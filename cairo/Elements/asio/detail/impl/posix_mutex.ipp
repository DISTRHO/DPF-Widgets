//
// detail/impl/posix_mutex.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP
#define ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "Elements/asio/detail/config.hpp"

#if defined(ASIO_HAS_PTHREADS)

#include "Elements/asio/detail/posix_mutex.hpp"
#include "Elements/asio/detail/throw_error.hpp"
#include "Elements/asio/error.hpp"

#include "Elements/asio/detail/push_options.hpp"

namespace asio {
namespace detail {

posix_mutex::posix_mutex()
{
  int error = ::pthread_mutex_init(&mutex_, 0);
  asio::error_code ec(error,
      asio::error::get_system_category());
  asio::detail::throw_error(ec, "mutex");
}

} // namespace detail
} // namespace asio

#include "Elements/asio/detail/pop_options.hpp"

#endif // defined(ASIO_HAS_PTHREADS)

#endif // ASIO_DETAIL_IMPL_POSIX_MUTEX_IPP
