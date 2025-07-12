# BlueCherry Database Optimization (3.1.14)

## Overview

This document describes the comprehensive database optimization implemented in version 3.1.14 to address critical MySQL connection and transaction issues that were causing server instability and recording failures.

## Problem Statement

The original database implementation suffered from several critical issues:

- **Single Global Connection**: All database operations shared one MySQL connection, causing bottlenecks
- **Connection Exhaustion**: When the connection was lost or busy, all operations failed
- **Transaction Conflicts**: "Commands out of sync" errors due to race conditions
- **Insufficient Timeouts**: Very short connection timeouts (5 seconds) causing failures under load
- **No Connection Health Monitoring**: No active monitoring of connection health
- **PHP Connection Issues**: Web interface created new connections without proper pooling

## Root Cause Analysis

### 1. Connection Pool Issues
- **Single global connection** (`my_con_global`) for entire server
- **No connection pooling** - all operations serialized through global lock
- **Connection exhaustion** during high load periods
- **No parallel processing** of database operations

### 2. Transaction Synchronization Issues
- **Race conditions** when multiple cameras start recordings simultaneously
- **Database lock timeouts** allowing operations without proper locking
- **"Commands out of sync" errors** from MySQL due to transaction conflicts
- **Insufficient retry logic** for failed transactions

### 3. PHP Database Handling Issues
- **New connections per request** without pooling
- **No connection health checking** in web interface
- **Insufficient error handling** for connection failures
- **No retry logic** for failed operations

## Solution: Comprehensive Database Optimization

### 1. Connection Pooling Implementation

#### C++ Database Layer (`lib/bc-db-mysql.cpp`)
- **Connection Pool Structure**: Added `mysql_connection` struct with up to 10 pooled connections
- **Connection Health Monitoring**: Active ping checking and connection validation
- **Improved Timeouts**: Increased from 5 seconds to 30 seconds for read/write operations
- **Retry Logic**: Added retry mechanism with exponential backoff
- **Connection Cleanup**: Automatic cleanup of stale connections

```cpp
// Connection pool structure
struct mysql_connection {
    MYSQL *con;
    time_t last_used;
    bool in_use;
    pthread_mutex_t lock;
};

#define MAX_CONNECTIONS 10
#define CONNECTION_TIMEOUT 300  // 5 minutes
#define CONNECTION_CHECK_INTERVAL 60  // Check every minute
```

#### Key Improvements:
- **Pooled Connections**: Up to 10 concurrent database connections
- **Connection Health Checks**: `mysql_ping()` validation before operations
- **Improved Timeouts**: 30-second read/write timeouts vs 5 seconds
- **Automatic Reconnection**: Transparent reconnection on connection loss
- **Connection Cleanup**: Automatic cleanup of idle connections

### 2. Transaction Handling Improvements

#### Database Lock Management (`lib/bc-db-core.cpp`)
- **Timeout Protection**: 10-second timeout for database locks with retry mechanism
- **Lock State Tracking**: `db_lock_available` flag to prevent operations without locks
- **Graceful Degradation**: Return errors instead of continuing without proper locking

#### Transaction Functions (`lib/bc-db-mysql.cpp`)
- **Retry Logic**: 3 retry attempts for transaction operations
- **Connection Reset**: Automatic connection reset on "Commands out of sync" errors
- **Error Recovery**: Proper handling of connection loss during transactions

```cpp
static int bc_db_mysql_start_trans(void)
{
    MYSQL *my_con = get_handle();
    int ret;

    if (my_con == NULL)
        return -1;

    unsigned int retries = 3;
    for (; retries; retries--) {
        ret = mysql_query(my_con, "START TRANSACTION");
        if (!ret)
            break;

        int err = mysql_errno(my_con);
        if (err == CR_COMMANDS_OUT_OF_SYNC) {
            // Handle "Commands out of sync" error
            bc_log(Warning, "MySQL commands out of sync, attempting to reset connection");
            my_con = reset_con();
            if (!my_con) {
                bc_log(Error, "Failed to reset MySQL connection");
                break;
            }
            usleep(100000); // 100ms delay before retry
            continue;
        }
        // ... other error handling
    }
    return ret;
}
```

### 3. PHP Database Layer Improvements

#### Main Database Class (`www/lib/lib.php`)
- **Connection Pooling**: Improved connection management with retry logic
- **Health Checking**: `mysqli_ping()` validation before operations
- **Timeout Configuration**: Configurable connection timeouts (10-30 seconds)
- **Session Settings**: Automatic setting of MySQL session timeouts

#### Standalone Database Class (`www/lib/db.php`)
- **Consistent Implementation**: Same improvements as main database class
- **Error Recovery**: Automatic reconnection on connection loss
- **Resource Management**: Proper cleanup of result sets

```php
// CRITICAL FIX: Improved connection with retry logic and health checking
private function connect() {
    $this->load_config();
    
    $retries = 0;
    while ($retries < $this->max_retries) {
        $this->dblink = mysqli_init();
        if (!$this->dblink) {
            $retries++;
            continue;
        }
        
        // Set connection options for better reliability
        mysqli_options($this->dblink, MYSQLI_OPT_CONNECT_TIMEOUT, $this->connection_timeout);
        mysqli_options($this->dblink, MYSQLI_OPT_READ_TIMEOUT, 30);
        mysqli_options($this->dblink, MYSQLI_OPT_WRITE_TIMEOUT, 30);
        
        $this->dblink = mysqli_real_connect($this->dblink, $this->dbhost, $this->dbuser, $this->dbpassword, $this->dbname);
        
        if ($this->dblink) {
            // Set charset and session options
            mysqli_real_query($this->dblink, "set names utf8;");
            mysqli_real_query($this->dblink, "SET SESSION wait_timeout=300;");
            mysqli_real_query($this->dblink, "SET SESSION interactive_timeout=300;");
            return;
        }
        
        $retries++;
        if ($retries < $this->max_retries) {
            usleep(100000); // 100ms delay before retry
        }
    }
}
```

