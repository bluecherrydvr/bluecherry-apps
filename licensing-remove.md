# Licensing Removal TODO List

## Overview
This document outlines the steps required to remove all Cryptlex licensing from Bluecherry and make it completely free and open source with no licensing restrictions.

## Build System Changes

### 1. Remove V3 Licensing Compilation
- [x] **File**: `server/BCMK`
- [x] **Action**: Remove `CFLAGS += -DV3_LICENSING` from line 4
- [x] **Effect**: Disables all V3 licensing code compilation

### 2. Update Makefile Dependencies
- [x] **File**: `server/BCMK`
- [x] **Action**: Remove `v3license_server.o v3license_processor.o` from object files list
- [x] **Effect**: Excludes licensing components from build

## Server Code Removal

### 3. Remove V3 Licensing Files
- [x] **Delete**: `server/v3license_server.h`
- [x] **Delete**: `server/v3license_server.cpp`
- [x] **Delete**: `server/v3license_processor.h`
- [x] **Delete**: `server/v3license_processor.cpp`
- [x] **Delete**: `server/lexapi/` directory (entire Cryptlex SDK)

### 4. Update Main Server Code
- [x] **File**: `server/bc-server.cpp`
- [x] **Action**: Remove all `#ifdef V3_LICENSING` blocks
- [x] **Lines**: 47-49, 99-101, 1927-1939, 1969-1972, 2052-2055
- [x] **Effect**: Removes v3license server initialization, thread creation, and cleanup

### 5. Remove License Utility Programs
- [x] **Delete**: `utils/licensecmd.cpp`
- [x] **Delete**: `utils/bc-license.cpp`
- [x] **Delete**: `lib/bc-key.cpp`

## Web Interface Changes

### 6. Update PHP License Wrapper
- [x] **File**: `www/lib/bc_license_wrapper.php`
- [x] **Action**: Modify `bc_license_devices_allowed()` to always return `-1` (unlimited)
- [x] **Action**: Modify `bc_license_check_genuine()` to always return success
- [x] **Action**: Remove all actual license checking logic

### 7. Remove Device Limits
- [x] **File**: `www/lib/lib.php`
- [x] **Action**: Modify `checkLimitDevices()` function to always return `true`
- [x] **Action**: Remove device count validation in `camera::create()`
- [x] **Lines**: ~958, ~1015-1019

### 8. Remove License Management Interface
- [x] **File**: `www/ajax/licenses.php`
- [x] **Action**: Remove or disable entire license management interface
- [x] **Action**: Remove license activation/deactivation functionality
- [x] **Action**: Remove license status checking

### 9. Update Main Web Interface
- [x] **File**: `www/index.php`
- [x] **Action**: Remove license checking from main page (lines 29-30)
- [x] **Action**: Remove license-related error messages

### 10. Remove License Templates
- [x] **Delete**: `www/template/ajax/licenses.php`
- [x] **Delete**: Any other license-related template files

## Database Changes

### 11. Remove License Tables
- [x] **Action**: Remove `Licenses` table entirely
- [x] **Action**: Remove any license-related columns from other tables
- [x] **Action**: Update database schema scripts

### 12. Update Database Upgrade Scripts
- [x] **Action**: Remove license-related database migrations
- [x] **Action**: Add migration to clean up existing license data

## Configuration Changes

### 13. Remove License Server Configuration
- [x] **Action**: Remove license server endpoints (port 7004)
- [x] **Action**: Remove license validation from startup
- [x] **Action**: Remove license-related configuration options

### 14. Update Systemd/Init Scripts
- [x] **Action**: Remove license server service definitions
- [x] **Action**: Update startup scripts to not check for licenses

## Documentation Updates

### 15. Update Installation Documentation
- [ ] **Action**: Remove license setup steps from installation guides
- [ ] **Action**: Update user manuals to remove license management sections
- [ ] **Action**: Remove licensing documentation entirely

### 16. Update README and Project Files
- [ ] **Action**: Update README.md to reflect free/open source status
- [ ] **Action**: Remove license requirements from project description
- [ ] **Action**: Update any license-related project metadata

## Testing and Validation

### 17. Test Device Limits Removal
- [ ] **Action**: Verify unlimited camera addition works
- [ ] **Action**: Test that no device count restrictions exist
- [ ] **Action**: Verify web interface shows no license warnings

### 18. Test Server Startup
- [ ] **Action**: Verify server starts without license validation
- [ ] **Action**: Test that no license-related errors occur
- [ ] **Action**: Verify all core functionality works without licenses

### 19. Test Web Interface
- [ ] **Action**: Verify no license management UI appears
- [ ] **Action**: Test device addition without license restrictions
- [ ] **Action**: Verify no license-related error messages

## Cleanup Tasks

### 20. Remove License-Related Constants
- [x] **File**: `www/lib/var.php`
- [x] **Action**: Remove license-related constant definitions
- [x] **Action**: Remove license status messages

### 21. Remove License Language Strings
- [x] **File**: `www/lib/lang.php`
- [x] **Action**: Remove all license-related language definitions
- [x] **Action**: Clean up unused language strings

### 22. Update Error Handling
- [x] **Action**: Remove license-related error handling code
- [x] **Action**: Update error messages to not mention licensing
- [x] **Action**: Ensure graceful handling when license checks are removed

## Post-Removal Verification

### 23. Build Verification
- [ ] **Action**: Verify clean build without licensing components
- [ ] **Action**: Test that all functionality works as expected
- [ ] **Action**: Verify no licensing code remains in binary

### 24. Functionality Testing
- [ ] **Action**: Test camera addition (should be unlimited)
- [ ] **Action**: Test recording functionality
- [ ] **Action**: Test web interface functionality
- [ ] **Action**: Test API functionality

### 25. Performance Testing
- [ ] **Action**: Verify no performance impact from license removal
- [ ] **Action**: Test with large numbers of cameras
- [ ] **Action**: Verify memory usage is not affected

## Optional Enhancements

### 26. Add Free Software Notice
- [ ] **Action**: Add prominent notice that software is now free
- [ ] **Action**: Update copyright notices if needed
- [ ] **Action**: Add information about community support

### 27. Update Version Information
- [ ] **Action**: Update version to reflect major licensing change
- [ ] **Action**: Update changelog to document licensing removal
- [ ] **Action**: Consider new version numbering scheme

## Progress Log

### 2025-01-XX - Initial Setup
- [x] Created TODO list
- [x] Created new branch `remove-licensing` from `release-3.1.14`
- [x] Started build system changes
- [x] Removed V3_LICENSING compilation flag and dependencies

## Notes

- **Priority**: High - This is a major architectural change
- **Testing**: Extensive testing required after each major change
- **Backup**: Ensure database backups before removing license tables
- **Rollback**: Keep original files until testing is complete
- **Documentation**: Update all user-facing documentation

## Impact Summary

**What Will Be Removed:**
- Device limits and camera restrictions
- Trial period limitations
- License validation and checking
- Machine binding and hardware fingerprinting
- License management UI
- Cryptlex integration and dependencies

**What Will Remain:**
- All core surveillance functionality
- User management and permissions
- Device support (all camera types)
- Web interface (minus license management)
- API functionality
- Storage management
- ONVIF support

**Result:**
- Completely free and open source surveillance system
- No external licensing dependencies
- Unlimited device support
- Simplified installation and deployment 