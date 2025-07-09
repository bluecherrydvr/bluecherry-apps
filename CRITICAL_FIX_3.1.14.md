# Critical Fix: Segmentation Fault in Recording System (v3.1.14)

## Overview

This document describes the critical bug fix for the segmentation fault that was causing server crashes in Bluecherry v3.1.13 on large installations.

## Problem Description

### Symptoms
- **Segmentation fault** at address `0x0000000000000030` (null pointer dereference)
- **Server crashes** during recording operations
- **Endless loop** of abandoned recording processing after restart
- **Affects large installations** (53+ cameras, 35TB+ data)

### Error Pattern
```
2025-07-08T20:17:27.907056-05:00 nvr01 bc-server[521525]: E(49/Boiler Room (flr22-blr-cam-1)): Error writing frame to recording. Likely timestamping problem.
2025-07-08T20:17:27.907139-05:00 nvr01 bc-server[521525]: E(49/Boiler Room (flr22-blr-cam-1)): Failure in recording writing
2025-07-08T20:17:28.280742-05:00 nvr01 bc-server[521525]: BUG: Segment violation at 0x0000000000000030
```

## Root Cause Analysis

### Primary Issue
The segmentation fault was caused by **null pointer dereferences** in the recording system when accessing stream contexts:

1. **Line 204-206**: `out_ctx->streams[opkt.stream_index]->time_base.num`
   - No bounds checking for `opkt.stream_index`
   - No null pointer validation for `out_ctx->streams[opkt.stream_index]`

2. **Line 232**: `avcodec_get_name(stream->codecpar->codec_id)`
   - `stream` could be NULL in error conditions
   - No null pointer check before accessing `stream->codecpar`

3. **Multiple locations**: `out_ctx->streams[opkt.stream_index]`
   - No validation of stream index bounds
   - No null pointer checks

### Contributing Factors
- **Large installations** with many cameras create more opportunities for corrupted streams
- **High data volume** (35TB+) increases likelihood of encountering corrupted files
- **Missing database indexes** in v3.1.13 exacerbated the problem by causing cleanup issues

## Solution Implementation

### 1. Stream Index Validation
```cpp
// CRITICAL SAFETY CHECK: Validate stream index and context
if (!out_ctx || opkt.stream_index < 0 || opkt.stream_index >= out_ctx->nb_streams || 
    !out_ctx->streams[opkt.stream_index]) {
    bc_log(Error, "Invalid stream context: out_ctx=%p, stream_index=%d, nb_streams=%d", 
           out_ctx, opkt.stream_index, out_ctx ? out_ctx->nb_streams : -1);
    return false;
}
```

### 2. Safe Debug Logging
```cpp
// SAFE DEBUG LOGGING: Add null pointer checks before accessing stream properties
AVStream *debug_stream = out_ctx->streams[opkt.stream_index];
if (debug_stream) {
    bc_log(Debug, "av_interleaved_write_frame: dts=%" PRId64 " pts=%" PRId64 " tb=%d/%d s_i=%d k=%d",
        opkt.dts, opkt.pts, debug_stream->time_base.num,
        debug_stream->time_base.den, opkt.stream_index,
        !!(opkt.flags & AV_PKT_FLAG_KEY));
} else {
    bc_log(Error, "av_interleaved_write_frame: NULL stream at index %d", opkt.stream_index);
    return false;
}
```

### 3. Safe Error Handling
```cpp
// SAFE ERROR HANDLING: Add null pointer checks before accessing stream properties
char err_buf[256];
av_strerror(re, err_buf, sizeof(err_buf));
const char *codec_name = "unknown";
if (stream && stream->codecpar) {
    codec_name = avcodec_get_name(stream->codecpar->codec_id);
}
bc_log(Error, "Error writing %s frame to recording: %s (codec: %s, stream: %d)", 
    pkt.type == AVMEDIA_TYPE_VIDEO ? "video" : "audio",
    err_buf, codec_name, opkt.stream_index);
```

### 4. Stream Initialization Validation
```cpp
// CRITICAL SAFETY CHECK: Validate stream initialization
if (!out_ctx || out_ctx->nb_streams == 0 || !out_ctx->streams[0]) {
    bc_log(Error, "Stream validation failed after muxer initialization: out_ctx=%p, nb_streams=%d", 
           out_ctx, out_ctx ? out_ctx->nb_streams : -1);
    close();
    return -1;
}
```

### 5. Safe Cleanup Procedures
```cpp
// SAFE CLEANUP: Add null pointer checks before accessing context
if (out_ctx->pb) {
    // Only write trailer if the context is still valid
    if (out_ctx->nb_streams > 0 && out_ctx->streams[0]) {
        av_write_trailer(out_ctx);
    }
    avio_close(out_ctx->pb);
}
```

## Files Modified

- `server/media_writer.cpp` - Added comprehensive null pointer checks

## Testing

### Code Validation
- ✅ **Syntax check**: Code compiles successfully
- ✅ **Null pointer protection**: All stream access points protected
- ✅ **Bounds checking**: Stream index validation implemented
- ✅ **Error handling**: Safe error reporting without crashes

### Expected Behavior
- **No more segmentation faults** when encountering corrupted streams
- **Graceful error handling** with proper logging
- **Continued operation** even with problematic camera streams
- **Safe cleanup** during recording failures

## Deployment Instructions

### For Customers Affected by v3.1.13
1. **Immediate action**: Rollback to v3.1.9 if server is down
2. **Apply database indexes** (if not already done):
   ```sql
   CREATE INDEX IF NOT EXISTS idx_media_cleanup ON Media (archive, start);
   CREATE INDEX IF NOT EXISTS idx_media_filepath ON Media (filepath);
   CREATE INDEX IF NOT EXISTS idx_eventscam_media_id ON EventsCam (media_id);
   CREATE INDEX IF NOT EXISTS idx_media_archive ON Media (archive);
   CREATE INDEX IF NOT EXISTS idx_media_start ON Media (start);
   CREATE INDEX IF NOT EXISTS idx_storage_priority ON Storage (priority);
   ```
3. **Upgrade to v3.1.14** when available
4. **Monitor logs** for any remaining issues

### For New Installations
- Use v3.1.14 or later
- No additional configuration required

## Prevention

### Code Quality Improvements
- **Static analysis**: Add null pointer detection to build process
- **Unit testing**: Add tests for corrupted stream scenarios
- **Code review**: Require null pointer checks for all stream access

### Monitoring
- **Log monitoring**: Watch for "Invalid stream context" errors
- **Performance monitoring**: Track recording failures
- **Alert system**: Notify on repeated recording errors

## Impact Assessment

### Severity: CRITICAL
- **Data loss**: Potential loss of recordings during crashes
- **Service disruption**: Server downtime affecting all cameras
- **Customer impact**: Complete loss of surveillance capability

### Affected Systems
- **Large installations**: 50+ cameras, 30TB+ storage
- **High-traffic systems**: Many concurrent recording streams
- **Older camera models**: May have more stream corruption issues

## Conclusion

This fix addresses a critical vulnerability in the recording system that could cause complete server failure on large installations. The comprehensive null pointer checks ensure that the system can handle corrupted streams gracefully without crashing.

**Recommendation**: Deploy v3.1.14 immediately to all affected customers and include this fix in all future releases. 