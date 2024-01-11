#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 7004
#define BUFFER_SIZE 2048
#define LICENSE_KEY_LENGTH 41

void error_handling(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void print_help() {
    printf("Available commands:\n");
    printf("bc_v3_license_isActivated = Is License Activated\n");
    printf("bc_v3_license_isLicenseGenuine = Is License Genuine\n");
    printf("bc_v3_license_IsTrialGenuine = Is Trial Genuine\n");
    printf("bc_v3_license_GetLicenseMetadata = Return amount of licenses (Message from server: bc_v3_license_GetLicenseMetadata 1 == No license configured, or problem with license / Message from server: bc_v3_license_GetLicenseMetadata 0 12 == License good | 12 ports)\n");
    printf("bc_v3_license_GetLicenseExpiryDate = Not used, all licenses run for ever.\n");
    printf("bc_v3_license_GetTrialExpiryDate = Trial expiry date...we only use this for trials\n");
    printf("bc_v3_license_ActivateLicense = Activate license\n");
    printf("bc_v3_license_ActivateTrial = Activate trial license, good for 30 days\n");
    printf("bc_v3_license_DeactivateLicense = Deactivate license.\n");
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUFFER_SIZE];
    int str_len;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    if (argc != 3) {
        fprintf(stderr, "Usage: %s command licensekey\n  Use --help for available commands\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Validate the license key length
    if (strlen(argv[2]) != LICENSE_KEY_LENGTH) {
        fprintf(stderr, "Invalid license key length. Expected %d characters.\n", LICENSE_KEY_LENGTH);
        exit(EXIT_FAILURE);
    }

    // Ensure the total message size does not exceed BUFFER_SIZE
    if (strlen(argv[1]) + 1 + LICENSE_KEY_LENGTH >= BUFFER_SIZE) {
        fprintf(stderr, "Command and license key are too long.\n");
        exit(EXIT_FAILURE);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error!");
    }

    if (snprintf(message, BUFFER_SIZE, "%s %s", argv[1], argv[2]) >= BUFFER_SIZE) {
        error_handling("Message too long");
    }

    if (send(sock, message, strlen(message), 0) == -1) {
        error_handling("send() error!");
    }

    str_len = recv(sock, message, BUFFER_SIZE - 1, 0);
    if (str_len == -1) {
        error_handling("recv() error!");
    }

    message[str_len] = '\0';
    printf("Message from server: %s\n", message);

    close(sock);
    return EXIT_SUCCESS;
}


