# MySQL/MariaDB Optimization for Large Bluecherry Systems

This guide provides manual optimization instructions for MySQL/MariaDB databases on large Bluecherry surveillance systems (150+ cameras).

## When to Apply These Optimizations

Apply these optimizations if you have:
- **150+ cameras** in your Bluecherry system
- **High recording activity** (24/7 recording, motion detection)
- **Performance issues** (slow web interface, database timeouts)
- **Large storage requirements** (months/years of video retention)

## Pre-Optimization Checklist

1. **Backup your database** before making any changes
2. **Check current system resources**:
   ```bash
   # Check RAM
   free -h
   
   # Check CPU cores
   nproc
   
   # Check disk space
   df -h
   ```
3. **Identify your database type**:
   ```bash
   # Check if MySQL or MariaDB
   systemctl status mysql mariadb
   
   # Check version
   mysql --version
   ```

## System-Specific Optimizations

### Low-Memory Systems (< 4GB RAM)
```ini
[mysqld]
# Connection settings
max_connections = 100
wait_timeout = 300
interactive_timeout = 300
connect_timeout = 10
net_read_timeout = 30
net_write_timeout = 30

# InnoDB settings
innodb_buffer_pool_size = 256M
innodb_log_file_size = 32M
innodb_log_buffer_size = 8M
innodb_flush_log_at_trx_commit = 2
innodb_lock_wait_timeout = 50
innodb_rollback_on_timeout = ON

# Performance settings
max_allowed_packet = 16M
table_open_cache = 1000
thread_cache_size = 8

# Logging (optional)
slow_query_log = 1
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 2
log_error = /var/log/mysql/error.log
```

### Medium-Memory Systems (4-8GB RAM)
```ini
[mysqld]
# Connection settings
max_connections = 200
wait_timeout = 300
interactive_timeout = 300
connect_timeout = 10
net_read_timeout = 30
net_write_timeout = 30

# InnoDB settings
innodb_buffer_pool_size = 1G
innodb_log_file_size = 64M
innodb_log_buffer_size = 16M
innodb_flush_log_at_trx_commit = 2
innodb_lock_wait_timeout = 50
innodb_rollback_on_timeout = ON

# Performance settings
max_allowed_packet = 16M
table_open_cache = 2000
thread_cache_size = 32

# Logging (optional)
slow_query_log = 1
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 2
log_error = /var/log/mysql/error.log
```

### High-Memory Systems (8GB+ RAM)
```ini
[mysqld]
# Connection settings
max_connections = 300
wait_timeout = 300
interactive_timeout = 300
connect_timeout = 10
net_read_timeout = 30
net_write_timeout = 30

# InnoDB settings
innodb_buffer_pool_size = 2G
innodb_log_file_size = 128M
innodb_log_buffer_size = 32M
innodb_flush_log_at_trx_commit = 2
innodb_lock_wait_timeout = 50
innodb_rollback_on_timeout = ON

# Performance settings
max_allowed_packet = 32M
table_open_cache = 4000
thread_cache_size = 64

# Logging (optional)
slow_query_log = 1
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 2
log_error = /var/log/mysql/error.log
```

## MariaDB-Specific Settings

For MariaDB systems, you can also add query cache settings:

```ini
[mysqld]
# Query cache (MariaDB only)
query_cache_type = 1
query_cache_size = 32M
query_cache_limit = 2M
```

## Client Settings

Add these settings for better client connections:

```ini
[client]
connect_timeout = 10
read_timeout = 30
write_timeout = 30
```

## Step-by-Step Application

### 1. Locate Your Configuration File

**MySQL:**
```bash
# Common locations
/etc/mysql/mysql.conf.d/mysqld.cnf
/etc/mysql/my.cnf
/etc/my.cnf
```

**MariaDB:**
```bash
# Common locations
/etc/mysql/mariadb.conf.d/50-server.cnf
/etc/mysql/mariadb.conf.d/mysqld.cnf
/etc/mysql/my.cnf
/etc/my.cnf
```

### 2. Create Backup

```bash
# Create backup with timestamp
sudo cp /etc/mysql/mysql.conf.d/mysqld.cnf /etc/mysql/mysql.conf.d/mysqld.cnf.backup.$(date +%Y%m%d_%H%M%S)
```

### 3. Apply Optimizations

1. **Edit the configuration file**:
   ```bash
   sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf
   ```

2. **Add the appropriate settings** from the sections above

3. **Save and exit**

### 4. Validate Configuration

**MySQL:**
```bash
sudo mysqld --defaults-file=/etc/mysql/mysql.conf.d/mysqld.cnf --validate-config
```

**MariaDB:**
```bash
sudo mariadb-check --config-file=/etc/mysql/mariadb.conf.d/50-server.cnf
```

### 5. Restart Database Service

```bash
# For MySQL
sudo systemctl restart mysql

# For MariaDB
sudo systemctl restart mariadb
```

### 6. Verify Service Status

```bash
# Check if service is running
sudo systemctl status mysql
# or
sudo systemctl status mariadb

# Test connection
mysql -u root -p -e "SELECT VERSION();"
```

## Monitoring and Troubleshooting

### Check Current Settings

```sql
-- Check current InnoDB settings
SHOW VARIABLES LIKE 'innodb_%';

-- Check connection settings
SHOW VARIABLES LIKE 'max_connections';
SHOW VARIABLES LIKE 'wait_timeout';

-- Check performance counters
SHOW STATUS LIKE 'Threads_connected';
SHOW STATUS LIKE 'Innodb_buffer_pool_read_requests';
```

### Monitor Performance

```bash
# Check slow query log
sudo tail -f /var/log/mysql/slow.log

# Monitor MySQL processes
mysql -u root -p -e "SHOW PROCESSLIST;"

# Check system resources
htop
iotop
```

### Common Issues and Solutions

**Issue: "Too many connections"**
- Increase `max_connections` value
- Check for connection leaks in applications

**Issue: Slow queries**
- Enable slow query log
- Analyze slow queries and add indexes
- Increase `innodb_buffer_pool_size`

**Issue: Database timeouts**
- Increase `wait_timeout` and `interactive_timeout`
- Check network connectivity

**Issue: Out of memory errors**
- Reduce `innodb_buffer_pool_size`
- Check system memory usage
- Consider adding more RAM

## Reverting Changes

If you need to revert the optimizations:

```bash
# Restore from backup
sudo cp /etc/mysql/mysql.conf.d/mysqld.cnf.backup.* /etc/mysql/mysql.conf.d/mysqld.cnf

# Restart service
sudo systemctl restart mysql
```

## Additional Recommendations

### For Very Large Systems (500+ cameras)

1. **Consider database partitioning** for large tables
2. **Implement read replicas** for reporting
3. **Use SSD storage** for database files
4. **Monitor and tune regularly** based on usage patterns

### Regular Maintenance

```bash
# Weekly maintenance script
#!/bin/bash
# Optimize tables
mysql -u root -p -e "OPTIMIZE TABLE Events;"
mysql -u root -p -e "OPTIMIZE TABLE Events_Archived;"

# Analyze table statistics
mysql -u root -p -e "ANALYZE TABLE Events;"
mysql -u root -p -e "ANALYZE TABLE Events_Archived;"
```

## Support

If you encounter issues after applying these optimizations:

1. **Check the error logs**: `/var/log/mysql/error.log`
2. **Restore from backup** if necessary
3. **Contact Bluecherry support** with specific error messages
4. **Provide system specifications** and optimization settings used

---

**Note**: These optimizations are recommendations based on typical Bluecherry usage patterns. Your specific requirements may vary based on your hardware, camera count, recording settings, and usage patterns. 