#include <unistd.h>

// From lib/
#define GENERATE_LICENSES 1
#define BC_KEY_STANDALONE 1
#include "bc-key.cpp"

extern char *__progname;

static void usage()
{
	fprintf(stderr, "Generate license:  %s -g[num] -n[ports]\n", __progname);
	fprintf(stderr, "Authorize license: %s -a[license] -m[machine]\n", __progname);
	exit(1);
}

int main(int argc, char **argv)
{
	int gen_n_ports = 0;
	int gen_count = 0;
	const char *auth_license = 0;
	const char *auth_machine = 0;

	int opt;
	while ((opt = getopt(argc, argv, "n:g::a:m:h")) != -1) {
		switch (opt) {
		case 'n': gen_n_ports = atoi(optarg); break;
		case 'g': gen_count = optarg ? atoi(optarg) : 1; break;
		case 'a': auth_license = optarg; break;
		case 'm': auth_machine = optarg; break;
		case 'h': default: usage();
		}
	}

	/* Seed the random number generator from /dev/urandom */
	FILE *rnd = fopen("/dev/urandom", "r");
	if (!rnd)
		perror("Cannot open /dev/urandom");
	unsigned int seed = 0;
	if (fread(&seed, sizeof(seed), 1, rnd) < 1)
		perror("Cannot read /dev/urandom");
	fclose(rnd);
	srand(seed);

	/* Generate keys if requested */
	if (gen_count > 0 || gen_n_ports > 0) {
		if (gen_count < 1 || gen_n_ports < 1 || gen_n_ports > 32)
			usage();

		for (int i = 0; i < gen_count; i++) {
			char buf[64];
			bc_license_generate(buf, sizeof(buf), gen_n_ports);
			printf("%s # %d ports\n", buf, gen_n_ports);
		}
	} else if (auth_license || auth_machine) {
		if (!auth_license || !auth_machine)
			usage();

		char buf[64];
		if (bc_license_generate_auth(buf, sizeof(buf), auth_license, auth_machine) < 1) {
			fprintf(stderr, " Invalid input for authorization\n");
			exit(1);
		}
		printf("%s # authorize %s for machine %s\n", buf, auth_license, auth_machine);
	} else
		usage();

	return 0;
}

