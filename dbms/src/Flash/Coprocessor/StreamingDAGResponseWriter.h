// Copyright 2022 PingCAP, Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <Common/Logger.h>
#include <Core/Types.h>
#include <DataTypes/IDataType.h>
#include <Flash/Coprocessor/ChunkCodec.h>
#include <Flash/Coprocessor/DAGContext.h>
#include <Flash/Coprocessor/DAGResponseWriter.h>
#include <Flash/Mpp/TrackedMppDataPacket.h>
#include <common/logger_useful.h>

namespace DB
{
/// Serializes the stream of blocks and sends them to TiDB/TiSpark with different serialization paths.
template <class StreamWriterPtr>
class StreamingDAGResponseWriter : public DAGResponseWriter
{
public:
    StreamingDAGResponseWriter(
        StreamWriterPtr writer_,
        Int64 records_per_chunk_,
        Int64 batch_send_min_limit_,
        bool should_send_exec_summary_at_last,
        DAGContext & dag_context_);
    void write(const Block & block) override;
    void finishWrite() override;

private:
    template <bool send_exec_summary_at_last>
    void encodeThenWriteBlocks();

    Int64 batch_send_min_limit;
    bool should_send_exec_summary_at_last; /// only one stream needs to sending execution summaries at last.
    StreamWriterPtr writer;
    std::vector<Block> blocks;
    size_t rows_in_blocks;
    std::unique_ptr<ChunkCodecStream> chunk_codec_stream;
};

} // namespace DB
