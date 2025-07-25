/**
 *    Copyright (C) 2025-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */
#pragma once
#include "mongo/db/extension/public/api.h"
#include "mongo/db/extension/sdk/byte_buf_utils.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace mongo::extension::sdk {

class VecByteBuf final : public ::MongoExtensionByteBuf {
public:
    static const ::MongoExtensionByteBufVTable VTABLE;
    // TODO: Fill out VecByteBuf interface as needed.
    VecByteBuf() : ::MongoExtensionByteBuf{&VTABLE}, _buffer() {}
    std::string_view getView() const {
        return std::string_view{reinterpret_cast<const char*>(_buffer.data()), _buffer.size()};
    }

private:
    static void _extDestroy(::MongoExtensionByteBuf* buf) noexcept {
        delete static_cast<extension::sdk::VecByteBuf*>(buf);
    }

    static MongoExtensionByteView _extGetView(const ::MongoExtensionByteBuf* byteBufPtr) noexcept {
        const auto viewSd = static_cast<const extension::sdk::VecByteBuf*>(byteBufPtr)->getView();
        return MongoExtensionByteView{.data = reinterpret_cast<const uint8_t*>(viewSd.data()),
                                      .len = viewSd.length()};
    }
    std::vector<uint8_t> _buffer;
};

}  // namespace mongo::extension::sdk