### 4. MySQL/MariaDB Configuration Optimization

#### Integrated Installation Optimization (`misc/postinstall.sh`)
- **Automatic Detection**: Automatically detects whether MySQL or MariaDB is installed
- **MariaDB Support**: Full support for MariaDB on Debian/Ubuntu systems
- **Configuration File Detection**: Supports multiple configuration file locations for both MySQL and MariaDB
- **Service Management**: Handles both `mysql` and `mariadb` service names
- **Backup Creation**: Automatic backup of original configuration
- **Service Restart**: Automatic restart of database service to apply changes
- **Error Handling**: Graceful handling if database configuration cannot be found

#### Supported Database Systems:
- **MySQL**: Standard MySQL server installations
- **MariaDB**: MariaDB server (common on Debian/Ubuntu systems)
- **Configuration Files**: Supports multiple standard locations for both systems

#### Key MySQL/MariaDB Settings Applied:
```ini
[mysqld]
max_connections = 200
wait_timeout = 300
interactive_timeout = 300
connect_timeout = 10
net_read_timeout = 30
net_write_timeout = 30

# InnoDB settings for better transaction handling
innodb_buffer_pool_size = 256M
innodb_log_file_size = 64M
innodb_flush_log_at_trx_commit = 2
innodb_lock_wait_timeout = 50
innodb_rollback_on_timeout = ON

# Performance settings (supported by both MySQL and MariaDB)
query_cache_type = 1
query_cache_size = 32M
max_allowed_packet = 16M
table_open_cache = 2000
thread_cache_size = 8
```

## Performance Improvements

### Before (3.1.13)
- Single global database connection
- 5-second connection timeouts
- "Commands out of sync" errors
- No connection health monitoring
- Transaction conflicts under load
- PHP connection failures

### After (3.1.14)
- Up to 10 pooled database connections
- 30-second connection timeouts
- Automatic error recovery and retry
- Active connection health monitoring
- Proper transaction synchronization
- Robust PHP database handling

## Migration Guide

### 1. Automatic MySQL Configuration
MySQL optimization is now **automatic** during BlueCherry installation and upgrade. The system will:
- Detect your MySQL configuration file
- Create a backup of original settings
- Apply BlueCherry-optimized settings
- Restart MySQL service to apply changes

### 2. Verify Installation
Check that the optimizations are working:
```bash
# Check MySQL status
systemctl status mysql

# Monitor connections
mysql -e 'SHOW STATUS LIKE "Threads_connected";'

# Check slow queries
tail -f /var/log/mysql/slow.log
```

### 3. Manual Optimization (if needed)
If you need to manually reapply optimizations:
```bash
# The optimization function is available in the postinstall script
sudo /usr/share/bluecherry/postinstall.sh configure
```

### 4. Monitor Performance
- **Connection Count**: Monitor `Threads_connected` status
- **Slow Queries**: Check `/var/log/mysql/slow.log`
- **Error Log**: Monitor `/var/log/mysql/error.log`
- **Server Logs**: Watch for database-related errors

## Troubleshooting

### Connection Issues
- **High Connection Count**: Check for connection leaks in application
- **Connection Timeouts**: Verify MySQL timeout settings
- **Connection Refused**: Check MySQL service status and firewall

### Transaction Issues
- **"Commands out of sync"**: Usually resolved by connection pool improvements
- **Lock Timeouts**: Check for long-running transactions
- **Transaction Failures**: Monitor error logs for specific error messages

### Performance Issues
- **Slow Queries**: Check slow query log for optimization opportunities
- **High CPU Usage**: Monitor MySQL process list for long-running queries
- **Memory Issues**: Check InnoDB buffer pool usage

## Configuration Options

### Connection Pool Settings
```cpp
#define MAX_CONNECTIONS 10           // Maximum pooled connections
#define CONNECTION_TIMEOUT 300       // Connection idle timeout (seconds)
#define CONNECTION_CHECK_INTERVAL 60 // Health check interval (seconds)
```

### PHP Database Settings
```php
private $connection_timeout = 10; // Connection timeout (seconds)
private $max_retries = 3;        // Maximum retry attempts
```

### MySQL Settings
- **max_connections**: 200 (adjust based on server capacity)
- **wait_timeout**: 300 seconds
- **innodb_buffer_pool_size**: 256M (adjust based on available RAM)

## Conclusion

The database optimization in 3.1.14 resolves the critical connection and transaction issues by:

1. **Implementing connection pooling** for better concurrency
2. **Adding comprehensive error recovery** and retry logic
3. **Improving transaction synchronization** to prevent conflicts
4. **Optimizing MySQL configuration** for BlueCherry workloads
5. **Adding connection health monitoring** for proactive issue detection

These improvements ensure stable operation on large installations with many cameras while maintaining excellent performance and reliability. 