#include <stdio.h>
#include <stdlib.h>

#define KEY_LENGTH 80
#define IV_LENGTH 80
#define STATE_LENGTH 288

typedef struct {
    int state[STATE_LENGTH];
} Trivium;

// Function to write an integer to a file
void writeIntegerToFile(const char *filename, int value) {
    FILE *file = fopen(filename, "ab");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fwrite(&value, sizeof(int), 1, file);
    fclose(file);
}

// Function to set up the key for Trivium
void key_setup(Trivium *trivium, int *key) {
    int i;
    for (i = 0; i < KEY_LENGTH; i++) {
        trivium->state[i] = key[i];
    }
}

// Function to set up the initialization vector (IV) for Trivium
void iv_setup(Trivium *trivium, int *iv) {
    int i;
    for (i = 0; i < IV_LENGTH; i++) {
        trivium->state[i + 93] = iv[i];
    }
}

// Function to rotate elements in an array to the right
void rotate(int *array, int n) {
    int i, temp;
    for (i = 0; i < n; i++) {
        temp = array[STATE_LENGTH - 1];
        for (int j = STATE_LENGTH - 1; j > 0; j--) {
            array[j] = array[j - 1];
        }
        array[0] = temp;
    }
}

// Function to perform the clocking steps in Trivium
void clock(Trivium *trivium) {
    int t1 = trivium->state[65] ^ (trivium->state[90] & trivium->state[91]) ^ trivium->state[92] ^ trivium->state[170];
    int t2 = trivium->state[161] ^ (trivium->state[174] & trivium->state[175]) ^ trivium->state[176] ^ trivium->state[263];
    int t3 = trivium->state[242] ^ (trivium->state[285] & trivium->state[286]) ^ trivium->state[287] ^ trivium->state[68];

    rotate(trivium->state, 1);

    trivium->state[0] = t3;
    for (int i = 93; i < 173; i++) {
        trivium->state[i] = trivium->state[i + 1];
    }
    trivium->state[173] = t1;

    for (int i = 177; i < 257; i++) {
        trivium->state[i] = trivium->state[i + 1];
    }
    trivium->state[257] = t2;
}

// Function to generate keystream from Trivium
void generate_keystream(Trivium *trivium, int *keystream, int length) {
    for (int i = 0; i < length; i++) {
        int t1 = trivium->state[65] ^ trivium->state[92];
        int t2 = trivium->state[161] ^ trivium->state[176];
        int t3 = trivium->state[242] ^ trivium->state[287];

        int z = t1 ^ t2 ^ t3;
        keystream[i] = z;

        clock(trivium);
    }
}

int main() {
    Trivium trivium;

    // Key and IV are defined in hexadecimal format
    int key[KEY_LENGTH] = {0x1A2B3C4D5E6F7890123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF};
    int iv[IV_LENGTH] = {0xFEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210};

    // Set up key and IV for Trivium
    key_setup(&trivium, key);
    iv_setup(&trivium, iv);

    // Set the length of the keystream to 1500
    int length = 1500;// Number of 32-bit words to generate
    int keystream[length];

    // Open file for writing the keystream
    FILE *file = fopen("keystream.txt", "ab");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Generate and write the keystream to the file
    generate_keystream(&trivium, keystream, length);
    for (int i = 0; i < length; i++) {
        writeIntegerToFile("keystream.txt", keystream[i]);
    }

    // Close the file
    fclose(file);
  	printf("Trivium Keystream has been written to keystream.txt\n");
    return 0;
}

