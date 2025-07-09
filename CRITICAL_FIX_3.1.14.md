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

### Primary Issues Identified

1. **media_writer.cpp**: Unprotected stream access in recording system
   - Line 204-206: `out_ctx->streams[opkt.stream_index]->time_base.num`
   - Line 232: `avcodec_get_name(stream->codecpar->codec_id)`
   - Missing bounds checking for stream indices

2. **streaming.cpp**: Unprotected stream access in streaming system
   - Line 285-286: RTP streaming context access without validation
   - Line 335-336: HLS streaming context access without validation
   - Line 363: Codec parameter access without null checks

3. **lavf_device.cpp**: Unprotected stream access in device drivers
   - Line 397: Stream timebase access without validation
   - Multiple locations in `update_properties()` and `stream_info()`
   - Missing bounds checking for stream indices

### Root Cause
The segmentation fault occurs when the system tries to access stream properties on corrupted or invalid stream contexts. This happens more frequently on large installations due to:
- Higher stream count (53+ cameras)
- Increased memory pressure
- Race conditions during stream initialization
- Corrupted stream contexts from network issues

## Fix Implementation

### 1. media_writer.cpp Fixes
```cpp
// Added comprehensive null pointer checks
if (!out_ctx || opkt.stream_index < 0 || 
    opkt.stream_index >= out_ctx->nb_streams || 
    !out_ctx->streams[opkt.stream_index]) {
    bc_log(Error, "Invalid stream access in media_writer");
    return false;
}
```

### 2. streaming.cpp Fixes
```cpp
// Added RTP streaming context validation
if (!bc_rec->rtp_stream_ctx[ctx_index] || 
    !bc_rec->rtp_stream_ctx[ctx_index]->streams || 
    bc_rec->rtp_stream_ctx[ctx_index]->nb_streams == 0 ||
    !bc_rec->rtp_stream_ctx[ctx_index]->streams[0]) {
    bc_rec->log.log(Error, "Invalid RTP streaming context");
    return -1;
}
```

### 3. lavf_device.cpp Fixes
```cpp
// Added stream access validation
if (!ctx || !ctx->streams || src->stream_index < 0 || 
    src->stream_index >= ctx->nb_streams || 
    !ctx->streams[src->stream_index]) {
    bc_log(Error, "Invalid stream access in lavf_device");
    return;
}
```

## Deployment Instructions

### For Customers Experiencing Crashes

1. **Immediate Action**: Rollback to v3.1.9
   ```bash
   sudo systemctl stop bc-server
   sudo apt-get install bluecherry=3:3.1.9-1
   sudo systemctl start bc-server
   ```

2. **Clear Abandoned Recordings**
   ```sql
   mysql -u root -p bluecherry -e "UPDATE EventsCam SET length=0 WHERE length=-1;"
   ```

3. **Apply Database Indexes** (if upgrading to 3.1.13+)
   ```sql
   CREATE INDEX IF NOT EXISTS idx_media_cleanup ON Media (archive, start);
   CREATE INDEX IF NOT EXISTS idx_media_filepath ON Media (filepath);
   CREATE INDEX IF NOT EXISTS idx_eventscam_media_id ON EventsCam (media_id);
   ```

### For New Installations

1. **Use v3.1.14 or later** which includes all fixes
2. **Ensure proper database indexes** are applied
3. **Monitor system resources** on large installations

## Prevention Strategies

### System Monitoring
- Monitor memory usage on large installations
- Watch for stream initialization errors
- Check for corrupted recording files

### Configuration Recommendations
- Increase system memory for installations with 50+ cameras
- Use SSD storage for recording directories
- Implement proper network redundancy

### Code Quality
- All stream access now includes null pointer checks
- Bounds checking implemented for all array accesses
- Comprehensive error logging for debugging

## Impact Assessment

### Affected Systems
- **High Impact**: Large installations (50+ cameras, 30TB+ data)
- **Medium Impact**: Medium installations (20-50 cameras)
- **Low Impact**: Small installations (<20 cameras)

### Performance Impact
- **Minimal**: Null pointer checks add negligible overhead
- **Improved Stability**: Prevents catastrophic crashes
- **Better Error Handling**: Graceful degradation instead of crashes

## Testing

### Test Cases
- [x] Large installation simulation (53 cameras, 35TB data)
- [x] Corrupted stream context handling
- [x] Network interruption recovery
- [x] Memory pressure scenarios
- [x] Race condition testing

### Validation
- [x] Code compiles successfully
- [x] Null pointer protection verified
- [x] Error handling tested
- [x] Performance impact measured

## Version History

- **v3.1.13**: Introduced new cleanup system (caused crashes)
- **v3.1.14**: Comprehensive null pointer fixes implemented
- **Future**: Additional monitoring and prevention features planned

## Support

For customers experiencing issues:
1. Check system logs for segmentation faults
2. Verify database indexes are applied
3. Consider rolling back to v3.1.9 if crashes persist
4. Contact support with detailed error logs

---

**Note**: This fix addresses the critical stability issues that were affecting large installations. The comprehensive null pointer protection ensures the system can handle corrupted streams gracefully without crashing. 