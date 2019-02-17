#include <stdio.h>
#include <stdlib.h>

struct Abyss{
	int depth;
	char message[0x90];
};

struct Abyss *Abysslist[10];
int poison_flag[10] = {0};
int abyss_count=0;

void shout(int cnt)
{
    unsigned int len = (Abysslist[cnt]->depth) - (Abysslist[cnt-1]->depth) - 1;
    printf("shout to deep blue abyss : ");
	while( getchar() != '\n' );
    read(0, Abysslist[cnt]->message, len);
    printf("your shout echoes back to you : %s\n", Abysslist[cnt]->message);
    return;
}


void menu()
{
	puts("1. Dive into the deeper abyss");
	puts("2. Go Up");
	puts("3. Leave message");
	puts("4. Poison the abyss");
	puts("5. Touch the sky\n");

	printf(">> ");

	return;
}

void intro()
{
	puts("\"If you gaze long into an abyss, the abyss also gazes into you.\"\n");
	puts("                             Beyond Good and Evil, Aphorism 146\n");

	return;
}

void go_deep()
{
	int deep;
	int i;

	printf("how deep do you want  : ");
	scanf("%d", &deep);

	if ((abyss_count + deep) > 10) {puts("too deep"); exit(-1);}
	for (i = 1; i<=deep; i++)
	{
		if (!Abysslist[abyss_count+i]) Abysslist[abyss_count+i] = (struct Abyss*)malloc(sizeof(struct Abyss));	
		Abysslist[abyss_count+i]-> depth = (abyss_count + i) * 137;
	}
	abyss_count += deep;	

	return;
}

void final(int cnt)
{
	if (cnt == 10){
		puts("You've reached the end of the abyss. Time to leave.");
		
        long long int size;
		unsigned long long int *msg;

        printf("how deep are you? : ");
        scanf("%lld", &size);

        malloc(size);
		msg = malloc(0x30);

        printf("your final words : ");
		read(0, msg, 0x30);
		
		puts("\nGoodbye, cruel world.");
		free(msg);

		exit(1);
	}

	return;
}

void poison(int cnt)
{
	printf("abyss %d poisoned. now nobody can approach.\n\n", cnt);
	free(Abysslist[cnt]);
	poison_flag[cnt] = 1;
	
	return;
}

int main()
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
	
	char buf[4];

	intro();
	while(1){
        if (!Abysslist[abyss_count] && abyss_count != 10) Abysslist[abyss_count] = (struct Abyss*)calloc(1, sizeof(struct Abyss));	
		if (abyss_count != 10) Abysslist[abyss_count]-> depth = abyss_count * 137;
		if (!abyss_count) printf("you need some oxygen : %p\n", Abysslist[0]);
        final(abyss_count);

		printf("You are now in Abyss %d - depth : %d m\n\n", abyss_count, Abysslist[abyss_count]->depth);

        menu();

		read(0, buf, 4);
		switch(atoi(buf)){
			case 1:
				go_deep();
				break;
			case 2:
				if (!abyss_count){
					puts("Where u going?");
					exit(-1);
				}
				if (poison_flag[abyss_count-1]){
					puts("you poisoned yourself.");
					exit(-1);
                }
                abyss_count--;
				break;
			case 3:
				if (!abyss_count){
				puts("you are not deep enough.\n");
				break;
				}
                shout(abyss_count);
				break;
			case 4:
				poison(abyss_count);
				abyss_count++;
				break;
			case 5:
				puts("Shiny sunlight greets you, as always.");
				exit(1);
			default:
				puts("invalid");
				exit(0);
		}
	}
	return 0;
}
