#!/bin/bash
set -euo pipefail

# Script to change permissions and ownership of a directory, with enhanced safety checks

# List of protected directories
PROTECTED_DIRS=(
    "/"
    "/bin"
    "/boot"
    "/dev"
    "/etc"
    "/home"
    "/lib"
    "/lib64"
    "/opt"
    "/proc"
    "/root"
    "/run"
    "/sbin"
    "/srv"
    "/sys"
    "/tmp"
    "/usr"
    "/var"
)

# Minimum required free space in MB
MIN_FREE_SPACE=1024  # 1GB minimum free space

# Verify if a directory is "protected"
is_subdir() {
    local dir=$1
    local protected_dir=$2
    while [[ "$dir" != "/" ]]; do
        if [[ "$dir" == "$protected_dir" ]]; then
            return 0 # True, is a subdir
        fi
        dir=$(dirname "$dir")
    done
    return 1 # False, not a subdir
}

# Check if path is valid
is_valid_path() {
    local path=$1
    # Check for invalid characters
    if [[ "$path" =~ [\|\&\;\$\#\*\(\)\{\}\[\]\<\>] ]]; then
        return 1
    fi
    return 0
}

# Check available disk space
check_disk_space() {
    local path=$1
    local free_space=$(df -m "$path" | awk 'NR==2 {print $4}')
    if [ "$free_space" -lt "$MIN_FREE_SPACE" ]; then
        echo "Error: Insufficient disk space. Required: ${MIN_FREE_SPACE}MB, Available: ${free_space}MB"
        return 1
    fi
    return 0
}

# Verify write permissions
verify_write_permissions() {
    local path=$1
    if ! touch "$path/.write_test" 2>/dev/null; then
        echo "Error: Cannot write to directory $path"
        return 1
    fi
    rm -f "$path/.write_test"
    return 0
}

# Directory variable to store location
DIR=$(realpath "$1") # Converts to absolute path

# Validate path format
if ! is_valid_path "$DIR"; then
    echo "Error: Invalid path format. Path must contain no special characters."
    exit 1
fi

# We need to atleast allow storage for the default storage directory which is inside a protected directory /var/lib/bluecherry/recordings
if [[ "$DIR" == "/var/lib/bluecherry/recordings" ]]; then
    echo "Modifying permissions and ownership of $DIR"
    chmod 770 "$DIR"
    chown -R bluecherry:bluecherry "$DIR"
    if ! verify_write_permissions "$DIR"; then
        echo "Error: Failed to verify write permissions after setting them"
        exit 1
    fi
    exit 0
fi

# Check if the directory is a subdirectory of any protected directory
for protected_dir in "${PROTECTED_DIRS[@]}"; do
    if is_subdir "$DIR" "$protected_dir"; then
        # Allow /var/lib/bluecherry/recordings but disallow others in /var
        if [[ "$protected_dir" == "/var" && "$DIR" != "/var/lib/bluecherry/recordings" ]]; then
            echo "Error: Attempted to modify restricted directory $DIR. Operation aborted."
            exit 1
        elif [[ "$protected_dir" != "/var" ]]; then
            echo "Error: Attempted to modify directory $DIR inside protected path $protected_dir. Operation aborted."
            exit 1
        fi
    fi
done

# Check if the directory exists
if [[ -d "$DIR" ]]; then
    # Check the permissions and ownership
    PERMS=$(stat -c "%a" "$DIR")
    OWNER=$(stat -c "%U.%G" "$DIR")

    # Check if permissions are not 770 or owner is not bluecherry.bluecherry
    if [[ "$PERMS" != "770" || "$OWNER" != "bluecherry.bluecherry" ]]; then
        echo "Changing permissions and ownership of $DIR"
        chmod 770 "$DIR"
        chown -R bluecherry:bluecherry "$DIR"
        if ! verify_write_permissions "$DIR"; then
            echo "Error: Failed to verify write permissions after setting them"
            exit 1
        fi
    else
        echo "Directory $DIR already has the correct permissions and ownership."
    fi
elif [[ -e "$DIR" ]]; then
    echo "$DIR already exists but is not a directory."
    exit 1
else
    echo "Directory $DIR does not exist. Creating it now..."
    mkdir -p "$DIR"
    chmod 770 "$DIR"
    chown -R bluecherry:bluecherry "$DIR"
    if ! verify_write_permissions "$DIR"; then
        echo "Error: Failed to verify write permissions after creating directory"
        exit 1
    fi
fi

# Check available disk space
if ! check_disk_space "$DIR"; then
    echo "Error: Insufficient disk space for storage location"
    exit 1
fi

echo "Successfully configured storage location: $DIR"
exit 0
