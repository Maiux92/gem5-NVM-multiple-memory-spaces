volatile int __attribute__((section(".NVM"))) test[300];
int i;

int main() {
    printf("Starting!\n");

    for(i = 0; i < 300; i++) {
        test[i] = i;
    }

    for(i = 0; i < 300; i++) {
        printf("%d ", test[i]);
    }

    printf("Done!\n");
}
