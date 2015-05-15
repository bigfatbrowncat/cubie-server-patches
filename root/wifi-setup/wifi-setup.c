#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ESC_RED_BOLD "\x1b[31;1m"
#define ESC_GREEN_BOLD "\x1b[32;1m"
#define ESC_YELLOW "\x1b[33m"
#define ESC_GRAY_BOLD "\x1b[37;1m"
#define ESC_DEFAULT "\x1b[0m"

#define WPA_CONF_NAME "/etc/wpa_supplicant.conf"

#define RES_OK		0
#define RES_CANCEL	1
#define RES_ERROR	2

void signalsHandler(int sig) {
	if (sig == SIGINT) {
		printf(ESC_RED_BOLD "\nProcess cancelled by the user\n" ESC_DEFAULT);
		exit(RES_CANCEL);
	} else if (sig == SIGTERM) {
		printf(ESC_RED_BOLD "\nProcess terminated\n" ESC_DEFAULT);
		exit(RES_CANCEL);
	}
}

void configSignalHandler() {
	struct sigaction sigHandler;
	sigHandler.sa_handler = signalsHandler;
	sigemptyset(&sigHandler.sa_mask);
	sigHandler.sa_flags = 0;

	sigaction(SIGINT, &sigHandler, NULL);
	sigaction(SIGTERM, &sigHandler, NULL);
}

int main() {
	configSignalHandler();

	printf(ESC_YELLOW "\nLinaro server Wi-Fi configurator.\nCopyright (C) Ilya Mizus, 2015" ESC_DEFAULT "\n");
	char networkName[256];
	char networkPSK[256];

	printf("\nWi-Fi point name (SSID): ");
	printf(ESC_GRAY_BOLD);
	scanf("%255s", networkName);
	printf(ESC_DEFAULT);

	printf("Password (PSK): ");
	printf(ESC_GRAY_BOLD);
	scanf("%255s", networkPSK);
	printf(ESC_DEFAULT);

	printf("\nGenerating %s... ", WPA_CONF_NAME);

        FILE* wpaConfFile = fopen(WPA_CONF_NAME, "w");
        if (wpaConfFile == NULL) {
                printf(ESC_RED_BOLD "\nCan't open file \"%s\" for writing. Oops..." ESC_DEFAULT "\n", WPA_CONF_NAME);
                return RES_ERROR;
        }

	fprintf(wpaConfFile, "network={\n");
	fprintf(wpaConfFile, "    ssid=\"%s\"\n", networkName);
	fprintf(wpaConfFile, "    psk=\"%s\"\n", networkPSK);
	fprintf(wpaConfFile, "}\n");
	printf(ESC_GREEN_BOLD "Success\n" ESC_DEFAULT);

	fclose(wpaConfFile);

	printf("Restarting wlan0... \n\n"); fflush(stdout);

	int ret = system("ifdown wlan0 && ifup wlan0");
	if (ret == 0) {
		printf(ESC_GREEN_BOLD "\nwlan0 restarted successfully\n" ESC_DEFAULT);
	} else {
		printf(ESC_RED_BOLD "\nwlan0 restarting failed\n" ESC_DEFAULT);
	}

	return RES_OK;
}
