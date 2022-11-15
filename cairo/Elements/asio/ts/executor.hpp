//
// ts/executor.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_TS_EXECUTOR_HPP
#define ASIO_TS_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "Elements/asio/async_result.hpp"
#include "Elements/asio/associated_allocator.hpp"
#include "Elements/asio/execution_context.hpp"
#include "Elements/asio/is_executor.hpp"
#include "Elements/asio/associated_executor.hpp"
#include "Elements/asio/bind_executor.hpp"
#include "Elements/asio/executor_work_guard.hpp"
#include "Elements/asio/system_executor.hpp"
#include "Elements/asio/executor.hpp"
#include "Elements/asio/dispatch.hpp"
#include "Elements/asio/post.hpp"
#include "Elements/asio/defer.hpp"
#include "Elements/asio/strand.hpp"
#include "Elements/asio/packaged_task.hpp"
#include "Elements/asio/use_future.hpp"

#endif // ASIO_TS_EXECUTOR_HPP
